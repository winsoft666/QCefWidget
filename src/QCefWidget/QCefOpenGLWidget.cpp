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

QCefOpenGLWidget::QCefOpenGLWidget(const QString& url,
                                   QWidget* parent /*= nullptr*/) :
    QOpenGLWidget(parent) {
  setAttribute(Qt::WA_NativeWindow, true);
  setAttribute(Qt::WA_InputMethodEnabled, true);
  setAttribute(Qt::WA_StyledBackground, true);

  pImpl_ = std::make_unique<QCefWidgetImpl>(WidgetType::WT_OpenGLWidget, this);
  if (!url.isEmpty()) {
    pImpl_->navigateToUrl(url);
  }
}

QCefOpenGLWidget::~QCefOpenGLWidget() {
  qDebug().noquote() << "QCefOpenGLWidget::~QCefOpenGLWidget";
}

void QCefOpenGLWidget::navigateToUrl(const QString& url) {
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

void QCefOpenGLWidget::reloadBrowser(bool bIgnoreCache) {
  Q_ASSERT(pImpl_);
  pImpl_->reloadBrowser(bIgnoreCache);
}

void QCefOpenGLWidget::stopLoadBrowser() {
  Q_ASSERT(pImpl_);
  pImpl_->stopLoadBrowser();
}

bool QCefOpenGLWidget::triggerEvent(const QString& name,
                                    const QCefEvent& event) {
  Q_ASSERT(pImpl_);
  return pImpl_->triggerEvent(name, event);
}

bool QCefOpenGLWidget::responseCefQuery(const QCefQuery& query) {
  Q_ASSERT(pImpl_);
  return pImpl_->responseCefQuery(query);
}

void QCefOpenGLWidget::executeJavascript(const QString& javascript) {
  Q_ASSERT(pImpl_);
  pImpl_->executeJavascript(javascript);
}

bool QCefOpenGLWidget::setOsrEnabled(bool b) {
  Q_ASSERT(pImpl_);
  return pImpl_->setOsrEnabled(b);
}

bool QCefOpenGLWidget::osrEnabled() {
  Q_ASSERT(pImpl_);
  return pImpl_->browserSetting().osrEnabled;
}

void QCefOpenGLWidget::setContextMenuEnabled(bool b) {
  Q_ASSERT(pImpl_);
  pImpl_->setContextMenuEnabled(b);
}

bool QCefOpenGLWidget::contextMenuEnabled() {
  Q_ASSERT(pImpl_);
  return pImpl_->browserSetting().contextMenuEnabled;
}

void QCefOpenGLWidget::setAutoShowDevToolsContextMenu(bool b) {
  Q_ASSERT(pImpl_);
  pImpl_->setAutoShowDevToolsContextMenu(b);
}

bool QCefOpenGLWidget::autoShowDevToolsContextMenu() {
  Q_ASSERT(pImpl_);
  return pImpl_->browserSetting().autoShowDevToolsContextMenu;
}

void QCefOpenGLWidget::setAllowExecuteUnknownProtocolViaOS(bool b) {
  Q_ASSERT(pImpl_);
  pImpl_->setAllowExecuteUnknownProtocolViaOS(b);
}

bool QCefOpenGLWidget::allowExecuteUnknownProtocolViaOS() {
  Q_ASSERT(pImpl_);
  return pImpl_->browserSetting().executeUnknownProtocolViaOS;
}

void QCefOpenGLWidget::setAutoDestoryCefWhenCloseEvent(bool b) {
  Q_ASSERT(pImpl_);
  pImpl_->setAutoDestoryCefWhenCloseEvent(b);
}

bool QCefOpenGLWidget::autoDestoryCefWhenCloseEvent() {
  Q_ASSERT(pImpl_);
  return pImpl_->browserSetting().autoDestroyCefWhenCloseEvent;
}

void QCefOpenGLWidget::setFPS(int fps) {
  Q_ASSERT(pImpl_);
  pImpl_->setFPS(fps);
}

int QCefOpenGLWidget::fps() const {
  Q_ASSERT(pImpl_);
  return pImpl_->browserSetting().fps;
}

void QCefOpenGLWidget::setBrowserBackgroundColor(const QColor& color) {
  Q_ASSERT(pImpl_);
  pImpl_->setBrowserBackgroundColor(color);
}

QColor QCefOpenGLWidget::browserBackgroundColor() const {
  Q_ASSERT(pImpl_);
  return pImpl_->browserSetting().backgroundColor;
}

void QCefOpenGLWidget::showDevTools() {
  QCefManager::getInstance().showDevTools(this);
}

void QCefOpenGLWidget::closeDevTools() {
  QCefManager::getInstance().closeDevTools(this);
}

bool QCefOpenGLWidget::addResourceProvider(QCefResourceProvider* provider,
                                           const QString& identifier) {
  Q_ASSERT(pImpl_);
  return pImpl_->addResourceProvider(provider, identifier);
}

bool QCefOpenGLWidget::removeResourceProvider(const QString& identifier) {
  Q_ASSERT(pImpl_);
  return pImpl_->removeResourceProvider(identifier);
}

bool QCefOpenGLWidget::removeAllResourceProvider() {
  Q_ASSERT(pImpl_);
  return pImpl_->removeAllResourceProvider();
}

bool QCefOpenGLWidget::nativeEvent(const QByteArray& eventType,
                                   void* message,
                                   long* result) {
  // pImpl_ may be empty, if we call winId in QCefWidgetImpl::QCefWidgetImpl().
  if (!pImpl_)
    return QOpenGLWidget::nativeEvent(eventType, message, result);
  if (pImpl_->nativeEvent(eventType, message, result))
    return true;
  return QOpenGLWidget::nativeEvent(eventType, message, result);
}

bool QCefOpenGLWidget::event(QEvent* event) {
  // pImpl_ may be empty, if we call winId in QCefWidgetImpl::QCefWidgetImpl().
  if (!pImpl_)
    return QOpenGLWidget::event(event);

  if (pImpl_->event(event)) {
    return true;
  }
  return QOpenGLWidget::event(event);
}

void QCefOpenGLWidget::showEvent(QShowEvent* event) {
  if (pImpl_) {
    pImpl_->visibleChangedNotify(true);
  }
  QOpenGLWidget::showEvent(event);
}

void QCefOpenGLWidget::hideEvent(QHideEvent* event) {
  if (pImpl_) {
    pImpl_->visibleChangedNotify(false);
  }
  QOpenGLWidget::hideEvent(event);
}

void QCefOpenGLWidget::paintEvent(QPaintEvent* event) {
  Q_ASSERT(pImpl_);
  if (!pImpl_ || !pImpl_->openGLPaintEventHandle(event)) {
    QOpenGLWidget::paintEvent(event);
  }
}
#endif