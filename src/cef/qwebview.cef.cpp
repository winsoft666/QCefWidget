#include "qwebview.cef.h"
#include <QAbstractEventDispatcher>
#include <QDebug>
#include <QResizeEvent>
#include <QVariant>
#include <QPainter>
#include <include/base/cef_logging.h>
#include <include/cef_app.h>
#include <include/cef_browser.h>
#include <include/cef_frame.h>
#include <include/cef_sandbox_win.h>
#include "include/base/cef_scoped_ptr.h"
#include "include/cef_command_line.h"
#include "tests/cefclient/browser/main_context_impl.h"
#include "tests/cefclient/browser/main_message_loop_multithreaded_win.h"
#include "tests/cefclient/browser/root_window_manager.h"
#include "tests/cefclient/browser/test_runner.h"
#include "tests/cefclient/browser/client_handler.h"
#include "tests/shared/browser/client_app_browser.h"
#include "tests/shared/browser/main_message_loop_external_pump.h"
#include "tests/shared/browser/main_message_loop_std.h"
#include "tests/shared/common/client_app_other.h"
#include "tests/shared/common/client_switches.h"
#include "tests/shared/renderer/client_app_renderer.h"
#include "root_window_qt.h"
#include <QApplication>
#include "QWebView/Manager.h"
#include "QWebView/Creator.h"

using namespace client;

namespace {
CefString QStr2CefStr(const QString& s) {
  CefString cefStr;
  cefStr.FromWString(s.toStdWString());
  return cefStr;
}
}  // namespace

class QWebViewCEF::Impl : public QObject, public RootWindowQt::Delegate {
 public:
  Impl(QWebViewCEF* parent) :
      parent_(parent) {
    Q_ASSERT(parent_);
  }

  CefRefPtr<CefBrowser> GetBrowser() {
    if (rootWindow_)
      return rootWindow_->GetBrowser();
    return nullptr;
  }

  RootWindowQt* GetRootWindowQt() {
    if (rootWindow_)
      return reinterpret_cast<RootWindowQt*>(rootWindow_);
    return nullptr;
  }

  void setOffscreenRender(bool osr) {
    osr_ = osr;
  }

  bool isOffscreenRender() const {
    return osr_;
  }

  void TryCreateRootWindow() {
    if (rootWindow_)
      return;

    // 延后安装事件过滤器，防止QWebView控件还未加入到布局中，查找不到其父窗口
    //TopLevelEventFilter::Get()->setFilterWebViewEvent(parent_);

    MainContext* context = MainContext::Get();
    DCHECK(context);
    if (context) {
      RootWindowConfig window_config;
      window_config.with_controls = false;
      window_config.with_osr = osr_;
      window_config.widget = (void*)parent_;
      window_config.url.clear();

      rootWindow_ = context->GetRootWindowManager()->CreateRootWindow(window_config);

      RootWindowQt* q = GetRootWindowQt();
      DCHECK(q);
      if (q)
        q->SetDelegate(this);
    }
  }

 private:
  // RootWindowQt::Delegate
  void OnJsNotify(const std::string& message) override {
    emit parent_->messageReceived(QString::fromStdString(message));
  }

  void OnSetAddress(const std::string& url) override {
    emit parent_->urlChanged(QString::fromStdString(url));
  }

  void OnSetTitle(const std::string& title) override {
    emit parent_->titleChanged(QString::fromStdString(title));
  }

  void OnSetLoadingState(bool isLoading, bool canGoBack, bool canGoForward) override {
    emit parent_->loadStateChanged(isLoading);
  }

  void OnSetFullscreen(bool fullscreen) override {
    emit parent_->fullscreenChanged(fullscreen);
  }

  void OnBrowserCreated() override {
    QWebViewManager::Get()->add(parent_);
  }

  void OnWindowDestoryed() override {
    //rootWindow_ = nullptr;
  }

  void OnWindowAndBrowserDestoryed() override {
    rootWindow_ = nullptr;
    QWebViewManager::Get()->remove(parent_);
  }

 private:
  bool osr_ = false;
  QWebViewCEF* parent_ = nullptr;
  RootWindow* rootWindow_ = nullptr;  // 使用裸指针，不使用 scoped_refptr<RootWindow>，避免引用计数导致无法在RootWindowManager中释放
};

QWebViewCEF::QWebViewCEF(QWidget* parent) :
    QWebView(parent),
    impl_(new Impl(this)) {
  qDebug() << ">>>> QWebViewCEF Ctor";
  DCHECK(!qApp->quitOnLastWindowClosed());

  engine_ = BrowserEngine::CEF;
}

QWebViewCEF::~QWebViewCEF() {
  qDebug() << ">>>> QWebViewCEF Dtor";
}

QWebView::BrowserEngine QWebViewCEF::browserEngine() const {
  return engine_;
}

void QWebViewCEF::navigate(const QString& url) {
  REQUIRE_MAIN_THREAD();

  impl_->TryCreateRootWindow();

  CefRefPtr<CefBrowser> browser = impl_->GetBrowser();
  if (browser) {
    browser->GetMainFrame()->LoadURL(QStr2CefStr(url));
  }
  else {
    RootWindowQt* r = impl_->GetRootWindowQt();
    if (r)
      r->SetOneTimeUrl(url.toStdString());
  }
}

bool QWebViewCEF::canGoBack() {
  REQUIRE_MAIN_THREAD();
  CefRefPtr<CefBrowser> browser = impl_->GetBrowser();
  if (browser)
    return browser->CanGoBack();
  return false;
}

bool QWebViewCEF::canGoForward() {
  REQUIRE_MAIN_THREAD();
  CefRefPtr<CefBrowser> browser = impl_->GetBrowser();
  if (browser)
    return browser->CanGoForward();
  return false;
}

void QWebViewCEF::goBack() {
  REQUIRE_MAIN_THREAD();
  CefRefPtr<CefBrowser> browser = impl_->GetBrowser();
  if (browser)
    browser->GoBack();
}

void QWebViewCEF::goForward() {
  REQUIRE_MAIN_THREAD();
  CefRefPtr<CefBrowser> browser = impl_->GetBrowser();
  if (browser)
    browser->GoForward();
}

bool QWebViewCEF::isLoading() {
  REQUIRE_MAIN_THREAD();
  CefRefPtr<CefBrowser> browser = impl_->GetBrowser();
  if (browser)
    return browser->IsLoading();
  return false;
}

void QWebViewCEF::reload() {
  REQUIRE_MAIN_THREAD();
  CefRefPtr<CefBrowser> browser = impl_->GetBrowser();
  if (browser)
    browser->Reload();
}

void QWebViewCEF::stopLoading() {
  REQUIRE_MAIN_THREAD();
  CefRefPtr<CefBrowser> browser = impl_->GetBrowser();
  if (browser)
    browser->StopLoad();
}

void QWebViewCEF::showDevTools() {
  REQUIRE_MAIN_THREAD();
  RootWindowQt* r = impl_->GetRootWindowQt();
  if (r)
    r->ShowDevTools();
}

void QWebViewCEF::closeDevTools() {
  REQUIRE_MAIN_THREAD();
  RootWindowQt* r = impl_->GetRootWindowQt();
  if (r)
    r->CloseDevTools();
}

bool QWebViewCEF::isMute() {
  REQUIRE_MAIN_THREAD();
  CefRefPtr<CefBrowser> browser = impl_->GetBrowser();
  if (browser) {
    CefRefPtr<CefBrowserHost> host = browser->GetHost();
    if (host)
      return host->IsAudioMuted();
  }
  return false;
}

void QWebViewCEF::setMute(bool mute) {
  REQUIRE_MAIN_THREAD();
  CefRefPtr<CefBrowser> browser = impl_->GetBrowser();
  if (browser) {
    CefRefPtr<CefBrowserHost> host = browser->GetHost();
    if (host)
      host->SetAudioMuted(mute);
  }
}

bool QWebViewCEF::isOSREnabled() {
  return impl_->isOffscreenRender();
}

void QWebViewCEF::setOSREnabled(bool enabled) {
  impl_->setOffscreenRender(enabled);
}

bool QWebViewCEF::postMessage(const QString& message) {
  REQUIRE_MAIN_THREAD();
  CefString str;
  str.FromWString(QString("window.onMessage('%1')").arg(message).toStdWString());

  CefRefPtr<CefBrowser> browser = impl_->GetBrowser();
  if (browser) {
    CefRefPtr<CefFrame> main = browser->GetMainFrame();
    if (main) {
      main->ExecuteJavaScript(str, main->GetURL(), 0);
      return true;
    }
  }

  return false;
}

bool InitializeCEF(bool supportOSR) {
  return QWebViewCEF::Initialize(supportOSR);
}

void UnInitializeCEF() {
  QWebViewCEF::UnInitialize();
}

QWebView* CreateCEF(QWidget* parent) {
  return new QWebViewCEF(parent);
}