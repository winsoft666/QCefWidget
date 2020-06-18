#include "Include/QCefWidget.h"
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
#include "CefManager.h"

QCefWidget::QCefWidget(QWidget *parent) : QWidget(parent) {
  pImpl_ = std::make_unique<QCefWidgetImpl>(WidgetType::WT_Widget, this);
  setAttribute(Qt::WA_NativeWindow, true);
  setAttribute(Qt::WA_InputMethodEnabled, true);
}

QCefWidget::~QCefWidget() {
  pImpl_.reset();
  CefManager::getInstance().uninitializeCef();
}

void QCefWidget::navigateToUrl(const QString &url) {
  Q_ASSERT(pImpl_);
  pImpl_->navigateToUrl(url);
}

bool QCefWidget::browserCanGoBack() {
  Q_ASSERT(pImpl_);
  return pImpl_->browserCanGoBack();
}

bool QCefWidget::browserCanGoForward() {
  Q_ASSERT(pImpl_);
  return pImpl_->browserCanGoForward();
}

void QCefWidget::browserGoBack() {
  Q_ASSERT(pImpl_);
  pImpl_->browserGoBack();
}

void QCefWidget::browserGoForward() {
  Q_ASSERT(pImpl_);
  pImpl_->browserGoForward();
}

bool QCefWidget::browserIsLoading() {
  Q_ASSERT(pImpl_);
  return pImpl_->browserIsLoading();
}

void QCefWidget::browserReload() {
  Q_ASSERT(pImpl_);
  pImpl_->browserReload();
}

void QCefWidget::browserStopLoad() {
  Q_ASSERT(pImpl_);
  pImpl_->browserStopLoad();
}

bool QCefWidget::triggerEvent(const QString &name, const QCefEvent &event) {
  Q_ASSERT(pImpl_);
  return pImpl_->triggerEvent(name, event, CefBrowserHandler::MAIN_FRAME);
}

bool QCefWidget::triggerEvent(const QString &name, const QCefEvent &event, int frameId) {
  Q_ASSERT(pImpl_);
  return pImpl_->triggerEvent(name, event, frameId);
}

bool QCefWidget::broadcastEvent(const QString &name, const QCefEvent &event) {
  Q_ASSERT(pImpl_);
  return pImpl_->triggerEvent(name, event, CefBrowserHandler::ALL_FRAMES);
}

bool QCefWidget::responseQCefQuery(const QCefQuery &query) {
  Q_ASSERT(pImpl_);
  return pImpl_->responseQCefQuery(query);
}

void QCefWidget::executeJavascript(const QString &javascript) {
  Q_ASSERT(pImpl_);
  pImpl_->executeJavascript(javascript);
}

void QCefWidget::setFPS(int fps) {
  Q_ASSERT(pImpl_);
  pImpl_->setFPS(fps);
}

int QCefWidget::fps() const {
  Q_ASSERT(pImpl_);
  return pImpl_->fps();
}

void QCefWidget::setBackgroundColor(const QColor &color) {
  Q_ASSERT(pImpl_);
  pImpl_->setBackgroundColor(color);
}

QColor QCefWidget::backgroundColor() const {
  Q_ASSERT(pImpl_);
  return pImpl_->backgroundColor();
}

bool QCefWidget::nativeEvent(const QByteArray &eventType, void *message, long *result) {
  Q_ASSERT(pImpl_);
  return pImpl_->nativeEvent(eventType, message, result);
}

void QCefWidget::paintEvent(QPaintEvent *event) {
  Q_ASSERT(pImpl_);
  pImpl_->paintEvent(event);
}
