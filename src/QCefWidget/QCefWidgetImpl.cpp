#include "QCefWidgetImpl.h"
#include "CefManager.h"
#include "QCefProtocol.h"
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <include/base/cef_logging.h>
#include "Include/QCefWidget.h"
#include "Include/QCefOpenGLWidget.h"
#include "CefGlobalSetting.h"

namespace {
int kDefaultFPS = 60;
}

QCefWidgetImpl::QCefWidgetImpl(WidgetType vt, QWidget *pWidget)
    : pWidget_(pWidget)
    , pTopWidget_(nullptr)
    , vt_(vt)
    , browserCreated_(false)
    , browserClosing_(false)
    , pQCefViewHandler_(nullptr)
    , background_color_(255, 255, 255, 255)
    , fps_(kDefaultFPS) {}

QCefWidgetImpl::~QCefWidgetImpl() {}

bool QCefWidgetImpl::createBrowser() {
  if (browserCreated_)
    return true;
  Q_ASSERT(pWidget_);
  CefWindowHandle hwnd = nullptr;
  if (pWidget_) {
    hwnd = (CefWindowHandle)pWidget_->winId();
  }
  Q_ASSERT(hwnd);
  if (!hwnd) {
    return false;
  }

#if (defined Q_OS_WIN32 || defined Q_OS_WIN64)
  if (CefGlobalSetting::hook_top_level_window_close_msg) {
    pTopWidget_ = CefManager::getInstance().hookTopLevelWidget(pWidget_);
  }
  else {
    pTopWidget_ = CefManager::getInstance().getTopWidget(pWidget_);
  }
  Q_ASSERT(pTopWidget_);
  RegisterTouchWindow(hwnd, 0);
#endif

  CefWindowInfo window_info;
  window_info.SetAsWindowless(hwnd);

  CefBrowserSettings browserSettings;
  browserSettings.plugins = STATE_DISABLED; // disable all plugins
  browserSettings.windowless_frame_rate = fps_;
  browserSettings.background_color =
      CefColorSetARGB(background_color_.alpha(), background_color_.red(), background_color_.green(),
                      background_color_.blue());

  CefManager::getInstance().initializeCef();

  pQCefViewHandler_ = new CefBrowserHandler(this);

  // This method can be called on any browser process thread and will not block.
  if (!CefBrowserHost::CreateBrowser(window_info, pQCefViewHandler_, "", browserSettings, nullptr,
                                     CefRequestContext::GetGlobalContext())) {
    return false;
  }

  browserCreated_ = true;

  return true;
}

void QCefWidgetImpl::browserCreatedNotify(CefRefPtr<CefBrowser> browser) {
#if (defined Q_OS_WIN32 || defined Q_OS_WIN64)
  if (pCefUIEventWin_)
    pCefUIEventWin_.reset();
  pCefUIEventWin_ = std::make_shared<UIEventHandlerWin>((HWND)pWidget_->winId(), browser);
  Q_ASSERT(pCefUIEventWin_);
  if (pCefUIEventWin_) {
    pCefUIEventWin_->setDeviceScaleFactor(deviceScaleFactor());
  }
#else
#error("No default implement")
#endif

  CefManager::getInstance().addBrowser(pTopWidget_, browser);

  if (cachedNavigateUrl_.length() > 0) {
    browser->GetMainFrame()->LoadURL(cachedNavigateUrl_);
    cachedNavigateUrl_.clear();
  }
}

void QCefWidgetImpl::browserDestoryedNotify(CefRefPtr<CefBrowser> browser) {
  browserCreated_ = false;
  pQCefViewHandler_ = nullptr;
  pCefUIEventWin_.reset();

  CefManager::getInstance().removeBrowser(pTopWidget_, browser);

  if (CefManager::getInstance().browserCount(pTopWidget_) == 0) {
    if (CefGlobalSetting::hook_top_level_window_close_msg) {
      if (pTopWidget_) {
        QMetaObject::invokeMethod(
            pTopWidget_, [this]() { pTopWidget_->close(); }, Qt::QueuedConnection);
      }
    }
  }
}

void QCefWidgetImpl::OnImeCompositionRangeChanged(
    CefRefPtr<CefBrowser> browser, const CefRange &selection_range,
    const CefRenderHandler::RectList &character_bounds) {
  if (pCefUIEventWin_) {
    pCefUIEventWin_->OnImeCompositionRangeChanged(browser, selection_range, character_bounds);
  }
}

void QCefWidgetImpl::navigateToUrl(const QString &url) {
  if (!browserCreated_) {
    if (!createBrowser()) {
      Q_ASSERT(false);
      return;
    }
  }

  CefString strUrl;
  strUrl.FromWString(url.toStdWString());
  Q_ASSERT(pQCefViewHandler_);
  if (pQCefViewHandler_ && pQCefViewHandler_->browser() &&
      pQCefViewHandler_->browser()->GetMainFrame()) {
    pQCefViewHandler_->browser()->GetMainFrame()->LoadURL(strUrl);
  }
  else {
    cachedNavigateUrl_ = strUrl;
  }
}

bool QCefWidgetImpl::browserCanGoBack() {
  if (pQCefViewHandler_ && pQCefViewHandler_->browser())
    return pQCefViewHandler_->browser()->CanGoBack();

  return false;
}

bool QCefWidgetImpl::browserCanGoForward() {
  if (pQCefViewHandler_ && pQCefViewHandler_->browser())
    return pQCefViewHandler_->browser()->CanGoForward();

  return false;
}

void QCefWidgetImpl::browserGoBack() {
  if (pQCefViewHandler_ && pQCefViewHandler_->browser())
    pQCefViewHandler_->browser()->GoBack();
}

void QCefWidgetImpl::browserGoForward() {
  if (pQCefViewHandler_ && pQCefViewHandler_->browser())
    pQCefViewHandler_->browser()->GoForward();
}

bool QCefWidgetImpl::browserIsLoading() {
  if (pQCefViewHandler_ && pQCefViewHandler_->browser())
    return pQCefViewHandler_->browser()->IsLoading();

  return false;
}

void QCefWidgetImpl::browserReload() {
  if (pQCefViewHandler_ && pQCefViewHandler_->browser())
    pQCefViewHandler_->browser()->Reload();
}

void QCefWidgetImpl::browserStopLoad() {
  if (pQCefViewHandler_ && pQCefViewHandler_->browser())
    pQCefViewHandler_->browser()->StopLoad();
}

bool QCefWidgetImpl::triggerEvent(const QString &name, const QCefEvent &event, int frameId) {
  if (!name.isEmpty()) {
    return sendEventNotifyMessage(frameId, name, event);
  }

  return false;
}

bool QCefWidgetImpl::responseQCefQuery(const QCefQuery &query) {
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

  if (pQCefViewHandler_ && pQCefViewHandler_->browser() &&
      pQCefViewHandler_->browser()->GetMainFrame()) {
    pQCefViewHandler_->browser()->GetMainFrame()->ExecuteJavaScript(
        strJavascript, pQCefViewHandler_->browser()->GetMainFrame()->GetURL(), 0);
  }
}

bool QCefWidgetImpl::sendEventNotifyMessage(int frameId, const QString &name,
                                                 const QCefEvent &event) {
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

bool QCefWidgetImpl::nativeEvent(const QByteArray &eventType, void *message, long *result) {
#if (defined Q_OS_WIN32 || defined Q_OS_WIN64)
  if (eventType == "windows_generic_MSG") {
    MSG *pMsg = (MSG *)message;
    if (!pMsg || !pWidget_ || !pCefUIEventWin_)
      return false;
    if (pMsg->hwnd != (HWND)pWidget_->winId())
      return false;

    if (pMsg->message == WM_SYSCHAR || pMsg->message == WM_SYSKEYDOWN ||
        pMsg->message == WM_SYSKEYUP || pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP ||
        pMsg->message == WM_CHAR) {
      if (pWidget_->isActiveWindow() && pWidget_->hasFocus()) {
        pCefUIEventWin_->OnKeyboardEvent(pMsg->message, pMsg->wParam, pMsg->lParam);
      }
    }
    else if (pMsg->message == WM_MOUSEMOVE || pMsg->message == WM_MOUSEWHEEL ||
             pMsg->message == WM_MOUSELEAVE || pMsg->message == WM_LBUTTONDOWN ||
             pMsg->message == WM_RBUTTONDOWN || pMsg->message == WM_MBUTTONDOWN ||
             pMsg->message == WM_LBUTTONUP || pMsg->message == WM_RBUTTONUP ||
             pMsg->message == WM_MBUTTONUP) {

      if (pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_RBUTTONDOWN ||
          pMsg->message == WM_MBUTTONDOWN) {
        pWidget_->setFocus();
      }
      else if (pMsg->message == WM_MOUSELEAVE) {
        qInfo() << "WM_MOUSELEAVE";
        // winsoft666:
        // WM_MOUSELEAVE message will be triggered when using third-party input method input,
        // at this time you can not release the focus, otherwise you can not enter.
        // pCefView_->clearFocus();
        //
        if (::GetCapture() == pMsg->hwnd)
          ReleaseCapture();

        // winsoft666:
        //::SetCursor(NULL);
        SetClassLongPtr((HWND)pWidget_->winId(), GCLP_HCURSOR, NULL);
      }

      if (pWidget_->isActiveWindow()) {
        pCefUIEventWin_->OnMouseEvent(pMsg->message, pMsg->wParam, pMsg->lParam);
      }
    }
    else if (pMsg->message == WM_SIZE) {
      // winsoft666:
      // Old cef version maybe has some bugs about resize with dpi scale.
      // https://bitbucket.org/chromiumembedded/cef/issues/2823/osr-on-a-monitor-at-125x-scale-onpaint
      // https://bitbucket.org/chromiumembedded/cef/issues/2733/viz-osr-might-be-causing-some-graphic
      // https://bitbucket.org/chromiumembedded/cef/issues/2833/osr-gpu-consume-cpu-and-may-not-draw
      pCefUIEventWin_->OnSize(pMsg->message, pMsg->wParam, pMsg->lParam);
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
    else if (pMsg->message == WM_IME_SETCONTEXT || pMsg->message == WM_IME_STARTCOMPOSITION ||
             pMsg->message == WM_IME_COMPOSITION || pMsg->message == WM_IME_ENDCOMPOSITION) {
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

void QCefWidgetImpl::paintEvent(QPaintEvent *event) {
  if (!pQCefViewHandler_ || browserClosing_)
    return;

  DrawImageParam *pDrawImageParam = pQCefViewHandler_->lockImage();
  if (pDrawImageParam) {
    QImage image(pDrawImageParam->imageArray.get(), pDrawImageParam->imageWidth,
                 pDrawImageParam->imageHeight, QImage::Format_ARGB32);

    Q_ASSERT(vt_ == WT_Widget);
    QPainter paint(pWidget_);

    paint.drawImage(pWidget_->rect(), image);
  }
  pQCefViewHandler_->releaseImage();
}

void QCefWidgetImpl::openGLPaintEvent(QPaintEvent *event) {
  if (!pQCefViewHandler_ || browserClosing_)
    return;

  // TODO
  //
  DrawImageParam *pDrawImageParam = pQCefViewHandler_->lockImage();
  if (pDrawImageParam) {
    QImage image(pDrawImageParam->imageArray.get(), pDrawImageParam->imageWidth,
                 pDrawImageParam->imageHeight, QImage::Format_ARGB32);

    Q_ASSERT(vt_ == WT_OpenGLWidget);
    QOpenGLWidget *pCefWidget = (QOpenGLWidget *)pWidget_;
    QPainter paint(pCefWidget);

    paint.drawImage(pCefWidget->rect(), image);
  }
  pQCefViewHandler_->releaseImage();
}

float QCefWidgetImpl::deviceScaleFactor() {
  Q_ASSERT(pWidget_);
  return pWidget_->devicePixelRatioF();
}

void QCefWidgetImpl::setFPS(int fps) { fps_ = fps; }

int QCefWidgetImpl::fps() const { return fps_; }

void QCefWidgetImpl::setBackgroundColor(const QColor &color) { background_color_ = color; }

QColor QCefWidgetImpl::backgroundColor() const { return background_color_; }

void QCefWidgetImpl::updateCefView() {
  if (pWidget_) {
    pWidget_->update();
  }
}

void QCefWidgetImpl::setBrowserClosing(bool b) { browserClosing_ = b; }
