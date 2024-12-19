#include "SampleWnd.h"
#include "QWebView/Creator.h"
#include "QWebView/Manager.h"
#include <QTimer>

SampleWnd::SampleWnd(QWebView::BrowserEngine engine, QWidget* parent /*= nullptr*/) :
    QWidget(parent) {
  if (engine == QWebView::BrowserEngine::WebView2) {
    webview_ = CreateWebView2();
    setWindowTitle(windowTitle() + "当前是 WebView2 内核");
  }
  else if (engine == QWebView::BrowserEngine::CEF) {
    webview_ = CreateCEF();
    setWindowTitle(windowTitle() + "当前是 CEF 内核");
  }
  webview_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  connect(webview_, &QWebView::messageReceived, this, [this](QString message) {
    QMessageBox::information(this, "Message from JavaSript", message, QMessageBox::Ok);
  });

  QPushButton* btnClose = new QPushButton("调用QWidget::close()");
  connect(btnClose, &QPushButton::clicked, this, [this]() {
    this->close();
  });

  QPushButton* btnCloseLeft = new QPushButton("调用QWebView::close()");
  connect(btnCloseLeft, &QPushButton::clicked, this, [this]() {
    webview_->close();
  });

  QPushButton* btnQuit = new QPushButton("调用QCoreApplication::quit()");
  connect(btnQuit, &QPushButton::clicked, this, [this]() {
    QMessageBox::warning(this, "使用错误", "不能直接调用 QCoreApplication::quit() 来退出应用程序！");
  });

  QHBoxLayout* lTop = new QHBoxLayout();
  lTop->addWidget(webview_);

  QHBoxLayout* lBottom = new QHBoxLayout();
  lBottom->addWidget(btnCloseLeft);
  lBottom->addWidget(btnClose);
  lBottom->addWidget(btnQuit);

  QVBoxLayout* v = new QVBoxLayout();
  v->addLayout(lTop);
  v->addLayout(lBottom);

  setLayout(v);

  webview_->navigate(QString("file:///%1").arg(QCoreApplication::applicationDirPath() + u8"/asserts/test.html"));

  QTimer* timer = new QTimer(this);

  timer->setInterval(1500);
  connect(timer, &QTimer::timeout, this, [this]() {
    webview_->postMessage("hi, this is C++ message.");
  });
  timer->start();
}

void SampleWnd::closeEvent(QCloseEvent* e) {
  QWebViewManager::TopLevelWndCloseState state = QWebViewManager::Get()->topLevelWinCloseState(this);
  qDebug() << ">>>> SampleWnd closeEvent" << state;

  if (state == QWebViewManager::TopLevelWndCloseState::NotStart) {
    if (QMessageBox::Yes == QMessageBox::question(this, "警告", "确定要退出示例程序吗？")) {
      QWebViewManager::Get()->prepareToCloseTopLevelWindow(this);
    }
    qDebug() << ">>>> SampleWnd closeEvent: ignore";
    e->ignore();
  }
  else if (state == QWebViewManager::TopLevelWndCloseState::BrowsersClosing) {
    qDebug() << ">>>> SampleWnd closeEvent: ignore";
    e->ignore();
  }
  else if (state == QWebViewManager::TopLevelWndCloseState::BrowsersClosed) {
    qDebug() << ">>>> SampleWnd closeEvent: accept";
    e->accept();
  }
  else if (state == QWebViewManager::TopLevelWndCloseState::Closed) {
    Q_UNREACHABLE();
  }
}
