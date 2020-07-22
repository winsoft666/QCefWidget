#include "FramelessCEF.h"
#include "QCefWidget.h"
#include <QtWidgets>

FramelessCEF::FramelessCEF(QWidget *parent /*= nullptr*/)
    : FramelessWindow(false, parent) {
  this->setResizeable(true);
  this->setAllWidgetMouseTracking(this);

  setupUi();

  pCefWidget_->navigateToUrl("file:///D:/sourcecode/private-project/QCefWidget/out/QCefWidgetTest/Debug/TestResource/TestPage.html");
}

FramelessCEF::~FramelessCEF() {}

void FramelessCEF::setupUi() {
  this->setObjectName("FramelessCEF");
  this->setStyleSheet("background-color: rgba(40,40,40,1);");

  pCefWidget_ = new QCefWidget();
  pCefWidget_->setObjectName("cefWidget");
  pCefWidget_->setOsrEnabled(false);
  pCefWidget_->setContextMenuEnabled(false);
  pCefWidget_->setAutoShowDevToolsContextMenu(true);

  QHBoxLayout *hlMain = new QHBoxLayout();
  hlMain->setContentsMargins(0, 0, 0, 0);
  hlMain->setSpacing(0);
  hlMain->addWidget(pCefWidget_);

  this->setLayout(hlMain);

  this->resize(sizeHint());
}

QSize FramelessCEF::sizeHint() const {
  return QSize(1000,700);
}
