#include "TransparentCefWnd.h"
#include <QDebug>
#include <QMouseEvent>
#include "QCefWidget.h"
#include "QCefOpenGLWidget.h"

#pragma execution_character_set("utf-8")

TransparentCefWnd::TransparentCefWnd(QWidget *parent)
    : QWidget(parent)
    , cefWidget_(nullptr)
    , cefOpenGLWidget_(nullptr)
    , leftMousePressed_(false) {

  setupUi();

  if (cefWidget_) {
    cefWidget_->setBrowserBackgroundColor(QColor(0, 0, 0, 0));
    cefWidget_->navigateToUrl(QCoreApplication::applicationDirPath() + u8"/TestResource/Tree.html");
  }

  if (cefOpenGLWidget_) {
    cefOpenGLWidget_->setBrowserBackgroundColor(QColor(0, 0, 0, 0));
    cefOpenGLWidget_->navigateToUrl(QCoreApplication::applicationDirPath() + u8"/TestResource/Tree.html");
  }
}

TransparentCefWnd::~TransparentCefWnd() {}

void TransparentCefWnd::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    leftMousePressed_ = true;
    beginDragPos_ = event->globalPos() - this->frameGeometry().topLeft();
  }
  return QWidget::mousePressEvent(event);
}

void TransparentCefWnd::mouseMoveEvent(QMouseEvent *event) {
  QPoint globalPoint = event->globalPos();
  if (leftMousePressed_) {
    move(event->globalPos() - beginDragPos_);
    event->accept();
  }
  QWidget::mouseMoveEvent(event);
}

void TransparentCefWnd::mouseReleaseEvent(QMouseEvent *event) {
  leftMousePressed_ = false;
  return QWidget::mouseReleaseEvent(event);
}

void TransparentCefWnd::setupUi() {
  this->setObjectName("transparentCefWidget");
  this->setWindowTitle("Transparent CEF Demo");

  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowFlags(Qt::FramelessWindowHint);
  setMouseTracking(true);

  cefWidget_ = new QCefWidget();
  if(cefWidget_)
    cefWidget_->setObjectName("cefView");

  cefOpenGLWidget_ = new QCefOpenGLWidget();
  if(cefOpenGLWidget_)
    cefOpenGLWidget_->setObjectName("cefOpenGLWidget");

  QHBoxLayout *hLayout = new QHBoxLayout();
  hLayout->setSpacing(30);
  if(cefWidget_)
    hLayout->addWidget(cefWidget_);
  if(cefOpenGLWidget_)
    hLayout->addWidget(cefOpenGLWidget_);

  this->setLayout(hLayout);

  this->setFixedWidth(1000);
  this->setFixedHeight(400);
}
