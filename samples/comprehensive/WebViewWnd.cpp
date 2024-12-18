#include "WebViewWnd.h"
#include "QWebView/Creator.h"
#include "QWebView/Manager.h"

WebViewWnd::WebViewWnd(bool frameless, bool translucentWindowBackground, QWebView::BrowserEngine engine, QWidget* parent /*= nullptr*/) :
    WebViewWndBase<QWidget>(frameless, translucentWindowBackground, parent),
    framelessWindow_(frameless),
    translucentWindowBackground_(translucentWindowBackground),
    engine_(engine),
    allowExecuteUnknownProtocolViaOS_(false),
    usingHideInsteadClose_(false),
    forceClose_(false),
    osrEnabled_(false),
    maximumFps_(25),
    windowBkColor_(255, 255, 255, 255),
    browserBkColor_(255, 255, 255, 255),
    webview_(nullptr) {
  this->setResizeable(true);
}

WebViewWnd::~WebViewWnd() {}

void WebViewWnd::setupUi() {
  this->setWindowIcon(QIcon(":/Sample/images/logo.svg"));

  QString windowQss = QString("background-color: rgba(%1,%2,%3,%4);")
                          .arg(windowBkColor_.red())
                          .arg(windowBkColor_.green())
                          .arg(windowBkColor_.blue())
                          .arg(windowBkColor_.alpha());
  this->setStyleSheet(windowQss);

  if (engine_ == QWebView::BrowserEngine::WebView2) {
    webview_ = CreateWebView2();
  }
  else if (engine_ == QWebView::BrowserEngine::CEF) {
    webview_ = CreateCEF();
    webview_->setOSREnabled(osrEnabled_);
  }

  //pCefWidget_->setContextMenuEnabled(contextMenuEnabled_);
  //pCefWidget_->setBrowserBackgroundColor(browserBkColor_);
  //pCefWidget_->setAllowExecuteUnknownProtocolViaOS(
  //    allowExecuteUnknownProtocolViaOS_);
  if (!translucentWindowBackground_)
    webview_->setStyleSheet("image: url(:/Sample/images/logo_blue.svg);");

  connect(webview_, &QWebView::messageReceived, this, [this](QString message) {
    QMessageBox::information(this, "Message from JavaSript", message, QMessageBox::Ok);
  });

  connect(webview_, &QWebView::titleChanged, this, [this](QString title) {
    curTitle_ = title;
    UpdateWindowTitle();
  });

  connect(webview_, &QWebView::urlChanged, this, [this](QString url) {
    curUrl_ = url;
    UpdateWindowTitle();
  });

  connect(webview_, &QWebView::loadStateChanged, this, [this](bool isLoading) {
    curLoading_ = isLoading;
    UpdateWindowTitle();
  });

  connect(webview_, &QWebView::fullscreenChanged, this, [this](bool fullscreen) {
    this->showFullScreen();
  });

  QHBoxLayout* hlMain = new QHBoxLayout();
  hlMain->setContentsMargins(2, 2, 2, 2);
  hlMain->setSpacing(0);
  hlMain->addWidget(webview_);

  this->setLayout(hlMain);

  this->resize(sizeHint());

  if (!initUrl_.isEmpty() && initUrl_ != "None")
    webview_->navigate(initUrl_);

  UpdateWindowTitle();
}

void WebViewWnd::setAllowExecuteUnknownProtocolViaOS(bool b) {
  allowExecuteUnknownProtocolViaOS_ = b;
}

bool WebViewWnd::allowExecuteUnknownProtocolViaOS() {
  return allowExecuteUnknownProtocolViaOS_;
}

bool WebViewWnd::framelessWindow() {
  return framelessWindow_;
}

bool WebViewWnd::translucentWindowBackground() {
  return translucentWindowBackground_;
}

QWebView::BrowserEngine WebViewWnd::browserEngine() const {
  return engine_;
}

bool WebViewWnd::osrEnabled() {
  return osrEnabled_;
}

void WebViewWnd::setOsrEnabled(bool b) {
  osrEnabled_ = b;
}

bool WebViewWnd::usingHideInsteadClose() {
  return usingHideInsteadClose_;
}

void WebViewWnd::setUsingHideInsteadClose(bool b) {
  usingHideInsteadClose_ = b;
}

QSize WebViewWnd::initSize() {
  return initSize_;
}

void WebViewWnd::setInitSize(QSize s) {
  initSize_ = s;
}

QString WebViewWnd::initUrl() {
  return initUrl_;
}

void WebViewWnd::setInitUrl(QString u) {
  initUrl_ = u;
}

int WebViewWnd::maximumFps() {
  return maximumFps_;
}

void WebViewWnd::setMaximumFps(int f) {
  maximumFps_ = f;
}

QColor WebViewWnd::windowBkColor() {
  return windowBkColor_;
}

void WebViewWnd::setWindowBkColor(QColor c) {
  windowBkColor_ = c;
}

QColor WebViewWnd::browserBkColor() {
  return browserBkColor_;
}

void WebViewWnd::setBrowserBkColor(QColor c) {
  browserBkColor_ = c;
}

void WebViewWnd::forceClose() {
  forceClose_ = true;
  close();
}

QSize WebViewWnd::sizeHint() const {
  return initSize_;
}

void WebViewWnd::showEvent(QShowEvent* event) {
  QWidget::showEvent(event);

  emit webViewWndVisibleChanged(true);
}

void WebViewWnd::hideEvent(QHideEvent* event) {
  QWidget::hideEvent(event);

  emit webViewWndVisibleChanged(false);
}

void WebViewWnd::closeEvent(QCloseEvent* e) {
  QWebViewManager::TopLevelWndCloseState state = QWebViewManager::Get()->topLevelWinCloseState(this);
  qDebug() << ">>>> WebViewWnd closeEvent" << state;

  if (state == QWebViewManager::TopLevelWndCloseState::NotStart) {
    if (forceClose_) {
      QWebViewManager::Get()->prepareToCloseTopLevelWindow(this);
    }
    else if (usingHideInsteadClose_) {
      this->hide();
    }
    else if (QMessageBox::Yes == QMessageBox::question(this, "警告", "确定要退出示例程序吗？")) {
      QWebViewManager::Get()->prepareToCloseTopLevelWindow(this);
    }

    qDebug() << ">>>> WebViewWnd closeEvent: ignore";
    e->ignore();
  }
  else if (state == QWebViewManager::TopLevelWndCloseState::BrowsersClosing) {
    qDebug() << ">>>> WebViewWnd closeEvent: ignore";
    e->ignore();
  }
  else if (state == QWebViewManager::TopLevelWndCloseState::BrowsersClosed) {
    qDebug() << ">>>> WebViewWnd closeEvent: accept";
    e->accept();
    emit webViewWndClosed();
  }
  else if (state == QWebViewManager::TopLevelWndCloseState::Closed) {
    Q_UNREACHABLE();
  }
}

void WebViewWnd::onNavigateToUrl(QString url) {
  if (webview_)
    webview_->navigate(url);
}

void WebViewWnd::onReload() {
  if (webview_)
    webview_->reload();
}

void WebViewWnd::onShowDevTools() {
  if (webview_)
    webview_->showDevTools();
}

void WebViewWnd::onCloseDevTools() {
  if (webview_)
    webview_->closeDevTools();
}

void WebViewWnd::onTriggerEvent() {
  webview_->postMessage("hi, this is c++ message.");
}

void WebViewWnd::onPopupWindow(const QString& url) {
  //WebViewWnd* pPopupWnd = new WebViewWnd(false, false);
  //pPopupWnd->setInitUrl(url);
  //pPopupWnd->setupUi();
  //pPopupWnd->resize(800, 600);
  //pPopupWnd->show();
}

void WebViewWnd::UpdateWindowTitle() {
  QString str = QString("%1[%2] %3").arg(curLoading_ ? "[Loading...] " : "").arg(curTitle_).arg(curUrl_);
  setWindowTitle(str);
}
