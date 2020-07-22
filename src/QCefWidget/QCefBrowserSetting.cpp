#include "QCefBrowserSetting.h"

QCefBrowserSetting::QCefBrowserSetting()
    : osrEnabled(true)
    , contextMenuEnabled(true)
    , autoShowDevToolsContextMenu(true)
    , executeUnknownProtocolViaOS(true)
    , devToolsResourceExist(false)
    , fps(25)
    , presetDeviceScaleFactor(0.f)
    , usingDeviceScaleFactor(0.f)
    , backgroundColor(255, 255, 255, 255) {}

QCefBrowserSetting::~QCefBrowserSetting() {}
