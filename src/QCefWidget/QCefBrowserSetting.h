#ifndef QCEF_BROWSER_SETTING_H_
#define QCEF_BROWSER_SETTING_H_
#pragma once

#include <QColor>

class QCefBrowserSetting {
public:
  QCefBrowserSetting();
  ~QCefBrowserSetting();

  bool osrEnabled;
  bool contextMenuEnabled;
  bool executeUnknownProtocolViaOS;

  int fps;
  QColor backgroundColor;
};
#endif // !QCEF_BROWSER_SETTING_H_