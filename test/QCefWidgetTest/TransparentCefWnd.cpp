#include "TransparentCefWnd.h"
#include <QDebug>
#include <QMouseEvent>
#include "QCefWidget.h"
#include "QCefOpenGLWidget.h"

#pragma execution_character_set("utf-8")

TransparentCefWnd::TransparentCefWnd(bool bCefOpenGLWidget, QWidget *parent)
    : QWidget(parent)
    , bCefOpenGLWidget_(bCefOpenGLWidget)
    , cefWidget_(nullptr)
    , cefOpenGLWidget_(nullptr)
    , leftMousePressed_(false) {

  setupUi();
}

TransparentCefWnd::~TransparentCefWnd() {
  qInfo() << "TransparentCefWnd::~TransparentCefWnd, this: " << this;
}

void TransparentCefWnd::setOsrEnabled(bool b) {
  if (bCefOpenGLWidget_) {
    cefOpenGLWidget_->setOsrEnabled(b);
  }
  else {
    cefWidget_->setOsrEnabled(b);
  }
}

void TransparentCefWnd::navigateToUrl(const QString &url) {
  if (bCefOpenGLWidget_) {
    cefOpenGLWidget_->navigateToUrl(url);
  }
  else {
    cefWidget_->navigateToUrl(url);
  }
}

void TransparentCefWnd::showDevTools() {
  if (bCefOpenGLWidget_) {
    cefOpenGLWidget_->showDevTools();
  }
  else {
    cefWidget_->showDevTools();
  }
}

void TransparentCefWnd::closeDevTools() {
  if (bCefOpenGLWidget_) {
    cefOpenGLWidget_->closeDevTools();
  }
  else {
    cefWidget_->closeDevTools();
  }
}

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

  QHBoxLayout *hLayout = new QHBoxLayout();

  if (bCefOpenGLWidget_) {
    cefOpenGLWidget_ = new QCefOpenGLWidget();
    cefOpenGLWidget_->setObjectName("cefOpenGLWidget");
    cefOpenGLWidget_->setBrowserBackgroundColor(QColor(0, 0, 0, 0));
    hLayout->addWidget(cefOpenGLWidget_);
  }
  else {
    cefWidget_ = new QCefWidget();
    cefWidget_->setObjectName("cefView");
    cefWidget_->setBrowserBackgroundColor(QColor(0, 0, 0, 0));
    hLayout->addWidget(cefWidget_);
  }

  this->setLayout(hLayout);

  resize(800, 800);
}
