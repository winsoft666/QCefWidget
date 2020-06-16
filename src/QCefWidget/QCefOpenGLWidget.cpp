#include "Include/QCefOpenGLWidget.h"
#include "QCefWidgetImplement.h"
#include <QAbstractEventDispatcher>
#include <QDebug>
#include <QResizeEvent>
#include <QVariant>
#include <include/base/cef_logging.h>
#include <include/cef_app.h>
#include <include/cef_browser.h>
#include <include/cef_frame.h>
#include <include/cef_sandbox_win.h>
#include "CefManager.h"

QCefOpenGLWidget::QCefOpenGLWidget(QWidget *parent /*= nullptr*/) : QOpenGLWidget(parent) {
  pImpl_ = std::make_unique<QCefWidgetImplement>(WidgetType::OpenGLWidget, this);
  setAttribute(Qt::WA_NativeWindow, true);
  setAttribute(Qt::WA_InputMethodEnabled, true);
}

QCefOpenGLWidget::~QCefOpenGLWidget() {
  pImpl_.reset();
  CefManager::getInstance().uninitializeCef();
}

void QCefOpenGLWidget::navigateToUrl(const QString &url) {
  Q_ASSERT(pImpl_);
  pImpl_->navigateToUrl(url);
}

bool QCefOpenGLWidget::browserCanGoBack() {
  Q_ASSERT(pImpl_);
  return pImpl_->browserCanGoBack();
}

bool QCefOpenGLWidget::browserCanGoForward() {
  Q_ASSERT(pImpl_);
  return pImpl_->browserCanGoForward();
}

void QCefOpenGLWidget::browserGoBack() {
  Q_ASSERT(pImpl_);
  pImpl_->browserGoBack();
}

void QCefOpenGLWidget::browserGoForward() {
  Q_ASSERT(pImpl_);
  pImpl_->browserGoForward();
}

bool QCefOpenGLWidget::browserIsLoading() {
  Q_ASSERT(pImpl_);
  return pImpl_->browserIsLoading();
}

void QCefOpenGLWidget::browserReload() {
  Q_ASSERT(pImpl_);
  pImpl_->browserReload();
}

void QCefOpenGLWidget::browserStopLoad() {
  Q_ASSERT(pImpl_);
  pImpl_->browserStopLoad();
}

bool QCefOpenGLWidget::triggerEvent(const QString &name, const QCefEvent &event) {
  Q_ASSERT(pImpl_);
  return pImpl_->triggerEvent(name, event, CefBrowserHandler::MAIN_FRAME);
}

bool QCefOpenGLWidget::triggerEvent(const QString &name, const QCefEvent &event, int frameId) {
  Q_ASSERT(pImpl_);
  return pImpl_->triggerEvent(name, event, frameId);
}

bool QCefOpenGLWidget::broadcastEvent(const QString &name, const QCefEvent &event) {
  Q_ASSERT(pImpl_);
  return pImpl_->triggerEvent(name, event, CefBrowserHandler::ALL_FRAMES);
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

void QCefOpenGLWidget::setBackgroundColor(const QColor &color) {
  Q_ASSERT(pImpl_);
  pImpl_->setBackgroundColor(color);
}

QColor QCefOpenGLWidget::backgroundColor() const {
  Q_ASSERT(pImpl_);
  return pImpl_->backgroundColor();
}

bool QCefOpenGLWidget::nativeEvent(const QByteArray &eventType, void *message, long *result) {
  Q_ASSERT(pImpl_);
  return pImpl_->nativeEvent(eventType, message, result);
}

void QCefOpenGLWidget::paintEvent(QPaintEvent *event) {
  Q_ASSERT(pImpl_);
  pImpl_->openGLPaintEvent(event);
}