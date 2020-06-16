#include <include/cef_sandbox_win.h>
#include "CefManager.h"
#include "CefGlobalSetting.h"
#include <QWidget>

CefManager::CefManager()
    : initialized_(false)
    , nCefRefCount_(0L) {}

CefManager::~CefManager() {}

CefManager &CefManager::getInstance() {
  static CefManager s_instance;
  return s_instance;
}

void CefManager::initializeCef() {
  if (++nCefRefCount_ > 1)
    return;

  // Enable High-DPI support on Windows 7 or newer.
  CefEnableHighDPISupport();

  // This is the first time initialization
  CefGlobalSetting::initializeInstance();

  CefString(&cef_settings_.browser_subprocess_path) = CefGlobalSetting::browser_sub_process_path;
  CefString(&cef_settings_.resources_dir_path) = CefGlobalSetting::resource_directory_path;
  CefString(&cef_settings_.locales_dir_path) = CefGlobalSetting::locales_directory_path;
  CefString(&cef_settings_.user_agent) = CefGlobalSetting::user_agent;
  CefString(&cef_settings_.cache_path) = CefGlobalSetting::cache_path;
  CefString(&cef_settings_.user_data_path) = CefGlobalSetting::user_data_path;
  CefString(&cef_settings_.locale) = CefGlobalSetting::locale;
  CefString(&cef_settings_.accept_language_list) = CefGlobalSetting::accept_language_list;

  cef_settings_.persist_session_cookies = CefGlobalSetting::persist_session_cookies;
  cef_settings_.persist_user_preferences = CefGlobalSetting::persist_user_preferences;
  cef_settings_.remote_debugging_port = CefGlobalSetting::remote_debugging_port;
  cef_settings_.no_sandbox = true;
  cef_settings_.pack_loading_disabled = false;
  cef_settings_.multi_threaded_message_loop = true;
  cef_settings_.windowless_rendering_enabled = true;

#ifndef NDEBUG
  cef_settings_.log_severity = LOGSEVERITY_INFO;
  cef_settings_.remote_debugging_port = 7777;
#else
  cef_settings_.log_severity = LOGSEVERITY_WARNING;
#endif

  app_ = new CefBrowserApp();

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
  HINSTANCE hInstance = ::GetModuleHandle(nullptr);
  CefMainArgs main_args(hInstance);
#else
#error "CefMainArgs no implement"
#endif

  void *sandboxInfo = nullptr;
  if (!CefInitialize(main_args, cef_settings_, app_, sandboxInfo))
    assert(0);
  initialized_ = true;
}

void CefManager::uninitializeCef() {
  if (!initialized_ || --nCefRefCount_ > 0)
    return;

  CefShutdown();

  app_ = nullptr;

  initialized_ = false;
}

void CefManager::addBrowser(QWidget *pTopWidget, CefRefPtr<CefBrowser> browser) {
  Q_ASSERT(pTopWidget);
  if (!pTopWidget)
    return;

  std::lock_guard<std::recursive_mutex> lg(browsers_mutex_);
  if (browsers_map_.find(pTopWidget) == browsers_map_.end()) {
    std::list<CefRefPtr<CefBrowser>> list_b;
    list_b.push_back(browser);
    browsers_map_[pTopWidget] = list_b;
  }
  else {
    std::list<CefRefPtr<CefBrowser>> &list_b = browsers_map_[pTopWidget];
    list_b.push_back(browser);
  }
}

void CefManager::removeBrowser(QWidget *pTopWidget, CefRefPtr<CefBrowser> browser) {
  Q_ASSERT(pTopWidget);
  if (!pTopWidget)
    return;

  std::lock_guard<std::recursive_mutex> lg(browsers_mutex_);
  bool bFound = browsers_map_.find(pTopWidget) != browsers_map_.end();
  Q_ASSERT(bFound);
  if (bFound) {
    std::list<CefRefPtr<CefBrowser>> list_b = browsers_map_[pTopWidget];
    for (std::list<CefRefPtr<CefBrowser>>::iterator i = list_b.begin(); i != list_b.end(); i++) {
      if ((*i)->GetIdentifier() == browser->GetIdentifier()) {
        list_b.erase(i);
        break;
      }
    }
    browsers_map_[pTopWidget] = list_b;
  }
}

void CefManager::closeAllBrowsers(QWidget *pTopWidget, bool remove) {
  Q_ASSERT(pTopWidget);
  if (!pTopWidget)
    return;

  std::lock_guard<std::recursive_mutex> lg(browsers_mutex_);
  bool bFound = browsers_map_.find(pTopWidget) != browsers_map_.end();
  Q_ASSERT(bFound);
  if (bFound) {
    std::list<CefRefPtr<CefBrowser>> list_b = browsers_map_[pTopWidget];
    for (std::list<CefRefPtr<CefBrowser>>::iterator i = list_b.begin(); i != list_b.end(); i++) {
      if ((*i)->GetHost()) {
        (*i)->GetHost()->CloseBrowser(false);
      }
    }
  }

  if (remove)
    browsers_map_.erase(pTopWidget);
}

int CefManager::browserCount(QWidget *pTopWidget) {
  Q_ASSERT(pTopWidget);
  if (!pTopWidget)
    return 0;

  std::lock_guard<std::recursive_mutex> lg(browsers_mutex_);

  bool bFound = browsers_map_.find(pTopWidget) != browsers_map_.end();
  if (!bFound) {
    return 0;
  }
  return browsers_map_[pTopWidget].size();
}

QWidget *CefManager::getTopWidget(QWidget *pWidget) {
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

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
LRESULT CALLBACK CefManager::newWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  CefManager *pThis = reinterpret_cast<CefManager *>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
  if (!pThis)
    return 0;

  HookInfo hi;
  do {
    std::lock_guard<std::recursive_mutex> lg(pThis->hooks_mutex_);
    if (pThis->hooks_.find(hWnd) == pThis->hooks_.end())
      return 0;
    hi = pThis->hooks_[hWnd];
  } while (false);

  if (!hi.cefWidget || !hi.topWidget || !hi.prevWndProc)
    return 0;

  if (uMsg == WM_CLOSE) {

    pThis->closeAllBrowsers(hi.topWidget, false);

    ::SetWindowLongPtr(hWnd, GWLP_USERDATA, 0L);
    SetWindowLongPtr(hWnd, GWL_WNDPROC, (LONG_PTR)hi.prevWndProc);
    return 0;
  }

  return ::CallWindowProc(hi.prevWndProc, hWnd, uMsg, wParam, lParam);
}
#endif

QWidget *CefManager::hookTopLevelWidget(QWidget *pWidget) {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
  QWidget *topWidget = getTopWidget(pWidget);
  if (!topWidget)
    return nullptr;

  std::lock_guard<std::recursive_mutex> lg(hooks_mutex_);

  for (auto &c : hooks_) {
    if (c.second.topWidget == topWidget)
      return topWidget;
  }

  HookInfo hookInfo;
  hookInfo.cefWidget = pWidget;
  hookInfo.topWidget = topWidget;

  HWND hTopWnd = (HWND)topWidget->window()->winId();

  ::SetWindowLongPtr(hTopWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(this));
  hookInfo.prevWndProc = (WNDPROC)SetWindowLongPtr(hTopWnd, GWL_WNDPROC, (LONG_PTR)&newWndProc);

  hooks_[hTopWnd] = hookInfo;

  return topWidget;
#else
#error "No hook implement"
#endif
}
