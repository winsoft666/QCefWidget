#ifndef QCEF_MANAGER_H_
#define QCEF_MANAGER_H_
#pragma once

#include "CefBrowserApp/CefBrowserApp.h"
#include <mutex>
#include <list>
#include <map>
#include <qsystemdetection.h>

class QWidget;
class CefManager {
public:
  static CefManager &getInstance();
  void initializeCef();
  void uninitializeCef();

  void addBrowser(QWidget* pTopWidget, CefRefPtr<CefBrowser> browser);
  void removeBrowser(QWidget* pTopWidget, CefRefPtr<CefBrowser> browser);
  void closeAllBrowsers(QWidget* pTopWidget, bool remove);

  int browserCount(QWidget* pTopWidget);

  QWidget* getTopWidget(QWidget* pWidget);
  QWidget* hookTopLevelWidget(QWidget* pWidget); // return hooked top-level widget
protected:
  CefManager();
  ~CefManager();

private:
#if (defined Q_OS_WIN32 || defined Q_OS_WIN64)
  static LRESULT CALLBACK newWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
  CefRefPtr<CefBrowserApp> app_;
  CefSettings cef_settings_;
  int64_t nCefRefCount_;
  bool initialized_;


#if (defined Q_OS_WIN32 || defined Q_OS_WIN64)
  typedef struct _HookInfo {
    QWidget *cefWidget;
    QWidget *topWidget;
    WNDPROC prevWndProc;
    _HookInfo() {
      cefWidget = nullptr;
      topWidget = nullptr;
      prevWndProc = nullptr;
    }
  } HookInfo;

  std::recursive_mutex hooks_mutex_;
  std::map<HWND, HookInfo> hooks_;
#endif

  std::recursive_mutex browsers_mutex_;
  std::map<QWidget*, std::list<CefRefPtr<CefBrowser>>> browsers_map_; // all browsers of this top-widget
};
#endif // !QCEF_MANAGER_H_