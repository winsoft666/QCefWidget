#include "CefWnd.h"
#include "QCefWidget.h"
#include "QCefOpenGLWidget.h"
#include <QtWidgets>

CefWnd::CefWnd(bool frameless, bool translucentWindowBackground, QWidget* parent /*= nullptr*/) :
    CefWndBase<QWidget>(frameless, translucentWindowBackground, parent),
    framelessWindow_(frameless),
    translucentWindowBackground_(translucentWindowBackground),
    usingGLWidget_(false),
    contextMenuEnabled_(false),
    autoAddDevToolsContextMenu_(false),
    allowExecuteUnknownProtocolViaOS_(false),
    usingHideInsteadClose_(false),
    forceClose_(false),
    osrEnabled_(false),
    maximumFps_(25),
    windowBkColor_(255, 255, 255, 255),
    browserBkColor_(255, 255, 255, 255),
    pCefWidget_(nullptr),
    pCefGLWidget_(nullptr) {
  setAttribute(Qt::WA_DeleteOnClose, true);
  this->setResizeable(true);
}

CefWnd::~CefWnd() {}

void CefWnd::setupUi() {
  this->setObjectName("CefWnd");
  this->setWindowTitle(
      QString("%1").arg(initUrl_.isEmpty() ? "Not Created" : initUrl_));
  this->setWindowIcon(QIcon(":/QCefWidgetTest/images/logo.svg"));

  QString windowQss = QString("background-color: rgba(%1,%2,%3,%4);")
                          .arg(windowBkColor_.red())
                          .arg(windowBkColor_.green())
                          .arg(windowBkColor_.blue())
                          .arg(windowBkColor_.alpha());
  this->setStyleSheet(windowQss);

  if (usingGLWidget_) {
    pCefGLWidget_ = new QCefOpenGLWidget(initUrl_);
    pCefGLWidget_->setObjectName("cefWidget");
    pCefGLWidget_->setOsrEnabled(osrEnabled_);
    pCefGLWidget_->setContextMenuEnabled(contextMenuEnabled_);
    pCefGLWidget_->setAutoShowDevToolsContextMenu(autoAddDevToolsContextMenu_);
    pCefGLWidget_->setBrowserBackgroundColor(browserBkColor_);
    pCefGLWidget_->setAllowExecuteUnknownProtocolViaOS(
        allowExecuteUnknownProtocolViaOS_);
    if (!translucentWindowBackground_)
      pCefGLWidget_->setStyleSheet(
          "image: url(:/QCefWidgetTest/images/logo_blue.svg);");
    pCefGLWidget_->setAutoDestoryCefWhenCloseEvent(!usingHideInsteadClose_);

    connect(pCefGLWidget_,
            &QCefOpenGLWidget::invokeMethodNotify,
            this,
            &CefWnd::onInvokeMethodNotify);
    connect(pCefGLWidget_,
            &QCefOpenGLWidget::cefQueryRequest,
            this,
            &CefWnd::onCefQueryRequest);
    connect(pCefGLWidget_,
            &QCefOpenGLWidget::popupWindow,
            this,
            &CefWnd::onPopupWindow);
  }
  else {
    pCefWidget_ = new QCefWidget(initUrl_);
    pCefWidget_->setObjectName("cefWidget");
    pCefWidget_->setOsrEnabled(osrEnabled_);
    pCefWidget_->setContextMenuEnabled(contextMenuEnabled_);
    pCefWidget_->setAutoShowDevToolsContextMenu(autoAddDevToolsContextMenu_);
    pCefWidget_->setBrowserBackgroundColor(browserBkColor_);
    pCefWidget_->setAllowExecuteUnknownProtocolViaOS(
        allowExecuteUnknownProtocolViaOS_);
    if (!translucentWindowBackground_)
      pCefWidget_->setStyleSheet(
          "image: url(:/QCefWidgetTest/images/logo_blue.svg);");
    pCefWidget_->setAutoDestoryCefWhenCloseEvent(!usingHideInsteadClose_);

    connect(pCefWidget_,
            &QCefWidget::invokeMethodNotify,
            this,
            &CefWnd::onInvokeMethodNotify);
    connect(pCefWidget_,
            &QCefWidget::cefQueryRequest,
            this,
            &CefWnd::onCefQueryRequest);
    connect(pCefWidget_,
            &QCefWidget::popupWindow,
            this,
            &CefWnd::onPopupWindow);
  }

  QHBoxLayout* hlMain = new QHBoxLayout();
  hlMain->setContentsMargins(2, 2, 2, 2);
  hlMain->setSpacing(0);
  if (usingGLWidget_)
    hlMain->addWidget(pCefGLWidget_);
  else
    hlMain->addWidget(pCefWidget_);

  this->setLayout(hlMain);

  this->resize(sizeHint());
}

bool CefWnd::usingGLWidget() {
  return usingGLWidget_;
}

void CefWnd::setUsingGLWidget(bool b) {
  usingGLWidget_ = b;
}

void CefWnd::setAllowExecuteUnknownProtocolViaOS(bool b) {
  allowExecuteUnknownProtocolViaOS_ = b;
}

bool CefWnd::allowExecuteUnknownProtocolViaOS() {
  return allowExecuteUnknownProtocolViaOS_;
}

bool CefWnd::framelessWindow() {
  return framelessWindow_;
}

bool CefWnd::translucentWindowBackground() {
  return translucentWindowBackground_;
}

bool CefWnd::osrEnabled() {
  return osrEnabled_;
}

void CefWnd::setOsrEnabled(bool b) {
  osrEnabled_ = b;
}

bool CefWnd::contextMenuEnabled() {
  return contextMenuEnabled_;
}

void CefWnd::setContextMenuEnabled(bool b) {
  contextMenuEnabled_ = b;
}

bool CefWnd::autoAddDevToolsContextMenu() {
  return autoAddDevToolsContextMenu_;
}

void CefWnd::setAutoAddDevToolsContextMenu(bool b) {
  autoAddDevToolsContextMenu_ = b;
}

bool CefWnd::usingHideInsteadClose() {
  return usingHideInsteadClose_;
}

void CefWnd::setUsingHideInsteadClose(bool b) {
  usingHideInsteadClose_ = b;
}

QSize CefWnd::initSize() {
  return initSize_;
}

void CefWnd::setInitSize(QSize s) {
  initSize_ = s;
}

QString CefWnd::initUrl() {
  return initUrl_;
}

void CefWnd::setInitUrl(QString u) {
  initUrl_ = u;
}

int CefWnd::maximumFps() {
  return maximumFps_;
}

void CefWnd::setMaximumFps(int f) {
  maximumFps_ = f;
}

QColor CefWnd::windowBkColor() {
  return windowBkColor_;
}

void CefWnd::setWindowBkColor(QColor c) {
  windowBkColor_ = c;
}

QColor CefWnd::browserBkColor() {
  return browserBkColor_;
}

void CefWnd::setBrowserBkColor(QColor c) {
  browserBkColor_ = c;
}

void CefWnd::forceClose() {
  if (pCefWidget_)
    pCefWidget_->setAutoDestoryCefWhenCloseEvent(true);
  else if (pCefGLWidget_)
    pCefGLWidget_->setAutoDestoryCefWhenCloseEvent(true);

  forceClose_ = true;
  close();
}

QSize CefWnd::sizeHint() const {
  return initSize_;
}

void CefWnd::showEvent(QShowEvent* event) {
  QWidget::showEvent(event);

  emit cefWndVisibleChange(true);
}

void CefWnd::hideEvent(QHideEvent* event) {
  QWidget::hideEvent(event);

  emit cefWndVisibleChange(false);
}

void CefWnd::closeEvent(QCloseEvent* event) {
  if (!usingHideInsteadClose_ || forceClose_) {
    event->accept();
  }
  else {
    event->ignore();
    this->hide();
  }
}

void CefWnd::onNavigateToUrl(QString url) {
  if (usingGLWidget_) {
    if (pCefGLWidget_)
      pCefGLWidget_->navigateToUrl(url);
  }
  else {
    if (pCefWidget_)
      pCefWidget_->navigateToUrl(url);
  }

  this->setWindowTitle(QString("%1").arg(url));
}

void CefWnd::onReload() {
  if (usingGLWidget_) {
    if (pCefGLWidget_)
      pCefGLWidget_->reloadBrowser(true);
  }
  else {
    if (pCefWidget_)
      pCefWidget_->reloadBrowser(true);
  }
}

void CefWnd::onShowDevTools() {
  if (usingGLWidget_) {
    if (pCefGLWidget_)
      pCefGLWidget_->showDevTools();
  }
  else {
    if (pCefWidget_)
      pCefWidget_->showDevTools();
  }
}

void CefWnd::onCloseDevTools() {
  if (usingGLWidget_) {
    if (pCefGLWidget_)
      pCefGLWidget_->closeDevTools();
  }
  else {
    if (pCefWidget_)
      pCefWidget_->closeDevTools();
  }
}

void CefWnd::onInvokeMethodNotify(int browserId,
                                  int frameId,
                                  const QString& method,
                                  const QVariantList& arguments) {
  QString str =
      QString("[InvokeMethodNotify] browserId: %1, frameId: %2, method: %3\r\n")
          .arg(browserId)
          .arg(frameId)
          .arg(method);
  QString strArgs;
  for (int i = 0; i < arguments.size(); i++) {
    strArgs += QString("[%1]: %2\r\n").arg(i).arg(arguments[i].toString());
  }
  str += strArgs;
  QMessageBox::information(this, "Invoked from JS", str, QMessageBox::Ok);
}

void CefWnd::onTriggerEvent() {
  unsigned int random_index = time(nullptr);
  QCefEvent event(QString("TestEvent%1").arg(random_index));
  event.setStringProperty("StrProp", QString("String_%1").arg(random_index));
  event.setIntProperty("IntProp", 1000 + random_index);
  event.setDoubleProperty("DoubleProp", 3.1415926f + (double)random_index);
  event.setBoolProperty("BoolProp", random_index % 2 == 0);

  if (usingGLWidget_) {
    pCefGLWidget_->triggerEvent("eventTest", event);
  }
  else {
    pCefWidget_->triggerEvent("eventTest", event);
  }
}

void CefWnd::onCefQueryRequest(const QCefQuery& query) {
  QString str = QString("[CefQueryRequest] id: %1, reqeust: %2\r\n")
                    .arg(query.id())
                    .arg(query.reqeust());
  qDebug().noquote() << str;

  QCefQuery rsp = query;
  rsp.setResponseResult(
      true, "This is a query response message from C++.", 123);

  if (usingGLWidget_)
    pCefGLWidget_->responseCefQuery(rsp);
  else
    pCefWidget_->responseCefQuery(rsp);
}

void CefWnd::onPopupWindow(const QString& url) {
  CefWnd* pPopupWnd = new CefWnd(false, false);
  pPopupWnd->setInitUrl(url);
  pPopupWnd->setupUi();
  pPopupWnd->resize(800, 600);
  pPopupWnd->show();
}
