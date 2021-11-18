#ifndef QCEF_BROWSER_SETTING_H_
#define QCEF_BROWSER_SETTING_H_

#include <QColor>

class QCefBrowserSetting {
 public:
  QCefBrowserSetting();
  ~QCefBrowserSetting();

  bool osrEnabled;
  bool contextMenuEnabled;
  bool executeUnknownProtocolViaOS;
  bool autoShowDevToolsContextMenu;
  bool devToolsResourceExist;
  bool autoDestroyCefWhenCloseEvent;

  int fps;
  QColor backgroundColor;
  QString consoleLogPath;
};
#endif  // !QCEF_BROWSER_SETTING_H_