#include "QCefWidgetImpl.h"
#include "QCefManager.h"
#include "QCefProtocol.h"
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <include/base/cef_logging.h>
#include "Include/QCefWidget.h"
#include "Include/QCefOpenGLWidget.h"
#include "QCefGlobalSetting.h"
#include "CefBrowserApp/QCefRequestContextHandler.h"

namespace {
int kDefaultFPS = 30;
}

QCefWidgetImpl::QCefWidgetImpl(WidgetType vt, QWidget *pWidget)
    : pWidget_(pWidget)
    , pTopWidget_(nullptr)
    , vt_(vt)
    , widgetWId_(0)
    , browserCreated_(false)
    , browserClosing_(false)
    , osrEnabled_(true)
    , pQCefViewHandler_(nullptr)
    , background_color_(255, 255, 255, 255)
    , fps_(kDefaultFPS) {
  QCefManager::getInstance().initializeCef();
}

QCefWidgetImpl::~QCefWidgetImpl() { 
  qInfo() << "QCefWidgetImpl::~QCefWidgetImpl, this: " << this; 
  QCefManager::getInstance().uninitializeCef();
}

bool QCefWidgetImpl::createBrowser(const QString &url) {
  if (browserCreated_)
    return true;
  Q_ASSERT(pWidget_);
  CefWindowHandle hwnd = nullptr;
  if (pWidget_) {
    Q_ASSERT(widgetWId_);
    hwnd = (CefWindowHandle)widgetWId_;
  }
  Q_ASSERT(hwnd);
  if (!hwnd) {
    return false;
  }

#if (defined Q_OS_WIN32 || defined Q_OS_WIN64)
  RegisterTouchWindow(hwnd, 0);
#endif

  CefWindowInfo window_info;
  CefBrowserSettings browserSettings;
  if (osrEnabled_) {
    window_info.SetAsWindowless(hwnd);

    browserSettings.plugins = STATE_DISABLED; // disable all plugins
    browserSettings.windowless_frame_rate = fps_;
    browserSettings.background_color = CefColorSetARGB(background_color_.alpha(), background_color_.red(), background_color_.green(), background_color_.blue());
  }
  else {
    float scale = pWidget_->devicePixelRatioF();
    QRect viewRect = pWidget_->rect();
    RECT rc;
    rc.left = 0;
    rc.top = 0;
    rc.right = rc.left + viewRect.width() * scale;
    rc.bottom = rc.top + viewRect.height() * scale;

    window_info.SetAsChild(hwnd, rc);

    browserSettings.plugins = STATE_DISABLED; // disable all plugins
    browserSettings.background_color = CefColorSetARGB(background_color_.alpha(), background_color_.red(), background_color_.green(), background_color_.blue());
  }

  pQCefViewHandler_ = new QCefBrowserHandler(this);

  CefRefPtr<CefRequestContext> requestContext = CefRequestContext::CreateContext(CefRequestContext::GetGlobalContext(), new RequestContextHandler);

  // This method can be called on any browser process thread and will not block.
  if (!CefBrowserHost::CreateBrowser(window_info, pQCefViewHandler_, url.toStdWString(), browserSettings, nullptr, requestContext)) {
    return false;
  }

  browserCreated_ = true;

  return true;
}

bool QCefWidgetImpl::createDevTools(CefRefPtr<CefBrowser> targetBrowser) {
  if (browserCreated_)
    return true;
  Q_ASSERT(pWidget_);
  CefWindowHandle hwnd = nullptr;
  if (pWidget_) {
    hwnd = (CefWindowHandle)widgetWId_;
  }
  Q_ASSERT(hwnd);
  if (!hwnd) {
    return false;
  }

#if (defined Q_OS_WIN32 || defined Q_OS_WIN64)
  RegisterTouchWindow(hwnd, 0);
#endif

  CefWindowInfo windowInfo;
  windowInfo.SetAsWindowless(hwnd);

  CefBrowserSettings browserSettings;
  browserSettings.windowless_frame_rate = fps_;
  browserSettings.background_color = CefColorSetARGB(background_color_.alpha(), background_color_.red(), background_color_.green(), background_color_.blue());

  QCefManager::getInstance().initializeCef();

  pQCefViewHandler_ = new QCefBrowserHandler(this);

  if (targetBrowser) {
    targetBrowser->GetHost()->ShowDevTools(windowInfo, pQCefViewHandler_, browserSettings, CefPoint());
  }

  return true;
}

void QCefWidgetImpl::browserCreatedNotify(CefRefPtr<CefBrowser> browser) {
#if (defined Q_OS_WIN32 || defined Q_OS_WIN64)
  if (pCefUIEventWin_)
    pCefUIEventWin_.reset();
  pCefUIEventWin_ = std::make_shared<QCefWidgetUIEventHandlerWin>((HWND)widgetWId_, browser);
  Q_ASSERT(pCefUIEventWin_);
  if (pCefUIEventWin_) {
    pCefUIEventWin_->setDeviceScaleFactor(deviceScaleFactor());
  }
#else
#error("No default implement")
#endif

  pTopWidget_ = QCefManager::getInstance().addBrowser(pWidget_, browser, osrEnabled_);
}

void QCefWidgetImpl::browserClosingNotify(CefRefPtr<CefBrowser> browser) {
  qInfo() << "QCefWidgetImpl::browserClosingNotify, this: " << this;
  browserCreated_ = false;
  Q_ASSERT(pCefUIEventWin_);
  if (pCefUIEventWin_)
    pCefUIEventWin_.reset();
  QCefManager::getInstance().setBrowserClosing(pWidget_);
}

void QCefWidgetImpl::browserDestoryedNotify(CefRefPtr<CefBrowser> browser) {
  qInfo() << "QCefWidgetImpl::browserDestoryedNotify, this: " << this;
  Q_ASSERT(!pCefUIEventWin_);

  QCefManager::getInstance().setBrowserClosed(pWidget_);

  if (QCefManager::getInstance().aliveBrowserCount(pTopWidget_) == 0) {
    if (pTopWidget_) {
      QMetaObject::invokeMethod(
          pTopWidget_, [this]() { pTopWidget_->close(); }, Qt::QueuedConnection);
    }
  }
}

void QCefWidgetImpl::OnImeCompositionRangeChanged(CefRefPtr<CefBrowser> browser, const CefRange &selection_range, const CefRenderHandler::RectList &character_bounds) {
  if (pCefUIEventWin_) {
    pCefUIEventWin_->OnImeCompositionRangeChanged(browser, selection_range, character_bounds);
  }
}

void QCefWidgetImpl::navigateToUrl(const QString &url) {
  if (!browserCreated_) {
    if (!createBrowser(url)) {
      Q_ASSERT(false);
    }
    return;
  }

  CefString strUrl;
  strUrl.FromWString(url.toStdWString());
  Q_ASSERT(pQCefViewHandler_);
  if (pQCefViewHandler_ && pQCefViewHandler_->browser() && pQCefViewHandler_->browser()->GetMainFrame()) {
    pQCefViewHandler_->browser()->GetMainFrame()->LoadURL(strUrl);
  }
}

bool QCefWidgetImpl::canGoBack() {
  if (pQCefViewHandler_ && pQCefViewHandler_->browser())
    return pQCefViewHandler_->browser()->CanGoBack();

  return false;
}

bool QCefWidgetImpl::canGoForward() {
  if (pQCefViewHandler_ && pQCefViewHandler_->browser())
    return pQCefViewHandler_->browser()->CanGoForward();

  return false;
}

void QCefWidgetImpl::goBack() {
  if (pQCefViewHandler_ && pQCefViewHandler_->browser())
    pQCefViewHandler_->browser()->GoBack();
}

void QCefWidgetImpl::goForward() {
  if (pQCefViewHandler_ && pQCefViewHandler_->browser())
    pQCefViewHandler_->browser()->GoForward();
}

bool QCefWidgetImpl::isLoadingBrowser() {
  if (pQCefViewHandler_ && pQCefViewHandler_->browser())
    return pQCefViewHandler_->browser()->IsLoading();

  return false;
}

void QCefWidgetImpl::reloadBrowser() {
  if (pQCefViewHandler_ && pQCefViewHandler_->browser())
    pQCefViewHandler_->browser()->Reload();
}

void QCefWidgetImpl::stopLoadBrowser() {
  if (pQCefViewHandler_ && pQCefViewHandler_->browser())
    pQCefViewHandler_->browser()->StopLoad();
}

bool QCefWidgetImpl::triggerEvent(const QString &name, const QCefEvent &event, int frameId) {
  if (!name.isEmpty()) {
    return sendEventNotifyMessage(frameId, name, event);
  }

  return false;
}

bool QCefWidgetImpl::responseCefQuery(const QCefQuery &query) {
  if (pQCefViewHandler_) {
    CefString res;
    res.FromString(query.response().toStdString());
    return pQCefViewHandler_->responseQuery(query.id(), query.result(), res, query.error());
  }
  return false;
}

void QCefWidgetImpl::executeJavascript(const QString &javascript) {
  CefString strJavascript;
  strJavascript.FromWString(javascript.toStdWString());

  if (pQCefViewHandler_ && pQCefViewHandler_->browser() && pQCefViewHandler_->browser()->GetMainFrame()) {
    pQCefViewHandler_->browser()->GetMainFrame()->ExecuteJavaScript(strJavascript, pQCefViewHandler_->browser()->GetMainFrame()->GetURL(), 0);
  }
}

bool QCefWidgetImpl::sendEventNotifyMessage(int frameId, const QString &name, const QCefEvent &event) {
  CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create(TRIGGEREVENT_NOTIFY_MESSAGE);
  CefRefPtr<CefListValue> arguments = msg->GetArgumentList();

  int idx = 0;
  CefString eventName = name.toStdWString();
  arguments->SetString(idx++, eventName);

  CefRefPtr<CefDictionaryValue> dict = CefDictionaryValue::Create();

  CefString cefStr;
  cefStr.FromWString(event.objectName().toStdWString());
  dict->SetString("name", cefStr);

  QList<QByteArray> keys = event.dynamicPropertyNames();
  for (QByteArray &key : keys) {
    QVariant value = event.property(key.data());
    if (value.type() == QMetaType::Bool)
      dict->SetBool(key.data(), value.toBool());
    else if (value.type() == QMetaType::Int || value.type() == QMetaType::UInt)
      dict->SetInt(key.data(), value.toInt());
    else if (value.type() == QMetaType::Double)
      dict->SetDouble(key.data(), value.toDouble());
    else if (value.type() == QMetaType::QString) {
      cefStr.FromWString(value.toString().toStdWString());
      dict->SetString(key.data(), cefStr);
    }
    else {
      Q_ASSERT(false);
    }
  }

  arguments->SetDictionary(idx++, dict);

  return pQCefViewHandler_->triggerEvent(frameId, msg);
}

QWidget *QCefWidgetImpl::getWidget() {
  Q_ASSERT(pWidget_);
  return pWidget_;
}

WidgetType QCefWidgetImpl::getWidgetType() { return vt_; }

bool QCefWidgetImpl::event(QEvent *event) {
  if (event->type() == QEvent::WinIdChange) {
    if(pWidget_)
      widgetWId_ = pWidget_->winId();
  }
  
  return false;
}

bool QCefWidgetImpl::nativeEvent(const QByteArray &eventType, void *message, long *result) {
#if (defined Q_OS_WIN32 || defined Q_OS_WIN64)
  if (eventType == "windows_generic_MSG") {
    MSG *pMsg = (MSG *)message;
    if (!pMsg || !pWidget_ || !pCefUIEventWin_)
      return false;

    if (pMsg->hwnd != (HWND)widgetWId_)
      return false;

    if (pMsg->message == WM_SYSCHAR || pMsg->message == WM_SYSKEYDOWN || pMsg->message == WM_SYSKEYUP || pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP ||
        pMsg->message == WM_CHAR) {
      if (pWidget_->isActiveWindow() && pWidget_->hasFocus()) {
        pCefUIEventWin_->OnKeyboardEvent(pMsg->message, pMsg->wParam, pMsg->lParam);
      }
    }
    else if (pMsg->message == WM_MOUSEMOVE || pMsg->message == WM_MOUSEWHEEL || pMsg->message == WM_MOUSELEAVE || pMsg->message == WM_LBUTTONDOWN ||
             pMsg->message == WM_RBUTTONDOWN || pMsg->message == WM_MBUTTONDOWN || pMsg->message == WM_LBUTTONUP || pMsg->message == WM_RBUTTONUP ||
             pMsg->message == WM_MBUTTONUP) {

      if (pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_RBUTTONDOWN || pMsg->message == WM_MBUTTONDOWN) {
        pWidget_->setFocus();
      }
      else if (pMsg->message == WM_MOUSELEAVE) {
        // winsoft666:
        // WM_MOUSELEAVE message will be triggered when using third-party input method input,
        // at this time you can not release the focus, otherwise you can not enter.
        // pCefView_->clearFocus();
        //
        if (::GetCapture() == pMsg->hwnd)
          ReleaseCapture();

        // winsoft666:
        //::SetCursor(NULL);
        SetClassLongPtr((HWND)widgetWId_, GCLP_HCURSOR, NULL);
      }

      if (pWidget_->isActiveWindow()) {
        pCefUIEventWin_->OnMouseEvent(pMsg->message, pMsg->wParam, pMsg->lParam);
      }
    }
    else if (pMsg->message == WM_SIZE) {
      if (osrEnabled_) {
        // winsoft666:
        // Old cef version maybe has some bugs about resize with dpi scale.
        // https://bitbucket.org/chromiumembedded/cef/issues/2823/osr-on-a-monitor-at-125x-scale-onpaint
        // https://bitbucket.org/chromiumembedded/cef/issues/2733/viz-osr-might-be-causing-some-graphic
        // https://bitbucket.org/chromiumembedded/cef/issues/2833/osr-gpu-consume-cpu-and-may-not-draw
        pCefUIEventWin_->OnSize(pMsg->message, pMsg->wParam, pMsg->lParam);
      }
      else {
        CefWindowHandle cefhwnd = NULL;
        CefRefPtr<CefBrowser> browser = this->browser();
        if (browser)
          cefhwnd = browser->GetHost()->GetWindowHandle();
        if (cefhwnd) {
          QRect rc = pWidget_->rect();
          float scale = pWidget_->devicePixelRatioF();
          SetWindowPos(cefhwnd, NULL, rc.left() * scale, rc.top() * scale, rc.width() * scale, rc.height() * scale, SWP_NOZORDER);
        }
      }
    }
    else if (pMsg->message == WM_TOUCH) {
      pCefUIEventWin_->OnTouchEvent(pMsg->message, pMsg->wParam, pMsg->lParam);
    }
    else if (pMsg->message == WM_SETFOCUS || pMsg->message == WM_KILLFOCUS) {
      pCefUIEventWin_->OnFocusEvent(pMsg->message, pMsg->wParam, pMsg->lParam);
    }
    else if (pMsg->message == WM_CAPTURECHANGED || pMsg->message == WM_CANCELMODE) {
      pCefUIEventWin_->OnCaptureLostEvent(pMsg->message, pMsg->wParam, pMsg->lParam);
    }
    else if (pMsg->message == WM_IME_SETCONTEXT || pMsg->message == WM_IME_STARTCOMPOSITION || pMsg->message == WM_IME_COMPOSITION || pMsg->message == WM_IME_ENDCOMPOSITION) {
      if (pWidget_->isActiveWindow() && pWidget_->hasFocus()) {
        pCefUIEventWin_->OnIMEEvent(pMsg->message, pMsg->wParam, pMsg->lParam);
        if (pMsg->message != WM_IME_ENDCOMPOSITION)
          return true;
      }
    }
  }
#elif __APPLE__
#if TARGET_IPHONE_SIMULATOR
#elif TARGET_OS_IPHONE
#elif TARGET_OS_MAC
#else
#error "Unknown Apple platform"
#endif
#elif __linux__
#elif __unix__
#elif defined(_POSIX_VERSION)
#else
#error "Unknown compiler"
#endif
  return false;
}

CefRefPtr<CefBrowserHost> QCefWidgetImpl::getCefBrowserHost() {
  if (!pQCefViewHandler_)
    return nullptr;

  CefRefPtr<CefBrowser> browser = pQCefViewHandler_->browser();
  if (!browser)
    return nullptr;

  return browser->GetHost();
}

bool QCefWidgetImpl::paintEventHandle(QPaintEvent *event) {
  if (!pQCefViewHandler_ || !pWidget_ || browserClosing_)
    return false;

  Q_ASSERT(vt_ == WT_Widget);
  float scale = deviceScaleFactor();
  QPainter paint(pWidget_);

  CefRenderBuffer *pRenderBuffer = pQCefViewHandler_->lockViewBuffer();
  if (pRenderBuffer) {
    QImage image(pRenderBuffer->buffer.get(), pRenderBuffer->width, pRenderBuffer->height, QImage::Format_ARGB32);

    QRect destRect(pRenderBuffer->x, pRenderBuffer->y, pRenderBuffer->width / scale, pRenderBuffer->height / scale);
    paint.drawImage(destRect, image);
  }
  pQCefViewHandler_->unlockViewBuffer();

  if (pQCefViewHandler_->isPopupShow()) {
    CefRenderBuffer *pPopupImageParam = pQCefViewHandler_->lockPopupBuffer();
    if (pPopupImageParam) {
      QImage image(pPopupImageParam->buffer.get(), pPopupImageParam->width, pPopupImageParam->height, QImage::Format_ARGB32);

      QRect destRect(pPopupImageParam->x, pPopupImageParam->y, pPopupImageParam->width / scale, pPopupImageParam->height / scale);
      paint.drawImage(destRect, image);
    }
    pQCefViewHandler_->unlockPopupBuffer();
  }

  return true;
}
#ifndef QT_NO_OPENGL
bool QCefWidgetImpl::openGLPaintEventHandle(QPaintEvent *event) {
  if (!pQCefViewHandler_ || browserClosing_)
    return false;

  if (!pWidget_)
    return false;

  Q_ASSERT(vt_ == WT_OpenGLWidget);
  QOpenGLWidget *pCefWidget = qobject_cast<QCefOpenGLWidget *>(pWidget_);
  if (!pCefWidget)
    return false;

  float scale = deviceScaleFactor();
  QPainter paint(pCefWidget);

  CefRenderBuffer *pDrawImageParam = pQCefViewHandler_->lockViewBuffer();
  if (pDrawImageParam) {
    QImage image(pDrawImageParam->buffer.get(), pDrawImageParam->width, pDrawImageParam->height, QImage::Format_ARGB32);

    QRect destRect(pDrawImageParam->x, pDrawImageParam->y, pDrawImageParam->width / scale, pDrawImageParam->height / scale);
    paint.drawImage(destRect, image);
  }
  pQCefViewHandler_->unlockViewBuffer();

  if (pQCefViewHandler_->isPopupShow()) {
    CefRenderBuffer *pPopupImageParam = pQCefViewHandler_->lockPopupBuffer();
    if (pPopupImageParam) {
      QImage image(pPopupImageParam->buffer.get(), pPopupImageParam->width, pPopupImageParam->height, QImage::Format_ARGB32);

      QRect destRect(pPopupImageParam->x, pPopupImageParam->y, pPopupImageParam->width / scale, pPopupImageParam->height / scale);
      paint.drawImage(destRect, image);
    }
    pQCefViewHandler_->unlockPopupBuffer();
  }

  return true;
}
#endif

void QCefWidgetImpl::setVisible(bool visible) {
  if (browserClosing_)
    return;
  CefRefPtr<CefBrowserHost> host = getCefBrowserHost();
  if (!host)
    return;
  if (visible) {
    host->WasHidden(false);
    host->SendFocusEvent(true);
  }
  else {
    host->SendFocusEvent(false);
    host->WasHidden(true);
  }
}

bool QCefWidgetImpl::setOsrEnabled(bool b) {
  if (browserCreated_)
    return false;
  osrEnabled_ = b;
  return true;
}

bool QCefWidgetImpl::osrEnabled() {
  return osrEnabled_;
}

float QCefWidgetImpl::deviceScaleFactor() {
  Q_ASSERT(pWidget_);
  return pWidget_->devicePixelRatioF();
}

void QCefWidgetImpl::setFPS(int fps) { fps_ = fps; }

int QCefWidgetImpl::fps() const { return fps_; }

void QCefWidgetImpl::setBrowserBackgroundColor(const QColor &color) { background_color_ = color; }

QColor QCefWidgetImpl::browserBackgroundColor() const { return background_color_; }

void QCefWidgetImpl::updateCefWidget() {
  if (pWidget_) {
    pWidget_->update();
  }
}

void QCefWidgetImpl::setBrowserClosing(bool b) { browserClosing_ = b; }

CefRefPtr<CefBrowser> QCefWidgetImpl::browser() {
  if (!pQCefViewHandler_) {
    return nullptr;
  }
  return pQCefViewHandler_->browser();
}