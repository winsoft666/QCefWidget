#include "QCefBrowserSetting.h"

QCefBrowserSetting::QCefBrowserSetting()
    : osrEnabled(true)
    , contextMenuEnabled(true)
    , executeUnknownProtocolViaOS(true)
    , fps(25)
    , backgroundColor(255, 255, 255, 255) {}

QCefBrowserSetting::~QCefBrowserSetting() {}
