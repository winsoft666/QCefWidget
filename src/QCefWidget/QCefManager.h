#ifndef QCEF_MANAGER_H_
#define QCEF_MANAGER_H_
#pragma once

#include "CefBrowserApp/QCefBrowserApp.h"
#include <mutex>
#include <list>
#include <map>
#include <qsystemdetection.h>
#include <QObject>

class QWidget;
class QCefWidgetImpl;
class QCefDevToolsWnd;
class QCefManager : public QObject {
  Q_OBJECT
 public:
  enum BrowserStatus { BS_NOT_CREATE = 0,
                       BS_CREATED,
                       BS_CLOSING,
                       BS_CLOSED };
  static QCefManager& getInstance();
  void initializeCef();
  void uninitializeCef();

  QWidget* addBrowser(QWidget* pCefWidget,
                      QCefWidgetImpl* impl,
                      CefRefPtr<CefBrowser> browser,
                      bool osrMode);  // return top-level widget

  void removeAllCefWidgets(QWidget* pTopWidget);
  void unhookTopWidget(QWidget* pTopWidget);

  int aliveBrowserCount(HWND hTopWidget);
  int aliveBrowserCount(QWidget* pTopWidget);

  void setBrowserClosing(QWidget* pCefWidget);
  void setBrowserClosed(QWidget* pCefWidget);

  void showDevTools(QWidget* pCefWidget);
  void closeDevTools(QWidget* pCefWidget);
  void devToolsClosedNotify(QCefDevToolsWnd* pWnd);

 protected:
  QCefManager();
  ~QCefManager();

  QWidget* getTopWidget(QWidget* pWidget);
  WNDPROC hookWidget(HWND hTopWidget);
  void tryCloseAllBrowsers(HWND hTopWidget);
  void tryCloseAllBrowsers(QWidget* pTopLevelWidget);

 private:
#if (defined Q_OS_WIN32 || defined Q_OS_WIN64)
  static LRESULT CALLBACK newWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
  bool eventFilter(QObject* obj, QEvent* event) override;

  CefRefPtr<QCefBrowserApp> app_;
  CefSettings cefSettings_;
  bool initialized_;

  typedef struct _CefInfo {
    QWidget* cefWidget;

    QWidget* cefWidgetTopWidget;
    HWND cefWidgetTopWidgetHwnd;
    WNDPROC cefWidgetTopWidgetPrevWndProc;
    CefRefPtr<CefBrowser> browser;
    QCefDevToolsWnd* devToolsWnd;
    bool osrMode;
    BrowserStatus browserStatus;
    QCefWidgetImpl* cefWidgetImpl;

    _CefInfo() {
      cefWidget = nullptr;
      cefWidgetTopWidget = nullptr;
      cefWidgetTopWidgetHwnd = nullptr;
      cefWidgetTopWidgetPrevWndProc = nullptr;
      cefWidgetImpl = nullptr;
      browser = nullptr;
      devToolsWnd = nullptr;
      osrMode = false;
      browserStatus = BS_NOT_CREATE;
    }
  } CefInfo;

  std::recursive_mutex cefsMutex_;
  std::list<CefInfo> cefs_;
};
#endif  // !QCEF_MANAGER_H_