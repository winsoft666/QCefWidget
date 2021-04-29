#include "QCefDevToolsWnd.h"
#include "QCefWidgetImpl.h"
#include "Include/QCefWidget.h"
#include "QCefManager.h"
#include <QDebug>

QCefDevToolsWnd::QCefDevToolsWnd(CefRefPtr<CefBrowser> targetBrowser,
                                 QWidget* parent /*= nullptr*/) :
    cefWidget_(nullptr),
    targetBrowser_(targetBrowser) {
  setAttribute(Qt::WA_DeleteOnClose, true);
  setupUi();

  cefWidget_->setStyleSheet( QString("QWidget {background-color: rgba(255,255,255,1);}"));

  if (targetBrowser_) {
    QString str = QString::fromStdWString( targetBrowser_->GetMainFrame()->GetURL().ToWString());
    setWindowTitle(QString("[DevTools] %1").arg(str));
    cefWidget_->pImpl_->createDevTools(targetBrowser_);
  }
}

QCefDevToolsWnd::~QCefDevToolsWnd() {
  qDebug().noquote() << "QCefDevToolsWnd::~QCefDevToolsWnd";
}

void QCefDevToolsWnd::closeEvent(QCloseEvent* event) {
  QCefManager::getInstance().devToolsClosedNotify(this);
  event->accept();
}

void QCefDevToolsWnd::setupUi() {
  this->setObjectName("QCefWidget_DevToolsWnd");
  cefWidget_ = new QCefWidget();
  Q_ASSERT(cefWidget_);
  if (cefWidget_) {
    cefWidget_->setObjectName("cefWidget");
    cefWidget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cefWidget_->setOsrEnabled(false);  // FORCE NON-OSR
    cefWidget_->setAutoShowDevToolsContextMenu(false);
  }

  this->setCentralWidget(cefWidget_);
  resize(1024, 600);
  this->show();
}
