#include <include/cef_sandbox_win.h>
#include "QCefManager.h"
#include "QCefGlobalSetting.h"
#include <QWidget>
#include <QDebug>
#include "QCefDevToolsWnd.h"

QCefManager::QCefManager()
    : initialized_(false)
    , nCefRefCount_(0L) {}

QCefManager::~QCefManager() {}

QCefManager &QCefManager::getInstance() {
  static QCefManager s_instance;
  return s_instance;
}

void QCefManager::initializeCef() {
  if (++nCefRefCount_ > 1)
    return;

  CefEnableHighDPISupport();
  QCefGlobalSetting::initializeInstance();

  CefString(&cefSettings_.browser_subprocess_path) = QCefGlobalSetting::browser_sub_process_path;
  CefString(&cefSettings_.resources_dir_path) = QCefGlobalSetting::resource_directory_path;
  CefString(&cefSettings_.locales_dir_path) = QCefGlobalSetting::locales_directory_path;
  CefString(&cefSettings_.user_agent) = QCefGlobalSetting::user_agent;
  CefString(&cefSettings_.cache_path) = QCefGlobalSetting::cache_path;
  CefString(&cefSettings_.user_data_path) = QCefGlobalSetting::user_data_path;
  CefString(&cefSettings_.locale) = QCefGlobalSetting::locale;
  CefString(&cefSettings_.accept_language_list) = QCefGlobalSetting::accept_language_list;

  cefSettings_.persist_session_cookies = QCefGlobalSetting::persist_session_cookies;
  cefSettings_.persist_user_preferences = QCefGlobalSetting::persist_user_preferences;
  cefSettings_.remote_debugging_port = QCefGlobalSetting::remote_debugging_port;
  cefSettings_.no_sandbox = true;
  cefSettings_.pack_loading_disabled = false;
  cefSettings_.multi_threaded_message_loop = true;
  cefSettings_.windowless_rendering_enabled = true;

#ifndef NDEBUG
  cefSettings_.log_severity = LOGSEVERITY_VERBOSE;
  cefSettings_.remote_debugging_port = 7777;
#else
  cefSettings_.log_severity = LOGSEVERITY_WARNING;
#endif

  app_ = new QCefBrowserApp();

#if (defined Q_OS_WIN32 || defined Q_OS_WIN64)
  HINSTANCE hInstance = ::GetModuleHandle(nullptr);
  CefMainArgs main_args(hInstance);
#else
#error "CefMainArgs no implement"
#endif

  void *sandboxInfo = nullptr;
  if (!CefInitialize(main_args, cefSettings_, app_, sandboxInfo))
    assert(0);
  initialized_ = true;
}

void QCefManager::uninitializeCef() {
  if (!initialized_ || --nCefRefCount_ > 0)
    return;

  CefShutdown();

  app_ = nullptr;

  initialized_ = false;
}

QWidget *QCefManager::addBrowser(QWidget *pCefWidget, CefRefPtr<CefBrowser> browser) {
  std::lock_guard<std::recursive_mutex> lg(cefsMutex_);
  Q_ASSERT(pCefWidget && browser);
  if (!pCefWidget || !browser)
    return nullptr;

  QWidget *pTopWidget = getTopWidget(pCefWidget);
  Q_ASSERT(pTopWidget);

  if (!pTopWidget)
    return nullptr;

  CefInfo cefInfo;
  cefInfo.browser = browser;
  cefInfo.cefWidget = pCefWidget;
  cefInfo.cefWidgetTopWidget = pTopWidget;
  cefInfo.cefWidgetTopWidgetHwnd = (HWND)pTopWidget->window()->winId();

  for (std::list<CefInfo>::iterator it = cefs_.begin(); it != cefs_.end(); it++) {
    if (it->cefWidgetTopWidgetHwnd == cefInfo.cefWidgetTopWidgetHwnd) {
      cefInfo.cefWidgetTopWidgetPrevWndProc = it->cefWidgetTopWidgetPrevWndProc;
      break;
    }
  }
#if 1
  if (!cefInfo.cefWidgetTopWidgetPrevWndProc) {
    cefInfo.cefWidgetTopWidgetPrevWndProc = hookWidget(cefInfo.cefWidgetTopWidgetHwnd);
    cefInfo.cefWidgetTopWidget->installEventFilter(this);
  }
  Q_ASSERT(cefInfo.cefWidgetTopWidgetPrevWndProc);
#endif
  cefs_.push_back(cefInfo);

  return pTopWidget;
}

void QCefManager::removeBrowser(QWidget *pCefWidget, CefRefPtr<CefBrowser> browser) {
  Q_ASSERT(pCefWidget && browser);
  if (!pCefWidget || !browser)
    return;

  std::lock_guard<std::recursive_mutex> lg(cefsMutex_);
  int identifier = browser->GetIdentifier();

  for (std::list<CefInfo>::iterator it = cefs_.begin(); it != cefs_.end(); it++) {
    if (it->cefWidget == pCefWidget && it->browser->GetIdentifier() == identifier) {
      cefs_.erase(it);
      break;
    }
  }
}

void QCefManager::closeAllBrowsers(HWND hTopWidget) {
  std::lock_guard<std::recursive_mutex> lg(cefsMutex_);
  Q_ASSERT(hTopWidget);
  if (!hTopWidget)
    return;

  for (std::list<CefInfo>::iterator it = cefs_.begin(); it != cefs_.end(); it++) {
    if (it->cefWidgetTopWidgetHwnd == hTopWidget && it->browser->GetHost()) {
      it->browser->GetHost()->CloseBrowser(false);
    }
  }
}

void QCefManager::closeAllBrowsers(QWidget *pTopLevelWidget) {
  std::lock_guard<std::recursive_mutex> lg(cefsMutex_);
  Q_ASSERT(pTopLevelWidget);
  if (!pTopLevelWidget)
    return;

  for (std::list<CefInfo>::iterator it = cefs_.begin(); it != cefs_.end(); it++) {
    if (it->cefWidgetTopWidget == pTopLevelWidget && it->browser->GetHost()) {
      it->browser->GetHost()->CloseBrowser(false);
    }
  }
}

int QCefManager::browserCount(QWidget *pTopWidget) {
  Q_ASSERT(pTopWidget);
  if (!pTopWidget)
    return 0;

  std::lock_guard<std::recursive_mutex> lg(cefsMutex_);
  int count = 0;
  for (std::list<CefInfo>::iterator it = cefs_.begin(); it != cefs_.end(); it++) {
    if (it->cefWidgetTopWidget == pTopWidget) {
      count++;
    }
  }

  return count;
}

void QCefManager::showDevTools(QWidget *pCefWidget) {
  std::lock_guard<std::recursive_mutex> lg(cefsMutex_);
  Q_ASSERT(pCefWidget);
  if (!pCefWidget)
    return;

  for (std::list<CefInfo>::iterator it = cefs_.begin(); it != cefs_.end(); it++) {
    if (it->cefWidget == pCefWidget) {
      if (it->devToolsWnd) {
        if (it->devToolsWnd->isMinimized())
          it->devToolsWnd->showNormal();
        else
          it->devToolsWnd->show();
        it->devToolsWnd->activateWindow();
        return;
      }

      it->devToolsWnd = new QCefDevToolsWnd(it->browser, nullptr);
      break;
    }
  }
}

void QCefManager::closeDevTools(QWidget *pCefWidget) {
  std::lock_guard<std::recursive_mutex> lg(cefsMutex_);
  Q_ASSERT(pCefWidget);
  if (!pCefWidget)
    return;

  for (std::list<CefInfo>::iterator it = cefs_.begin(); it != cefs_.end(); it++) {
    if (it->cefWidget == pCefWidget) {
      if (it->devToolsWnd) {
        it->devToolsWnd->close();
        it->devToolsWnd = nullptr;
      }
      break;
    }
  }
}

void QCefManager::devToolsClosedNotify(QCefDevToolsWnd *pWnd) {
  std::lock_guard<std::recursive_mutex> lg(cefsMutex_);
  for (std::list<CefInfo>::iterator it = cefs_.begin(); it != cefs_.end(); it++) {
    if (it->devToolsWnd == pWnd) {
      it->devToolsWnd = nullptr;
      break;
    }
  }
}

QWidget *QCefManager::getTopWidget(QWidget *pWidget) {
  Q_ASSERT(pWidget);
  if (!pWidget)
    return nullptr;

  QWidget *topWidget = pWidget;
  while (topWidget->parent()) {
    topWidget = (QWidget *)topWidget->parent();
  }

  Q_ASSERT(topWidget);
  return topWidget;
}

WNDPROC QCefManager::hookWidget(HWND hTopWidget) {
  Q_ASSERT(hTopWidget);
  if (!hTopWidget)
    return nullptr;
  ::SetWindowLongPtr(hTopWidget, GWLP_USERDATA, reinterpret_cast<LPARAM>(this));
  return (WNDPROC)SetWindowLongPtr(hTopWidget, GWL_WNDPROC, (LONG_PTR)&newWndProc);
}

#if (defined Q_OS_WIN32 || defined Q_OS_WIN64)
LRESULT CALLBACK QCefManager::newWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  QCefManager *pThis = reinterpret_cast<QCefManager *>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
  if (!pThis)
    return 0;

  WNDPROC preWndProc = nullptr;
  do {
    std::lock_guard<std::recursive_mutex> lg(pThis->cefsMutex_);
    for (std::list<CefInfo>::iterator it = pThis->cefs_.begin(); it != pThis->cefs_.end(); it++) {
      if (it->cefWidgetTopWidgetHwnd == hWnd) {
        preWndProc = it->cefWidgetTopWidgetPrevWndProc;
        break;
      }
    }
  } while (false);

  if (!preWndProc)
    return 0;

  if (uMsg == WM_CLOSE) {
    qInfo() << "QCefManager::newWndProc WM_CLOSE, hwnd: " << (int)hWnd;
    pThis->closeAllBrowsers(hWnd);
    ::SetWindowLongPtr(hWnd, GWLP_USERDATA, 0L);
    SetWindowLongPtr(hWnd, GWL_WNDPROC, (LONG_PTR)preWndProc);
    return 0;
  }

  return ::CallWindowProc(preWndProc, hWnd, uMsg, wParam, lParam);
}

bool QCefManager::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::Close) {
    qInfo() << "QCefManager::eventFilter Close event, obj: " << obj;
    std::lock_guard<std::recursive_mutex> lg(cefsMutex_);
    for (std::list<CefInfo>::iterator it = cefs_.begin(); it != cefs_.end(); it++) {
      if (it->cefWidgetTopWidget == obj) {
        this->closeAllBrowsers(it->cefWidgetTopWidget);
        it->cefWidgetTopWidget->removeEventFilter(this);
        event->ignore();
        return true; // filter
      }
    }
  }

  return QObject::eventFilter(obj, event);
  ;
}

#endif
