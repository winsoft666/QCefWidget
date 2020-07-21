#include "FramelessCEF.h"
#include "QCefWidget.h"
#include <QtWidgets>

FramelessCEF::FramelessCEF(QWidget *parent /*= nullptr*/)
    : FramelessWindow(false, parent) {
  this->setResizeable(true);
  this->setAllWidgetMouseTracking(this);
  this->setTitlebarRegion({QRect(0,0,1000,40)});

  setupUi();
}

FramelessCEF::~FramelessCEF() {}

void FramelessCEF::setupUi() {
  this->setObjectName("FramelessCEF");

  pCefWidget_ = new QCefWidget("file:///D:/sourcecode/private-project/QCefWidget/out/QCefWidgetTest/Debug/TestResource/draggable.html");
  pCefWidget_->setObjectName("cefWidget");
  pCefWidget_->setOsrEnabled(false);

  QHBoxLayout *hlMain = new QHBoxLayout();
  hlMain->setContentsMargins(0, 0, 0, 0);
  hlMain->setSpacing(0);
  hlMain->addWidget(pCefWidget_);

  this->setLayout(hlMain);

  this->resize(800, 600);
}
