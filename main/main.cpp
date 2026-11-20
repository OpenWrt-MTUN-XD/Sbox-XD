#include <csignal>

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QTranslator>
#include <QMessageBox>
#include <QStandardPaths>
#include <QLocalSocket>
#include <QLocalServer>
#include <QThread>

#include "3rdparty/RunGuard.hpp"
#include "main/SboxXD.hpp"

#include "ui/mainwindow_interface.h"

#ifdef Q_OS_WIN
#include "sys/windows/MiniDump.h"
#endif

void signal_handler(int signum) {
    if (qApp) {
        GetMainWindow()->on_commitDataRequest();
        qApp->exit();
    }
}

QTranslator* trans = nullptr;
QTranslator* trans_qt = nullptr;

void loadTranslate(const QString& locale) {
    if (trans != nullptr) {
        trans->deleteLater();
    }
    if (trans_qt != nullptr) {
        trans_qt->deleteLater();
    }
    //
    trans = new QTranslator;
    trans_qt = new QTranslator;
    QLocale::setDefault(QLocale(locale));
    //
    if (trans->load(":/translations/" + locale + ".qm")) {
        QCoreApplication::installTranslator(trans);
    }
    if (trans_qt->load(QApplication::applicationDirPath() + "/qtbase_" + locale + ".qm")) {
        QCoreApplication::installTranslator(trans_qt);
    }
}

#define LOCAL_SERVER_PREFIX "sboxxdlocalserver-"

int main(int argc, char* argv[]) {
    // Core dump
#ifdef Q_OS_WIN
    Windows_SetCrashHandler();
#endif

    // pre-init QApplication
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0) && QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    QApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    QApplication::setAttribute(Qt::AA_DontUseNativeDialogs);
#endif
    QApplication::setQuitOnLastWindowClosed(false);
    auto preQApp = new QApplication(argc, argv);

    // Clean
    QDir::setCurrent(QApplication::applicationDirPath());
    if (QFile::exists("updater.old")) {
        QFile::remove("updater.old");
    }
#ifndef Q_OS_WIN
    if (!QFile::exists("updater")) {
        QFile::link("launcher", "updater");
    }
#endif

    // Flags
    SboxXD::dataStore->argv = QApplication::arguments();
    if (SboxXD::dataStore->argv.contains("-many")) SboxXD::dataStore->flag_many = true;
    if (SboxXD::dataStore->argv.contains("-appdata")) {
        SboxXD::dataStore->flag_use_appdata = true;
        int appdataIndex = SboxXD::dataStore->argv.indexOf("-appdata");
        if (SboxXD::dataStore->argv.size() > appdataIndex + 1 && !SboxXD::dataStore->argv.at(appdataIndex + 1).startsWith("-")) {
            SboxXD::dataStore->appdataDir = SboxXD::dataStore->argv.at(appdataIndex + 1);
        }
    }
    if (SboxXD::dataStore->argv.contains("-tray")) SboxXD::dataStore->flag_tray = true;
    if (SboxXD::dataStore->argv.contains("-debug")) SboxXD::dataStore->flag_debug = true;
    if (SboxXD::dataStore->argv.contains("-flag_restart_tun_on")) SboxXD::dataStore->flag_restart_tun_on = true;
    if (SboxXD::dataStore->argv.contains("-flag_reorder")) SboxXD::dataStore->flag_reorder = true;
#ifdef SBX_CPP_USE_APPDATA
    SboxXD::dataStore->flag_use_appdata = true; // Example: Package & MacOS
#endif
#ifdef SBX_CPP_DEBUG
    SboxXD::dataStore->flag_debug = true;
#endif

    // dirs & clean
    auto wd = QDir(QApplication::applicationDirPath());
    if (SboxXD::dataStore->flag_use_appdata) {
        QApplication::setApplicationName("Sbox-XD");
        if (!SboxXD::dataStore->appdataDir.isEmpty()) {
            wd.setPath(SboxXD::dataStore->appdataDir);
        } else {
            wd.setPath(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
        }
    }
    if (!wd.exists()) wd.mkpath(wd.absolutePath());
    if (!wd.exists("config")) wd.mkdir("config");
    QDir::setCurrent(wd.absoluteFilePath("config"));
    QDir("temp").removeRecursively();

    // init QApplication
    delete preQApp;
    QApplication a(argc, argv);

    // dispatchers
    DS_cores = new QThread;
    DS_cores->start();

    // RunGuard
    RunGuard guard("sboxxd" + wd.absolutePath());
    quint64 guard_data_in = GetRandomUint64();
    quint64 guard_data_out = 0;
    if (!SboxXD::dataStore->flag_many && !guard.tryToRun(&guard_data_in)) {
        // Some Good System
        if (guard.isAnotherRunning(&guard_data_out)) {
            // Wake up a running instance
            QLocalSocket socket;
            socket.connectToServer(LOCAL_SERVER_PREFIX + Int2String(guard_data_out));
            qDebug() << socket.fullServerName();
            if (!socket.waitForConnected(500)) {
                qDebug() << "Failed to wake a running instance.";
                return 0;
            }
            qDebug() << "connected to local server, try to raise another program";
            return 0;
        }
        // Some Bad System
        QMessageBox::warning(nullptr, "SboxXD", "RunGuard disallow to run, use -many to force start.");
        return 0;
    }
    MF_release_runguard = [&] { guard.release(); };

// icons
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    QIcon::setFallbackSearchPaths(QStringList{
        ":/sboxxd",
        ":/icon",
    });
#endif

    // icon for no theme
    if (QIcon::themeName().isEmpty()) {
        QIcon::setThemeName("breeze");
    }

    // Dir
    QDir dir;
    bool dir_success = true;
    if (!dir.exists("profiles")) {
        dir_success &= dir.mkdir("profiles");
    }
    if (!dir.exists("groups")) {
        dir_success &= dir.mkdir("groups");
    }
    if (!dir.exists(ROUTES_PREFIX_NAME)) {
        dir_success &= dir.mkdir(ROUTES_PREFIX_NAME);
    }
    if (!dir_success) {
        QMessageBox::warning(nullptr, "Error", "No permission to write " + dir.absolutePath());
        return 1;
    }

    // Load dataStore
    switch (SboxXD::coreType) {
        case SboxXD::CoreType::SING_BOX:
            SboxXD::dataStore->fn = "groups/sboxxd.json";
            break;
        default:
            MessageBoxWarning("Error", "Unknown coreType.");
            return 0;
    }
    // 改名兼容：旧版本（NekoBoxPro）的配置文件是 groups/nekobox.json。
    // 新文件还不存在而旧文件在时直接改名继承，避免升级后丢订阅/分组。
    if (!QFile::exists(SboxXD::dataStore->fn) && QFile::exists("groups/nekobox.json")) {
        QFile::rename("groups/nekobox.json", SboxXD::dataStore->fn);
    }
    auto isLoaded = SboxXD::dataStore->Load();
    if (!isLoaded) {
        SboxXD::dataStore->Save();
    }

    // Datastore & Flags
    if (SboxXD::dataStore->start_minimal) SboxXD::dataStore->flag_tray = true;

    // load routing
    SboxXD::dataStore->routing = std::make_unique<SboxXD::Routing>();
    SboxXD::dataStore->routing->fn = ROUTES_PREFIX + SboxXD::dataStore->active_routing;
    isLoaded = SboxXD::dataStore->routing->Load();
    if (!isLoaded) {
        SboxXD::dataStore->routing->Save();
    }

    // Translate
    QString locale;
    switch (SboxXD::dataStore->language) {
        case 1: // English
            break;
        case 2:
            locale = "zh_CN";
            break;
        case 3:
            locale = "fa_IR"; // farsi(iran)
            break;
        case 4:
            locale = "ru_RU"; // Russian
            break;
        default:
            locale = QLocale().name();
    }
    QGuiApplication::tr("QT_LAYOUT_DIRECTION");
    loadTranslate(locale);

    // Signals
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    // QLocalServer
    QLocalServer server;
    auto server_name = LOCAL_SERVER_PREFIX + Int2String(guard_data_in);
    QLocalServer::removeServer(server_name);
    server.listen(server_name);
    QObject::connect(&server, &QLocalServer::newConnection, &a, [&] {
        auto socket = server.nextPendingConnection();
        qDebug() << "nextPendingConnection:" << server_name << socket;
        socket->deleteLater();
        // raise main window
        MW_dialog_message("", "Raise");
    });

    UI_InitMainWindow();
    return QApplication::exec();
}
