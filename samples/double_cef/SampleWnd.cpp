#include "SampleWnd.h"
#include "QWebView/Core.h"
#include "QWebView/Creator.h"
#include "QWebView/Manager.h"

SampleWnd::SampleWnd(QWidget* parent /*= nullptr*/) :
    QWidget(parent) {
  webviewLeft_ = CreateCEF();
  webviewLeft_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  webviewRight_ = CreateCEF();
  webviewRight_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  QPushButton* btnClose = new QPushButton("调用QWidget::close()");
  connect(btnClose, &QPushButton::clicked, this, [this]() {
    this->close();
  });

  QPushButton* btnCloseLeft = new QPushButton("调用左边的QWebView::close()");
  connect(btnCloseLeft, &QPushButton::clicked, this, [this]() {
    webviewLeft_->close();
  });

  QPushButton* btnCloseRight = new QPushButton("调用右边的QWebView::close()");
  connect(btnCloseLeft, &QPushButton::clicked, this, [this]() {
    webviewRight_->close();
  });

  QPushButton* btnQuit = new QPushButton("调用QCoreApplication::quit()");
  connect(btnQuit, &QPushButton::clicked, this, [this]() {
    QMessageBox::warning(this, "使用错误", "不能直接调用 QCoreApplication::quit() 来退出应用程序！");
  });

  QHBoxLayout* lTop = new QHBoxLayout();
  lTop->addWidget(webviewLeft_);
  lTop->addWidget(webviewRight_);

  QHBoxLayout* lBottom = new QHBoxLayout();
  lBottom->addWidget(btnCloseLeft);
  lBottom->addWidget(btnCloseRight);
  lBottom->addWidget(btnClose);
  lBottom->addWidget(btnQuit);

  QVBoxLayout* v = new QVBoxLayout();
  v->addLayout(lTop);
  v->addLayout(lBottom);

  setLayout(v);

  webviewLeft_->navigate("https://www.baidu.com");
  webviewRight_->navigate("https://www.baidu.com");
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
