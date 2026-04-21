#include "root_window_qt.h"
#include "include/base/cef_bind.h"
#include "include/base/cef_build.h"
#include "include/cef_app.h"
#include "tests/cefclient/browser/browser_window_osr_win.h"
#include "tests/cefclient/browser/browser_window_std_win.h"
#include "tests/cefclient/browser/main_context.h"
#include "tests/cefclient/browser/resource.h"
#include "tests/cefclient/browser/temp_window.h"
#include "tests/cefclient/browser/window_test_runner_win.h"
#include "tests/shared/browser/geometry_util.h"
#include "tests/shared/browser/main_message_loop.h"
#include "tests/shared/browser/util_win.h"
#include "tests/shared/common/client_switches.h"
#include <QEvent>
#include <QWindow>
#include <QScreen>
#include <QTimer>
#include <QDebug>
#include "QWebView/Manager.h"
#include "QWebView/ManagerPrivate.h"
#ifdef Q_OS_WIN
#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#endif  // !GET_X_LPARAM

#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif  // !GET_Y_LPARAM
#endif  // Q_OS_WIN

#ifdef Q_OS_WIN
namespace win_draggable_helper {

LPCWSTR kParentWndProc = L"CefParentWndProc";
LPCWSTR kDraggableRegion = L"CefDraggableRegion";

HWND GetTopLevelWindow(HWND h) {
  if (!h)
    return nullptr;

  HWND result = h;
  do {
    HWND p = GetParent(result);
    if (!p)
      break;
    result = p;
  } while (true);

  return result;
}

LRESULT CALLBACK SubclassedWindowProc(HWND hWnd,
                                      UINT message,
                                      WPARAM wParam,
                                      LPARAM lParam) {
  WNDPROC hParentWndProc =
      reinterpret_cast<WNDPROC>(::GetPropW(hWnd, kParentWndProc));
  HRGN hRegion = reinterpret_cast<HRGN>(::GetPropW(hWnd, kDraggableRegion));

  if (message == WM_LBUTTONDOWN) {
    if (hRegion) {
      POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
      if (::PtInRegion(hRegion, point.x, point.y)) {
        ::ClientToScreen(hWnd, &point);
        HWND topLevel = GetTopLevelWindow(hWnd);
        if (topLevel) {
          ::PostMessage(topLevel,
                        WM_NCLBUTTONDOWN,
                        HTCAPTION,
                        MAKELPARAM(point.x, point.y));
        }
        return 0;
      }
    }
  }

  return CallWindowProc(hParentWndProc, hWnd, message, wParam, lParam);
}

void SubclassWindow(HWND hWnd, HRGN hRegion) {
  HANDLE hParentWndProc = ::GetPropW(hWnd, kParentWndProc);
  if (hParentWndProc) {
    return;
  }

  SetLastError(0);
  LONG_PTR hOldWndProc = SetWindowLongPtr(
      hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(SubclassedWindowProc));
  if (hOldWndProc == 0 && GetLastError() != ERROR_SUCCESS) {
    return;
  }

  ::SetPropW(hWnd, kParentWndProc, reinterpret_cast<HANDLE>(hOldWndProc));
  ::SetPropW(hWnd, kDraggableRegion, reinterpret_cast<HANDLE>(hRegion));
}

void UnSubclassWindow(HWND hWnd) {
  LONG_PTR hParentWndProc =
      reinterpret_cast<LONG_PTR>(::GetPropW(hWnd, kParentWndProc));
  if (hParentWndProc) {
    LONG_PTR hPreviousWndProc =
        SetWindowLongPtr(hWnd, GWLP_WNDPROC, hParentWndProc);
    ALLOW_UNUSED_LOCAL(hPreviousWndProc);
    DCHECK_EQ(hPreviousWndProc,
              reinterpret_cast<LONG_PTR>(SubclassedWindowProc));
  }

  ::RemovePropW(hWnd, kParentWndProc);
  ::RemovePropW(hWnd, kDraggableRegion);
}

BOOL CALLBACK SubclassWindowsProc(HWND hwnd, LPARAM lParam) {
  SubclassWindow(hwnd, reinterpret_cast<HRGN>(lParam));
  return TRUE;
}

BOOL CALLBACK UnSubclassWindowsProc(HWND hwnd, LPARAM lParam) {
  UnSubclassWindow(hwnd);
  return TRUE;
}

}  // namespace win_draggable_helper
#endif  // Q_OS_WIN

namespace client {
DevToolsPopupWidget::DevToolsPopupWidget(RootWindowQt* w, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    root_win_(w) {
  DCHECK(root_win_);
  this->installEventFilter(this);
}

void DevToolsPopupWidget::OnWindowAndBrowserDestoryed() {
  root_win_ = nullptr;
}

bool DevToolsPopupWidget::eventFilter(QObject* obj, QEvent* e) {
  if (root_win_)
    return root_win_->eventFilter(obj, e);
  return QWidget::eventFilter(obj, e);
}

RootWindowQt::RootWindowQt() {
#ifdef QT_DEBUG
  qDebug() << ">>>> RootWindowQt Ctor";
#endif

#ifdef Q_OS_WIN
  // Create a HRGN representing the draggable window area.
  draggable_region_ = ::CreateRectRgn(0, 0, 0, 0);
#endif  // Q_OS_WIN
}

RootWindowQt::~RootWindowQt() {
  REQUIRE_MAIN_THREAD();
#ifdef QT_DEBUG
  qDebug() << ">>>> RootWindowQt Dtor";
#endif

#ifdef Q_OS_WIN
  ::DeleteObject(draggable_region_);
#endif  // Q_OS_WIN

  // The window and browser should already have been destroyed.
  DCHECK(window_destroyed_);
  DCHECK(browser_destroyed_);
}

void RootWindowQt::Init(RootWindow::Delegate* delegate,
                        const RootWindowConfig& config,
                        const CefBrowserSettings& settings) {
  DCHECK(delegate);
  DCHECK(!initialized_);
  DCHECK(config.widget);

  if (!config.widget)
    return;

  delegate_ = delegate;
  with_osr_ = config.with_osr;
  with_extension_ = config.with_extension;
  start_rect_ = QRect(config.bounds.x, config.bounds.y, config.bounds.width, config.bounds.height);

  CreateBrowserWindow(config.url);

  initialized_ = true;

  // Create the native root window on the main thread.
  if (CURRENTLY_ON_MAIN_THREAD()) {
    CreateRootWindow(settings, config.initially_hidden, (QWidget*)config.widget);
  }
  else {
    MAIN_POST_CLOSURE(base::Bind(&RootWindowQt::CreateRootWindow, this,
                                 settings, config.initially_hidden, (QWidget*)config.widget));
  }
}

void RootWindowQt::InitAsPopup(RootWindow::Delegate* delegate,
                               bool with_controls,
                               bool with_osr,
                               const CefPopupFeatures& popupFeatures,
                               CefWindowInfo& windowInfo,
                               CefRefPtr<CefClient>& client,
                               CefBrowserSettings& settings) {
  CEF_REQUIRE_UI_THREAD();

  DCHECK(delegate);
  DCHECK(!initialized_);

  delegate_ = delegate;
  with_osr_ = with_osr;
  is_popup_ = true;

  if (popupFeatures.xSet)
    start_rect_.setLeft(popupFeatures.x);
  if (popupFeatures.ySet)
    start_rect_.setTop(popupFeatures.y);
  if (popupFeatures.widthSet)
    start_rect_.setRight(start_rect_.left() + popupFeatures.width);
  if (popupFeatures.heightSet)
    start_rect_.setBottom(start_rect_.top() + popupFeatures.height);

  CreateBrowserWindow(std::string());

  initialized_ = true;

  // The new popup is initially parented to a temporary window. The native root
  // window will be created after the browser is created and the popup window
  // will be re-parented to it at that time.
  browser_window_->GetPopupConfig(TempWindow::GetWindowHandle(), windowInfo,
                                  client, settings);
}

void RootWindowQt::Show(ShowMode mode) {
  REQUIRE_MAIN_THREAD();

  if (widget_) {
    widget_->show();
  }
}

void RootWindowQt::Hide() {
  REQUIRE_MAIN_THREAD();

  if (widget_) {
    widget_->hide();
  }
}

void RootWindowQt::SetBounds(int x, int y, size_t width, size_t height) {
  REQUIRE_MAIN_THREAD();

  if (widget_) {
    widget_->setGeometry(x, y, width, height);
  }
}

void RootWindowQt::Close(bool force) {
  REQUIRE_MAIN_THREAD();
#ifdef QT_DEBUG
  qDebug() << ">>>> RootWindowQt::Close, force:" << force;
#endif

  if (widget_) {
    // The meaning of the force parameter is to directly destroy the window without performing browser closing operations in WM_CLOSE or closeEvent.
    // After calling the QWidget::close() function, directly mark the window as already destroyed, i.e., window_destroyed_ = true.
    force_close_root_win_ = force;
#ifdef QT_DEBUG
    qDebug() << ">>>>     Call QWidget::close()" << widget_;
#endif
    widget_->close();
  }

  window_destroyed_ = true;
  NotifyDestroyedIfDone();
}

void RootWindowQt::SetDeviceScaleFactor(float device_scale_factor) {
  REQUIRE_MAIN_THREAD();

  if (browser_window_ && with_osr_)
    browser_window_->SetDeviceScaleFactor(device_scale_factor);
}

float RootWindowQt::GetDeviceScaleFactor() const {
  REQUIRE_MAIN_THREAD();

  if (browser_window_ && with_osr_)
    return browser_window_->GetDeviceScaleFactor();

  NOTREACHED();
  return 0.0f;
}

CefRefPtr<CefBrowser> RootWindowQt::GetBrowser() const {
  REQUIRE_MAIN_THREAD();

  if (browser_window_)
    return browser_window_->GetBrowser();
  return NULL;
}

ClientWindowHandle RootWindowQt::GetWindowHandle() const {
  REQUIRE_MAIN_THREAD();
  DCHECK(widget_);
  return (ClientWindowHandle)widget_->winId();
}

bool RootWindowQt::WithWindowlessRendering() const {
  REQUIRE_MAIN_THREAD();
  return with_osr_;
}

bool RootWindowQt::WithExtension() const {
  REQUIRE_MAIN_THREAD();
  return with_extension_;
}

void RootWindowQt::SetOneTimeUrl(const std::string& url) {
  one_time_url_ = url;
}

void RootWindowQt::ShowDevTools() {
  REQUIRE_MAIN_THREAD();
  if (browser_window_) {
    CefRefPtr<ClientHandler> clientHandler = browser_window_->clientHandler();
    if (clientHandler) {
      clientHandler->ShowDevTools(browser_window_->GetBrowser(), CefPoint());
    }
  }
}

void RootWindowQt::CloseDevTools() {
  REQUIRE_MAIN_THREAD();
  if (browser_window_) {
    CefRefPtr<ClientHandler> clientHandler = browser_window_->clientHandler();
    if (clientHandler) {
      clientHandler->CloseDevTools(browser_window_->GetBrowser());
    }
  }
}

void RootWindowQt::OnReceiveJsNotify(const std::string& message) {
  if (root_win_qt_delegate_) {
    root_win_qt_delegate_->OnJsNotify(message);
  }
}

void RootWindowQt::OnPopupWindow(const std::string& url) {
  if (widget_) {
    QWebView* webview = (QWebView*)widget_.data();
    emit webview->newPopupWindow(QString::fromStdString(url));
  }
}

bool RootWindowQt::isAllowBrowserClose() {
  return true;
}

bool RootWindowQt::eventFilter(QObject* obj, QEvent* e) {
  QEvent::Type event_type = e->type();

  if (event_type == QEvent::Resize) {
    OnSize(false);
  }
  else if (event_type == QEvent::WindowStateChange) {
    bool minimized = widget_->windowState() == Qt::WindowMinimized;
    OnSize(minimized);
  }
  else if (event_type == QEvent::FocusIn) {
    OnFocus();
  }
  else if (event_type == QEvent::Move) {
    OnMove();
  }
  else if (event_type == QEvent::Close) {
#ifdef QT_DEBUG
    qDebug() << ">>>> Recv QWebView QEvent::Close";
#endif

    if (OnClose()) {
#ifdef QT_DEBUG
      qDebug() << ">>>> QWebView Cancel close";
#endif
      return true;  // Cancel close
    }
#ifdef QT_DEBUG
    qDebug() << ">>>> QWebView allow close";
#endif
  }

  // standard event processing
  return QObject::eventFilter(obj, e);
}

void RootWindowQt::onScreenLogicalDotsPerInchChanged(qreal dpi) {
  OnDpiChanged();
}

void RootWindowQt::CreateBrowserWindow(const std::string& startup_url) {
  if (with_osr_) {
    OsrRendererSettings settings = {};
    MainContext::Get()->PopulateOsrSettings(&settings);
    browser_window_.reset(new BrowserWindowOsrWin(this, startup_url, settings));
  }
  else {
    browser_window_.reset(new BrowserWindowStdWin(this, startup_url));
  }
}

void RootWindowQt::CreateRootWindow(const CefBrowserSettings& settings,
                                    bool initially_hidden,
                                    QWidget* w) {
  REQUIRE_MAIN_THREAD();

  CefRefPtr<CefCommandLine> command_line =
      CefCommandLine::GetGlobalCommandLine();
  const bool no_activate = command_line->HasSwitch(switches::kNoActivate);

  browser_settings_ = settings;

  if (w) {
    widget_ = w;
    widget_->installEventFilter(this);
  }
  else {
    // The temporary window does not have a specified QWidget and needs to be created automatically.
    // Only the DevTools popup window will reach this point, and other popup window have already been cancelled.
    widget_ = new DevToolsPopupWidget(this);
    widget_->installEventFilter(this);
  }

  QWidget* window = widget_->window();
  if (window) {
    connect(window->windowHandle(), &QWindow::screenChanged, this, [this](QScreen* screen) {
      if (screen)
        connect(screen, &QScreen::logicalDotsPerInchChanged, this, &RootWindowQt::onScreenLogicalDotsPerInchChanged);
    });
    connect(window->screen(), &QScreen::logicalDotsPerInchChanged, this, &RootWindowQt::onScreenLogicalDotsPerInchChanged);
  }
  OnCreate();

  if (!initially_hidden) {
    // Show this window.
    Show(no_activate ? ShowNoActivate : ShowNormal);
  }
}

float RootWindowQt::GetWidgetScaleFactor() const {
  REQUIRE_MAIN_THREAD();
  DCHECK(widget_);

  if (widget_) {
    return widget_->devicePixelRatioF();
  }
  return 1;
}

QRect RootWindowQt::GetWidgetScaledRealRect() const {
  DCHECK(widget_);
  QRect rect;
  if (widget_) {
    float s = GetWidgetScaleFactor();
    rect = widget_->rect();
    rect.setWidth(rect.width() * s);
    rect.setHeight(rect.height() * s);
  }
  return rect;
}

void RootWindowQt::OnFocus() {
  // Selecting "Close window" from the task bar menu may send a focus
  // notification even though the window is currently disabled (e.g. while a
  // modal JS dialog is displayed).
  if (browser_window_ && widget_->isEnabled())
    browser_window_->SetFocus(true);
}

void RootWindowQt::OnActivate(bool active) {
  if (active)
    delegate_->OnRootWindowActivated(this);
}

void RootWindowQt::OnSize(bool minimized) {
  if (minimized) {
    // Notify the browser window that it was hidden and do nothing further.
    if (browser_window_)
      browser_window_->Hide();
    return;
  }

  if (browser_window_) {
    browser_window_->Show();

    QRect rect = GetWidgetScaledRealRect();
    // Size the browser window to the whole client area.
    browser_window_->SetBounds(0, 0, rect.right(), rect.bottom());
  }
}

void RootWindowQt::OnMove() {
  // Notify the browser of move events so that popup windows are displayed
  // in the correct location and dismissed when the window moves.
  CefRefPtr<CefBrowser> browser = GetBrowser();
  if (browser)
    browser->GetHost()->NotifyMoveOrResizeStarted();
}

void RootWindowQt::OnDpiChanged() {
  if (browser_window_ && with_osr_) {
    // Scale factor for the new display.
    const float display_scale_factor = GetDeviceScaleFactor();
    browser_window_->SetDeviceScaleFactor(display_scale_factor);
  }
}

void RootWindowQt::OnCreate() {
  const float device_scale_factor = GetWidgetScaleFactor();

  if (with_osr_) {
    browser_window_->SetDeviceScaleFactor(device_scale_factor);
  }

  QRect rect;

  if (start_rect_.isEmpty()) {
    rect = GetWidgetScaledRealRect();
  }
  else {
    start_rect_.setWidth(start_rect_.width() * device_scale_factor);
    start_rect_.setHeight(start_rect_.height() * device_scale_factor);
    rect = start_rect_;
  }

  if (!is_popup_) {
    // Create the browser window.
    CefRect cef_rect(rect.left(), rect.top(), rect.width(), rect.height());
    browser_window_->CreateBrowser((ClientWindowHandle)widget_->winId(), cef_rect, browser_settings_,
                                   delegate_->GetRequestContext(this));
  }
  else {
    // With popups we already have a browser window. Parent the browser window
    // to the root window and show it in the correct location.
    browser_window_->ShowPopup((ClientWindowHandle)widget_->winId(), rect.left(), rect.top(), rect.width(), rect.height());
  }
}

bool RootWindowQt::OnClose() {
  if (!force_close_root_win_) {
    if (browser_window_ && !browser_window_->IsClosing()) {
      CefRefPtr<CefBrowser> browser = GetBrowser();
      if (browser) {
#ifdef QT_DEBUG
        qDebug() << ">>>> Call GetHost()->CloseBrowser(false)";
#endif

        QWebViewManager::Get()->privatePointer()->setWebViewClosed(nullptr, (QWebView*)widget_.data());

        // Notify the browser window that we would like to close it. This
        // will result in a call to ClientHandler::DoClose() if the
        // JavaScript 'onbeforeunload' event handler allows it.
        browser->GetHost()->CloseBrowser(false);

        // In non-OSR mode, after calling browser->GetHost()->CloseBrowser(false),
        // CEF will send a closing message to the top-level window, such as the WM_CLOSE message.
        //
        // However, in non-OSR mode, the WM_CLOSE message will not be sent.
        // Therefore, this message is simulated here to maintain consistent exit procedures in both modes.
        if (with_osr_) {
          QWebViewManager::Get()->privatePointer()->sendCloseEventToTopLevel((QWebView*)widget_.data());
        }

        // Cancel the close.
        return true;
      }
    }
  }
  force_close_root_win_ = false;

  window_destroyed_ = true;
  NotifyDestroyedIfDone();

  // Allow the close.
  return false;
}

void RootWindowQt::OnBrowserCreated(CefRefPtr<CefBrowser> browser) {
  REQUIRE_MAIN_THREAD();

  if (is_popup_) {
    // For popup browsers create the root window once the browser has been
    // created.
    CreateRootWindow(CefBrowserSettings(), false, nullptr);
  }
  else {
    // Make sure the browser is sized correctly.
    OnSize(false);
  }

  delegate_->OnBrowserCreated(this, browser);

  if (root_win_qt_delegate_)
    root_win_qt_delegate_->OnBrowserCreated();

  if (!one_time_url_.empty()) {
    browser->GetMainFrame()->LoadURL(one_time_url_);
    one_time_url_.clear();
  }
}

void RootWindowQt::OnBrowserWindowDestroyed() {
  REQUIRE_MAIN_THREAD();
#ifdef QT_DEBUG
  qDebug() << ">>>> RootWindowQt::OnBrowserWindowDestroyed";
#endif

  browser_window_.reset();

  if (!window_destroyed_) {
    // The browser was destroyed first. This could be due to the use of
    // off-screen rendering or execution of JavaScript window.close().
    // Close the RootWindow.
    Close(true);
  }

  browser_destroyed_ = true;
  NotifyDestroyedIfDone();
}

void RootWindowQt::OnSetAddress(const std::string& url) {
  REQUIRE_MAIN_THREAD();
  if (root_win_qt_delegate_) {
    root_win_qt_delegate_->OnSetAddress(url);
  }
}

void RootWindowQt::OnSetTitle(const std::string& title) {
  REQUIRE_MAIN_THREAD();
  if (root_win_qt_delegate_) {
    root_win_qt_delegate_->OnSetTitle(title);
  }
}

void RootWindowQt::OnSetFullscreen(bool fullscreen) {
  REQUIRE_MAIN_THREAD();
  if (root_win_qt_delegate_) {
    root_win_qt_delegate_->OnSetFullscreen(fullscreen);
  }
}

void RootWindowQt::OnAutoResize(const CefSize& new_size) {
  REQUIRE_MAIN_THREAD();

  if (!widget_)
    return;
}

void RootWindowQt::OnSetLoadingState(bool isLoading,
                                     bool canGoBack,
                                     bool canGoForward) {
  REQUIRE_MAIN_THREAD();
  if (root_win_qt_delegate_) {
    root_win_qt_delegate_->OnSetLoadingState(isLoading, canGoBack, canGoForward);
  }
}

void RootWindowQt::OnSetDraggableRegions(
    const std::vector<CefDraggableRegion>& regions) {
  REQUIRE_MAIN_THREAD();

#ifdef Q_OS_WIN
  float dpiScale = widget_->devicePixelRatioF();

  // Reset draggable region.
  ::SetRectRgn(draggable_region_, 0, 0, 0, 0);

  // Determine new draggable region.
  std::vector<CefDraggableRegion>::const_iterator it = regions.begin();
  for (; it != regions.end(); ++it) {
    cef_rect_t rc = it->bounds;
    rc.x = (float)rc.x * dpiScale;
    rc.y = (float)rc.y * dpiScale;
    rc.width = (float)rc.width * dpiScale;
    rc.height = (float)rc.height * dpiScale;
    HRGN region =
        ::CreateRectRgn(rc.x, rc.y, rc.x + rc.width, rc.y + rc.height);
    ::CombineRgn(draggable_region_,
                 draggable_region_,
                 region,
                 it->draggable ? RGN_OR : RGN_DIFF);
    ::DeleteObject(region);
  }

  // Subclass child window procedures in order to do hit-testing.
  // This will be a no-op, if it is already subclassed.
  WNDENUMPROC proc =
      !regions.empty() ? win_draggable_helper::SubclassWindowsProc : win_draggable_helper::UnSubclassWindowsProc;
  ::EnumChildWindows((HWND)widget_->winId(), proc,
                     reinterpret_cast<LPARAM>(draggable_region_));
#endif
}

void RootWindowQt::NotifyDestroyedIfDone() {
  // Notify once both the window and the browser have been destroyed.
  if (window_destroyed_ && browser_destroyed_) {
    if (is_popup_) {
      DevToolsPopupWidget* popupWidget = (DevToolsPopupWidget*)widget_.data();
      if (popupWidget)
        popupWidget->OnWindowAndBrowserDestoryed();
    }

    if (root_win_qt_delegate_) {
#ifdef QT_DEBUG
      qDebug() << ">>>> Notify Browser/Window all destoryed";
#endif
      root_win_qt_delegate_->OnWindowAndBrowserDestoryed();
    }

#ifdef QT_DEBUG
    qDebug() << ">>>> OnRootWindowDestroyed";
#endif

    // Note: Calling this statement will cause the current RootWindowQt object to be destroyed.
    delegate_->OnRootWindowDestroyed(this);
  }
}

}  // namespace client
