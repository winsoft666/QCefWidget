#include "QCefDevToolsWnd.h"
#include "QCefWidgetImpl.h"
#include "Include/QCefWidget.h"
#include "QCefManager.h"

QCefDevToolsWnd::QCefDevToolsWnd(CefRefPtr<CefBrowser> targetBrowser, QWidget *parent /*= nullptr*/)
    : cefWidget_(nullptr)
    , targetBrowser_(targetBrowser) {
  setAttribute(Qt::WA_DeleteOnClose, true);
  setupUi();

  cefWidget_->setStyleSheet(QString("QWidget{background-color: rgba(0,255,0,1);}"));

  if (targetBrowser_) {
    cefWidget_->pImpl_->createDevTools(targetBrowser_);
  }
}

QCefDevToolsWnd::~QCefDevToolsWnd() {  }

void QCefDevToolsWnd::closeEvent(QCloseEvent *event) {
  QCefManager::getInstance().devToolsClosedNotify(this);
  event->accept();
}

void QCefDevToolsWnd::setupUi() {
  this->setObjectName("QCefWidget_DevToolsWnd");
  this->setWindowTitle("DevTools");
  cefWidget_ = new QCefWidget();
  Q_ASSERT(cefWidget_);
  if (cefWidget_) {
    cefWidget_->setObjectName("cefWidget");
    cefWidget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  }

  this->setCentralWidget(cefWidget_);
  resize(800, 600);
  this->show();
}
