#include "QCefBrowserSetting.h"
#include <QCoreApplication>
#include <QDir>

QCefBrowserSetting::QCefBrowserSetting()
    : osrEnabled(true)
    , contextMenuEnabled(true)
    , autoShowDevToolsContextMenu(true)
    , executeUnknownProtocolViaOS(true)
    , devToolsResourceExist(false)
    , fps(25)
    , backgroundColor(255, 255, 255, 255) {
}

QCefBrowserSetting::~QCefBrowserSetting() {}
