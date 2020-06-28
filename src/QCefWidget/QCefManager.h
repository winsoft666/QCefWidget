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
class QCefDevToolsWnd;
class QCefManager : QObject {
  Q_OBJECT
public:
  static QCefManager &getInstance();
  void initializeCef();
  void uninitializeCef();

  QWidget* addBrowser(QWidget* pCefWidget, CefRefPtr<CefBrowser> browser); // return top-level widget
  void removeBrowser(QWidget* pCefWidget, CefRefPtr<CefBrowser> browser);
  int browserCount(QWidget* pTopWidget);
  
  void showDevTools(QWidget* pCefWidget);
  void closeDevTools(QWidget* pCefWidget);
  void devToolsClosedNotify(QCefDevToolsWnd* pWnd);
protected:
  QCefManager();
  ~QCefManager();

  QWidget *getTopWidget(QWidget *pWidget);
  WNDPROC hookWidget(HWND hTopWidget);
  void closeAllBrowsers(HWND hTopWidget);
  void closeAllBrowsers(QWidget* pTopLevelWidget);

private:
#if (defined Q_OS_WIN32 || defined Q_OS_WIN64)
  static LRESULT CALLBACK newWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
  bool eventFilter(QObject *obj, QEvent *event) override;

  CefRefPtr<QCefBrowserApp> app_;
  CefSettings cefSettings_;
  int64_t nCefRefCount_;
  bool initialized_;

  typedef struct _CefInfo {
    QWidget* cefWidget;

    QWidget* cefWidgetTopWidget;
    HWND cefWidgetTopWidgetHwnd;
    WNDPROC cefWidgetTopWidgetPrevWndProc;
    CefRefPtr<CefBrowser> browser;
    QCefDevToolsWnd* devToolsWnd;

    _CefInfo() {
      cefWidget = nullptr;
      cefWidgetTopWidget = nullptr;
      cefWidgetTopWidgetHwnd = nullptr;
      cefWidgetTopWidgetPrevWndProc = nullptr;
      browser = nullptr;
      devToolsWnd = nullptr;
    }
  } CefInfo;

  std::recursive_mutex cefsMutex_;
  std::list<CefInfo> cefs_;

};
#endif // !QCEF_MANAGER_H_