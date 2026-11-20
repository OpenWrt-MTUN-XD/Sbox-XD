#pragma once

#include "Const.hpp"
#include "SboxXD_Utils.hpp"
#include "SboxXD_ConfigItem.hpp"
#include "SboxXD_DataStore.hpp"

// Switch core support

namespace SboxXD {
    inline int coreType = CoreType::SING_BOX;

    QString FindCoreAsset(const QString &name);

    QString FindSboxXDCoreRealPath();

    bool IsAdmin();
} // namespace SboxXD

#define ROUTES_PREFIX_NAME QStringLiteral("routes_box")
#define ROUTES_PREFIX QString(ROUTES_PREFIX_NAME + "/")
