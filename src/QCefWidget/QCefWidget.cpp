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

QCefWidget::QCefWidget(const QString &url, QWidget *parent)
    : QWidget(parent) {
  pImpl_ = std::make_unique<QCefWidgetImpl>(WidgetType::WT_Widget, this, url);
  setAttribute(Qt::WA_NativeWindow, true);
  setAttribute(Qt::WA_InputMethodEnabled, true);
}

QCefWidget::~QCefWidget() { qDebug() << "QCefWidget::~QCefWidget"; }

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
  return pImpl_->triggerEvent(name, event);
}

bool QCefWidget::responseCefQuery(const QCefQuery &query) {
  Q_ASSERT(pImpl_);
  return pImpl_->responseCefQuery(query);
}

void QCefWidget::executeJavascript(const QString &javascript) {
  Q_ASSERT(pImpl_);
  pImpl_->executeJavascript(javascript);
}

bool QCefWidget::setOsrEnabled(bool b) {
  Q_ASSERT(pImpl_);
  return pImpl_->setOsrEnabled(b);
}

bool QCefWidget::osrEnabled() const {
  Q_ASSERT(pImpl_);
  return pImpl_->browserSetting().osrEnabled;
}

void QCefWidget::setContextMenuEnabled(bool b) {
  Q_ASSERT(pImpl_);
  pImpl_->setContextMenuEnabled(b);
}

bool QCefWidget::contextMenuEnabled() const {
  Q_ASSERT(pImpl_);
  return pImpl_->browserSetting().contextMenuEnabled;
}

void QCefWidget::setAutoShowDevToolsContextMenu(bool b) {
  Q_ASSERT(pImpl_);
  pImpl_->setAutoShowDevToolsContextMenu(b);
}

bool QCefWidget::autoShowDevToolsContextMenu() const {
  Q_ASSERT(pImpl_);
  return pImpl_->browserSetting().autoShowDevToolsContextMenu;
}

void QCefWidget::setAllowExecuteUnknownProtocolViaOS(bool b) {
  Q_ASSERT(pImpl_);
  pImpl_->setAllowExecuteUnknownProtocolViaOS(b);
}

bool QCefWidget::allowExecuteUnknownProtocolViaOS() const {
  Q_ASSERT(pImpl_);
  return pImpl_->browserSetting().executeUnknownProtocolViaOS;
}

void QCefWidget::setFPS(int fps) {
  Q_ASSERT(pImpl_);
  pImpl_->setFPS(fps);
}

int QCefWidget::fps() const {
  Q_ASSERT(pImpl_);
  return pImpl_->browserSetting().fps;
}

void QCefWidget::setBrowserBackgroundColor(const QColor &color) {
  Q_ASSERT(pImpl_);
  pImpl_->setBrowserBackgroundColor(color);
}

QColor QCefWidget::browserBackgroundColor() const {
  Q_ASSERT(pImpl_);
  return pImpl_->browserSetting().backgroundColor;
}

void QCefWidget::showDevTools() { QCefManager::getInstance().showDevTools(this); }

void QCefWidget::closeDevTools() { QCefManager::getInstance().closeDevTools(this); }

bool QCefWidget::nativeEvent(const QByteArray &eventType, void *message, long *result) {
  // pImpl_ may be empty, if we call winId in QCefWidgetImpl::QCefWidgetImpl().
  if (!pImpl_)
    return QWidget::nativeEvent(eventType, message, result);
  if (pImpl_->nativeEvent(eventType, message, result))
    return true;
  return QWidget::nativeEvent(eventType, message, result);
}

bool QCefWidget::event(QEvent *event) {
  // pImpl_ may be empty, if we call winId in QCefWidgetImpl::QCefWidgetImpl().
  if (!pImpl_)
    return QWidget::event(event);

  if (pImpl_->event(event)) {
    return true;
  }
  return QWidget::event(event);
}

void QCefWidget::paintEvent(QPaintEvent *event) {
  Q_ASSERT(pImpl_);
  if (!pImpl_ || !pImpl_->paintEventHandle(event)) {
    QWidget::paintEvent(event);
  }
}

void QCefWidget::setVisible(bool visible) {
  QWidget::setVisible(visible);
  Q_ASSERT(pImpl_);
  if (pImpl_)
    pImpl_->setVisible(visible);
}