#include "PopupWnd.h"
#include "QWebView/Creator.h"

PopupWnd::PopupWnd(QString url, QWebView::BrowserEngine engine, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    engine_(engine) {
  setWindowTitle("弹窗");

  if (engine == QWebView::BrowserEngine::CEF)
    webview_ = CreateCEF();
  else if (engine == QWebView::BrowserEngine::WebView2)
    webview_ = CreateWebView2();

  webview_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  connect(webview_, &QWebView::newPopupWindow, this, &PopupWnd::onNewPopupWindow);

  QHBoxLayout* lTop = new QHBoxLayout();
  lTop->addWidget(webview_);

  setLayout(lTop);

  webview_->navigate(url);
}

void PopupWnd::closeEvent(QCloseEvent* e) {
  QWebViewManager::TopLevelWndCloseState state = QWebViewManager::Get()->topLevelWinCloseState(this);
  qDebug() << ">>>> PopupWnd closeEvent" << state;

  if (state == QWebViewManager::TopLevelWndCloseState::NotStart) {
    QWebViewManager::Get()->prepareToCloseTopLevelWindow(this);

    qDebug() << ">>>> PopupWnd closeEvent: ignore";
    e->ignore();
  }
  else if (state == QWebViewManager::TopLevelWndCloseState::BrowsersClosing) {
    qDebug() << ">>>> PopupWnd closeEvent: ignore";
    e->ignore();
  }
  else if (state == QWebViewManager::TopLevelWndCloseState::BrowsersClosed) {
    qDebug() << ">>>> PopupWnd closeEvent: accept";
    e->accept();
  }
  else if (state == QWebViewManager::TopLevelWndCloseState::Closed) {
    Q_UNREACHABLE();
  }
}

void PopupWnd::onNewPopupWindow(QString url) {
  PopupWnd* popup = new PopupWnd(url, engine_);
  popup->resize(800, 600);
  popup->show();
}