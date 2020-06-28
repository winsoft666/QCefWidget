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
#include "QCefManager.h"

QCefWidget::QCefWidget(QWidget *parent)
    : QWidget(parent) {
  pImpl_ = std::make_unique<QCefWidgetImpl>(WidgetType::WT_Widget, this);
  setAttribute(Qt::WA_NativeWindow, true);
  setAttribute(Qt::WA_InputMethodEnabled, true);
}

QCefWidget::~QCefWidget() {
  qInfo() << "QCefWidget::~QCefWidget, this: " << this;
  pImpl_.reset();
  QCefManager::getInstance().uninitializeCef();
}

void QCefWidget::navigateToUrl(const QString &url) {
  Q_ASSERT(pImpl_);
  pImpl_->navigateToUrl(url);
}

bool QCefWidget::canGoBack() {
  Q_ASSERT(pImpl_);
  return pImpl_->canGoBack();
}

bool QCefWidget::canGoForward() {
  Q_ASSERT(pImpl_);
  return pImpl_->canGoForward();
}

void QCefWidget::goBack() {
  Q_ASSERT(pImpl_);
  pImpl_->goBack();
}

void QCefWidget::goForward() {
  Q_ASSERT(pImpl_);
  pImpl_->goForward();
}

bool QCefWidget::isLoadingBrowser() {
  Q_ASSERT(pImpl_);
  return pImpl_->isLoadingBrowser();
}

void QCefWidget::reloadBrowser() {
  Q_ASSERT(pImpl_);
  pImpl_->reloadBrowser();
}

void QCefWidget::stopLoadBrowser() {
  Q_ASSERT(pImpl_);
  pImpl_->stopLoadBrowser();
}

bool QCefWidget::triggerEvent(const QString &name, const QCefEvent &event) {
  Q_ASSERT(pImpl_);
  return pImpl_->triggerEvent(name, event, QCefBrowserHandler::MAIN_FRAME);
}

bool QCefWidget::triggerEvent(const QString &name, const QCefEvent &event, int frameId) {
  Q_ASSERT(pImpl_);
  return pImpl_->triggerEvent(name, event, frameId);
}

bool QCefWidget::broadcastEvent(const QString &name, const QCefEvent &event) {
  Q_ASSERT(pImpl_);
  return pImpl_->triggerEvent(name, event, QCefBrowserHandler::ALL_FRAMES);
}

bool QCefWidget::responseCefQuery(const QCefQuery &query) {
  Q_ASSERT(pImpl_);
  return pImpl_->responseCefQuery(query);
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

void QCefWidget::setBrowserBackgroundColor(const QColor &color) {
  Q_ASSERT(pImpl_);
  pImpl_->setBrowserBackgroundColor(color);
}

QColor QCefWidget::browserBackgroundColor() const {
  Q_ASSERT(pImpl_);
  return pImpl_->browserBackgroundColor();
}

void QCefWidget::showDevTools() { QCefManager::getInstance().showDevTools(this); }

void QCefWidget::closeDevTools() { QCefManager::getInstance().closeDevTools(this); }

bool QCefWidget::nativeEvent(const QByteArray &eventType, void *message, long *result) {
  Q_ASSERT(pImpl_);
  return pImpl_->nativeEvent(eventType, message, result);
}

void QCefWidget::paintEvent(QPaintEvent *event) {
  Q_ASSERT(pImpl_);
  if (!pImpl_->paintEventHandle(event)) {
    QWidget::paintEvent(event);
  }
}

void QCefWidget::setVisible(bool visible) {
  QWidget::setVisible(visible);
  Q_ASSERT(pImpl_);
  pImpl_->setVisible(visible);
}
