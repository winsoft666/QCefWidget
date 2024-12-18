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

namespace client {
PopupWidget::PopupWidget(RootWindowQt* w, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    root_win_(w) {
  DCHECK(root_win_);
  this->installEventFilter(this);
}

bool PopupWidget::eventFilter(QObject* obj, QEvent* e) {
  return root_win_->eventFilter(obj, e);
}

RootWindowQt::RootWindowQt() {
  qDebug() << ">>>> RootWindowQt Ctor";
}

RootWindowQt::~RootWindowQt() {
  REQUIRE_MAIN_THREAD();
  qDebug() << ">>>> RootWindowQt Dtor";

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
  qDebug() << ">>>> RootWindowQt::Close, force:" << force;

  if (widget_) {
    // force参数的意思是直接销毁窗口，不在WM_CLOSE或closeEvent中执行浏览器关闭操作
    // 在调用close()函数之后，直接标记为窗口已经销毁 window_destroyed_=true
    force_close_root_win_ = force;
    qDebug() << ">>>>     Call QWidget::close()" << widget_;
    widget_->close();

    //if (!force) {
    //  LOG(INFO) << ">>>> QWebView::close()" << force;
    //  widget_->close();
    //}
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
    qDebug() << ">>>> Recv QWebView QEvent::Close";

    if (OnClose()) {
      qDebug() << ">>>> QWebView Cancel close";
      return true;  // Cancel close
    }
    qDebug() << ">>>> QWebView allow close";
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
    // 临时窗口未指定QWidget，需要自动创建
    widget_ = new PopupWidget(this);
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
        qDebug() << ">>>> Call GetHost()->CloseBrowser(false)";

        QWebViewManager::Get()->setWebViewClosed(nullptr, (QWebView*)widget_.data());

        // Notify the browser window that we would like to close it. This
        // will result in a call to ClientHandler::DoClose() if the
        // JavaScript 'onbeforeunload' event handler allows it.
        browser->GetHost()->CloseBrowser(false);

        // 在非OSR模式下，调用browser->GetHost()->CloseBrowser(false)后，CEF会向顶级窗口发送WM_CLOSE消息
        // 而在非OSR模式下，则不会发送WM_CLOSE消息，因此在此处模拟发送该消息，保持两种模式下的退出流程一致
        if (with_osr_) {
          QWebViewManager::Get()->sendCloseEventToTopLevel((QWebView*)widget_.data());
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
  qDebug() << ">>>> RootWindowQt::OnBrowserWindowDestroyed";

  browser_window_.reset();

  // winsoft666: 校准window_destroyed_参数
  // 在主动关闭窗口或者调用QWidget::close时，由于WM_CLOSE消息可能会被CEF拦截，导致eventFilter无法捕获closeEvent事件，
  // 因此在此处通过widget_是否为空来判断QWidget是否已经销毁
  //if (!window_destroyed_) {
  //  if (!widget_)
  //    window_destroyed_ = true;
  //}

  if (!window_destroyed_) {
    // The browser was destroyed first. This could be due to the use of
    // off-screen rendering or execution of JavaScript window.close().
    // Close the RootWindow.
    Close(true);
    //window_destroyed_ = true;
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
}

void RootWindowQt::NotifyDestroyedIfDone() {
  //if (window_destroyed_ && root_win_qt_delegate_) {
  //  qDebug() << ">>>> Notify Window destoryed";
  //  root_win_qt_delegate_->OnWindowDestoryed();
  //}

  // Notify once both the window and the browser have been destroyed.
  if (window_destroyed_ && browser_destroyed_) {
    qDebug() << ">>>> Notify Browser/Window all destoryed";
    root_win_qt_delegate_->OnWindowAndBrowserDestoryed();

    qDebug() << ">>>> OnRootWindowDestroyed";
    delegate_->OnRootWindowDestroyed(this);  // 调用该语句会使当前RootWindowQt对象被销毁
  }
}

}  // namespace client
