#include "Include/QCefOpenGLWidget.h"
#ifndef QT_NO_OPENGL
#include "QCefWidgetImpl.h"
#include <QAbstractEventDispatcher>
#include <QDebug>
#include <QResizeEvent>
#include <QVariant>
#include <include/base/cef_logging.h>
#include <include/cef_app.h>
#include <include/cef_browser.h>
#include <include/cef_frame.h>
#include <include/cef_sandbox_win.h>
#include "QCefManager.h"

QCefOpenGLWidget::QCefOpenGLWidget(QWidget *parent /*= nullptr*/)
    : QOpenGLWidget(parent) {
  pImpl_ = std::make_unique<QCefWidgetImpl>(WidgetType::WT_OpenGLWidget, this);
  setAttribute(Qt::WA_NativeWindow, true);
  setAttribute(Qt::WA_InputMethodEnabled, true);
}

QCefOpenGLWidget::~QCefOpenGLWidget() {
  qInfo() << "QCefOpenGLWidget::~QCefOpenGLWidget, this: " << this;
  pImpl_.reset();
  QCefManager::getInstance().uninitializeCef();
}

void QCefOpenGLWidget::navigateToUrl(const QString &url) {
  Q_ASSERT(pImpl_);
  pImpl_->navigateToUrl(url);
}

bool QCefOpenGLWidget::canGoBack() {
  Q_ASSERT(pImpl_);
  return pImpl_->canGoBack();
}

bool QCefOpenGLWidget::canGoForward() {
  Q_ASSERT(pImpl_);
  return pImpl_->canGoForward();
}

void QCefOpenGLWidget::goBack() {
  Q_ASSERT(pImpl_);
  pImpl_->goBack();
}

void QCefOpenGLWidget::goForward() {
  Q_ASSERT(pImpl_);
  pImpl_->goForward();
}

bool QCefOpenGLWidget::isLoadingBrowser() {
  Q_ASSERT(pImpl_);
  return pImpl_->isLoadingBrowser();
}

void QCefOpenGLWidget::reloadBrowser() {
  Q_ASSERT(pImpl_);
  pImpl_->reloadBrowser();
}

void QCefOpenGLWidget::stopLoadBrowser() {
  Q_ASSERT(pImpl_);
  pImpl_->stopLoadBrowser();
}

bool QCefOpenGLWidget::triggerEvent(const QString &name, const QCefEvent &event) {
  Q_ASSERT(pImpl_);
  return pImpl_->triggerEvent(name, event, QCefBrowserHandler::MAIN_FRAME);
}

bool QCefOpenGLWidget::triggerEvent(const QString &name, const QCefEvent &event, int frameId) {
  Q_ASSERT(pImpl_);
  return pImpl_->triggerEvent(name, event, frameId);
}

bool QCefOpenGLWidget::broadcastEvent(const QString &name, const QCefEvent &event) {
  Q_ASSERT(pImpl_);
  return pImpl_->triggerEvent(name, event, QCefBrowserHandler::ALL_FRAMES);
}

bool QCefOpenGLWidget::responseQCefQuery(const QCefQuery &query) {
  Q_ASSERT(pImpl_);
  return pImpl_->responseQCefQuery(query);
}

void QCefOpenGLWidget::executeJavascript(const QString &javascript) {
  Q_ASSERT(pImpl_);
  pImpl_->executeJavascript(javascript);
}

void QCefOpenGLWidget::setFPS(int fps) {
  Q_ASSERT(pImpl_);
  pImpl_->setFPS(fps);
}

int QCefOpenGLWidget::fps() const {
  Q_ASSERT(pImpl_);
  return pImpl_->fps();
}

void QCefOpenGLWidget::setBrowserBackgroundColor(const QColor &color) {
  Q_ASSERT(pImpl_);
  pImpl_->setBrowserBackgroundColor(color);
}

QColor QCefOpenGLWidget::browserBackgroundColor() const {
  Q_ASSERT(pImpl_);
  return pImpl_->browserBackgroundColor();
}

void QCefOpenGLWidget::showDevTools() { QCefManager::getInstance().showDevTools(this); }

void QCefOpenGLWidget::closeDevTools() { QCefManager::getInstance().closeDevTools(this); }

bool QCefOpenGLWidget::nativeEvent(const QByteArray &eventType, void *message, long *result) {
  Q_ASSERT(pImpl_);
  return pImpl_->nativeEvent(eventType, message, result);
}

void QCefOpenGLWidget::paintEvent(QPaintEvent *event) {
  Q_ASSERT(pImpl_);
  if (!pImpl_->openGLPaintEventHandle(event)) {
    QOpenGLWidget::paintEvent(event);
  }
}

void QCefOpenGLWidget::setVisible(bool visible) {
  QOpenGLWidget::setVisible(visible);
  Q_ASSERT(pImpl_);
  pImpl_->setVisible(visible);
}
#endif