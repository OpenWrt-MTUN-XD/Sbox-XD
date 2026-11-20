#include "dialog_vpn_settings.h"
#include "ui_dialog_vpn_settings.h"

#include "main/GuiUtils.hpp"
#include "main/SboxXD.hpp"
#include "ui/mainwindow_interface.h"

#include <QMessageBox>

DialogVPNSettings::DialogVPNSettings(QWidget *parent) : QDialog(parent), ui(new Ui::DialogVPNSettings) {
    ui->setupUi(this);
    ADD_ASTERISK(this);

    ui->fake_dns->setChecked(SboxXD::dataStore->fake_dns);
    ui->vpn_implementation->setCurrentIndex(SboxXD::dataStore->vpn_implementation);
    ui->vpn_mtu->setCurrentText(Int2String(SboxXD::dataStore->vpn_mtu));
    ui->vpn_ipv6->setChecked(SboxXD::dataStore->vpn_ipv6);
    ui->hide_console->setChecked(SboxXD::dataStore->vpn_hide_console);
#ifndef Q_OS_WIN
    ui->hide_console->setVisible(false);
#endif
    ui->strict_route->setChecked(SboxXD::dataStore->vpn_strict_route);
    ui->single_core->setChecked(SboxXD::dataStore->vpn_internal_tun);
    //
    D_LOAD_STRING_PLAIN(vpn_rule_cidr)
    D_LOAD_STRING_PLAIN(vpn_rule_process)
    //
    connect(ui->whitelist_mode, &QCheckBox::stateChanged, this, [=](int state) {
        if (state == Qt::Checked) {
            ui->gb_cidr->setTitle(tr("Proxy CIDR"));
            ui->gb_process_name->setTitle(tr("Proxy Process Name"));
        } else {
            ui->gb_cidr->setTitle(tr("Bypass CIDR"));
            ui->gb_process_name->setTitle(tr("Bypass Process Name"));
        }
    });
    ui->whitelist_mode->setChecked(SboxXD::dataStore->vpn_rule_white);
}

DialogVPNSettings::~DialogVPNSettings() {
    delete ui;
}

void DialogVPNSettings::accept() {
    //
    auto mtu = ui->vpn_mtu->currentText().toInt();
    if (mtu > 10000 || mtu < 1000) mtu = 9000;
    SboxXD::dataStore->vpn_implementation = ui->vpn_implementation->currentIndex();
    SboxXD::dataStore->fake_dns = ui->fake_dns->isChecked();
    SboxXD::dataStore->vpn_mtu = mtu;
    SboxXD::dataStore->vpn_ipv6 = ui->vpn_ipv6->isChecked();
    SboxXD::dataStore->vpn_hide_console = ui->hide_console->isChecked();
    SboxXD::dataStore->vpn_strict_route = ui->strict_route->isChecked();
    SboxXD::dataStore->vpn_rule_white = ui->whitelist_mode->isChecked();
    bool isInternalChanged = SboxXD::dataStore->vpn_internal_tun != ui->single_core->isChecked();
    SboxXD::dataStore->vpn_internal_tun = ui->single_core->isChecked();
    //
    D_SAVE_STRING_PLAIN(vpn_rule_cidr)
    D_SAVE_STRING_PLAIN(vpn_rule_process)
    //
    QStringList msg{"UpdateDataStore"};
    if (isInternalChanged) {
        msg << "NeedRestart";
    } else {
        msg << "VPNChanged";
    }
    MW_dialog_message("", msg.join(","));
    QDialog::accept();
}

void DialogVPNSettings::on_troubleshooting_clicked() {
    auto r = QMessageBox::information(this, tr("Troubleshooting"),
                                      tr("If you have trouble starting VPN, you can force reset sboxxd_core process here.\n\n"
                                         "If still not working, see documentation for more information.\n"
                                         "https://github.com/OpenWrt-MTUN-XD/Sbox-XD/tree/main/docs"),
                                      tr("Reset"), tr("Cancel"), "",
                                      1, 1);
    if (r == 0) {
        GetMainWindow()->StopVPNProcess(true);
    }
}
