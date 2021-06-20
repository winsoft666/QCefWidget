#include "QCefBrowserHandler.h"
#include "QCefWidgetImpl.h"
#include <QCefProtocol.h>
#include <QDebug>
#include <QRect>
#include <QVariant>
#include <QWidget>
#include <include/cef_app.h>
#include <include/wrapper/cef_closure_task.h>
#include <include/wrapper/cef_helpers.h>
#include <include/base/cef_logging.h>
#include <sstream>
#include <string>
#include "QCefManager.h"
#include "Include/QCefWidget.h"
#include "Include/QCefOpenGLWidget.h"
#include "Include/QCefResourceProvider.h"
#include "QCefResourceUtil.h"

namespace {
enum InternalMenuId {
  CLIENT_ID_SHOW_DEVTOOLS = MENU_ID_USER_FIRST,
  CLIENT_ID_CLOSE_DEVTOOLS,
};
}

QCefBrowserHandler::QCefBrowserHandler(QCefWidgetImpl* pImpl) :
    isClosing_(false),
    pCefqueryHandler_(new QCefQueryHandler(pImpl)),
    pResourceManager_(new CefResourceManager()),
    pMessageRouter_(nullptr),
    browserCount_(0),
    viewWidth_(0),
    viewHeight_(0),
    isPaintingPopup_(false),
    pImpl_(pImpl) {
  QString consoleLogPath = pImpl_->browserSetting().consoleLogPath;
  if (!consoleLogPath.isEmpty()) {
    consoleLog_.setFileName(consoleLogPath);
    consoleLog_.open(QFile::Append);

    if (consoleLog_.isOpen()) {
      consoleLogSteam_.setDevice(&consoleLog_);
      consoleLogSteam_.setCodec("UTF-8");
    }
  }
  const std::string& internalOrigin = "http://qcefwidget/";
  pResourceManager_->AddProvider(
      CreateBinaryResourceProvider(internalOrigin, std::string()),
      100,
      std::string());
}

QCefBrowserHandler::~QCefBrowserHandler() {
  consoleLogSteam_.flush();
  consoleLogSteam_.setDevice(nullptr);
  if (consoleLog_.isOpen())
    consoleLog_.close();
}

#if CEF_VERSION_MAJOR == 72
bool QCefBrowserHandler::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message) {
  CEF_REQUIRE_UI_THREAD();
  if (pMessageRouter_ && pMessageRouter_->OnProcessMessageReceived(
                             browser, source_process, message))
    return true;

  if (dispatchNotifyRequest(browser, source_process, message))
    return true;

  return false;
}
#elif CEF_VERSION_MAJOR == 76 || CEF_VERSION_MAJOR == 86 || CEF_VERSION_MAJOR == 87 || CEF_VERSION_MAJOR == 89
bool QCefBrowserHandler::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message) {
  CEF_REQUIRE_UI_THREAD();
  if (pMessageRouter_ && pMessageRouter_->OnProcessMessageReceived(
                             browser, frame, source_process, message))
    return true;

  if (dispatchNotifyRequest(browser, source_process, message))
    return true;

  return false;
}
#endif

void QCefBrowserHandler::OnBeforeContextMenu(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefContextMenuParams> params,
    CefRefPtr<CefMenuModel> model) {
  CEF_REQUIRE_UI_THREAD();

  if (pImpl_) {
    if (!pImpl_->browserSetting().contextMenuEnabled)
      model->Clear();

    if (pImpl_->browserSetting().autoShowDevToolsContextMenu &&
        pImpl_->browserSetting().devToolsResourceExist) {
      if ((params->GetTypeFlags() & (CM_TYPEFLAG_PAGE | CM_TYPEFLAG_FRAME)) !=
          0) {
        // Add a separator if the menu already has items.
        if (model->GetCount() > 0)
          model->AddSeparator();

        // Add DevTools items to all context menus.
        model->AddItem(CLIENT_ID_SHOW_DEVTOOLS, "Show DevTools");
        model->AddItem(CLIENT_ID_CLOSE_DEVTOOLS, "Close DevTools");
      }
    }
  }
}

bool QCefBrowserHandler::OnContextMenuCommand(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefContextMenuParams> params,
    int command_id,
    EventFlags event_flags) {
  CEF_REQUIRE_UI_THREAD();

  switch (command_id) {
    case CLIENT_ID_SHOW_DEVTOOLS:
      QCefManager::getInstance().showDevTools(pImpl_->getWidget());
      return true;
    case CLIENT_ID_CLOSE_DEVTOOLS:
      QCefManager::getInstance().closeDevTools(pImpl_->getWidget());
      return true;
    default:
      return false;
  }
}

void QCefBrowserHandler::OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) {
  CEF_REQUIRE_UI_THREAD();

  if (pImpl_->getWidgetType() == WT_Widget) {
    QCefWidget* p = (QCefWidget*)pImpl_->getWidget();
    if (p)
      emit p->urlChanged(frame->IsMain(),
                         QString::fromStdWString(url.ToWString()));
  }
#ifndef QT_NO_OPENGL
  else if (pImpl_->getWidgetType() == WT_OpenGLWidget) {
    QCefOpenGLWidget* p = (QCefOpenGLWidget*)pImpl_->getWidget();
    if (p)
      emit p->urlChanged(frame->IsMain(),
                         QString::fromStdWString(url.ToWString()));
  }
#endif
}

void QCefBrowserHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) {
  CEF_REQUIRE_UI_THREAD();

  if (pImpl_->getWidgetType() == WT_Widget) {
    QCefWidget* p = (QCefWidget*)pImpl_->getWidget();
    if (p)
      emit p->titleChanged(QString::fromStdWString(title.ToWString()));
  }
#ifndef QT_NO_OPENGL
  else if (pImpl_->getWidgetType() == WT_OpenGLWidget) {
    QCefOpenGLWidget* p = (QCefOpenGLWidget*)pImpl_->getWidget();
    if (p)
      emit p->titleChanged(QString::fromStdWString(title.ToWString()));
  }
#endif
}

bool QCefBrowserHandler::OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                                          cef_log_severity_t level,
                                          const CefString& message,
                                          const CefString& source,
                                          int line) {
  CEF_REQUIRE_UI_THREAD();
  if (source.empty() || message.empty())
    return false;

  QString result = QString("[%1] [%2] %3\r\n")
                       .arg(source.ToWString())
                       .arg(line)
                       .arg(message.ToWString());
  if (consoleLog_.isOpen()) {
    consoleLogSteam_ << result;
  }
  else {
#if (defined Q_OS_WIN32 || defined Q_OS_WIN64)
    OutputDebugStringW(result.toStdWString().c_str());
#endif
  }

  return false;
}

void QCefBrowserHandler::OnFaviconURLChange( CefRefPtr<CefBrowser> browser, const std::vector<CefString>& icon_urls) {
  CEF_REQUIRE_UI_THREAD();

  if (!icon_urls.empty()) {
    // TODO
  }
}

void QCefBrowserHandler::OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool fullscreen) {
  CEF_REQUIRE_UI_THREAD();

  // TODO
}

bool QCefBrowserHandler::OnDragEnter(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDragData> dragData, CefDragHandler::DragOperationsMask mask) {
  CEF_REQUIRE_UI_THREAD();

  return false;
}

#if CEF_VERSION_MAJOR == 72
void QCefBrowserHandler::OnDraggableRegionsChanged(
    CefRefPtr<CefBrowser> browser,
    const std::vector<CefDraggableRegion>& regions) {
  CEF_REQUIRE_UI_THREAD();
  if (pImpl_) {
    pImpl_->draggableRegionsChangedNotify(browser, regions);
  }
}
#elif CEF_VERSION_MAJOR == 76 || CEF_VERSION_MAJOR == 86 || CEF_VERSION_MAJOR == 87 || CEF_VERSION_MAJOR == 89
void QCefBrowserHandler::OnDraggableRegionsChanged(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const std::vector<CefDraggableRegion>& regions) {
  CEF_REQUIRE_UI_THREAD();
  if (pImpl_) {
    pImpl_->draggableRegionsChangedNotify(browser, regions);
  }
}
#endif

bool QCefBrowserHandler::OnJSDialog(CefRefPtr<CefBrowser> browser,
                                    const CefString& origin_url,
                                    JSDialogType dialog_type,
                                    const CefString& message_text,
                                    const CefString& default_prompt_text,
                                    CefRefPtr<CefJSDialogCallback> callback,
                                    bool& suppress_message) {
  CEF_REQUIRE_UI_THREAD();

  return false;
}

bool QCefBrowserHandler::OnBeforeUnloadDialog(
    CefRefPtr<CefBrowser> browser,
    const CefString& message_text,
    bool is_reload,
    CefRefPtr<CefJSDialogCallback> callback) {
  CEF_REQUIRE_UI_THREAD();

  return false;
}

void QCefBrowserHandler::OnResetDialogState(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();
}

bool QCefBrowserHandler::OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
                                       const CefKeyEvent& event,
                                       CefEventHandle os_event,
                                       bool* is_keyboard_shortcut) {
  CEF_REQUIRE_UI_THREAD();

  return false;
}

#if CEF_VERSION_MAJOR == 72
bool QCefBrowserHandler::OnBeforePopup(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& target_url,
    const CefString& target_frame_name,
    CefLifeSpanHandler::WindowOpenDisposition target_disposition,
    bool user_gesture,
    const CefPopupFeatures& popupFeatures,
    CefWindowInfo& windowInfo,
    CefRefPtr<CefClient>& client,
    CefBrowserSettings& settings,
    bool* no_javascript_access) {
  CEF_REQUIRE_UI_THREAD();

  if (isClosing_)
    return true;

  if (pImpl_->getWidgetType() == WT_Widget) {
    QCefWidget* p = qobject_cast<QCefWidget*>(pImpl_->getWidget());
    if (p)
      emit p->popupWindow(QString::fromStdWString(target_url.ToWString()));
  }
#ifndef QT_NO_OPENGL
  else if (pImpl_->getWidgetType() == WT_OpenGLWidget) {
    QCefOpenGLWidget* p = qobject_cast<QCefOpenGLWidget*>(pImpl_->getWidget());
    if (p)
      emit p->popupWindow(QString::fromStdWString(target_url.ToWString()));
  }
#endif
  return true;
}
#elif CEF_VERSION_MAJOR == 76 || CEF_VERSION_MAJOR == 86 || CEF_VERSION_MAJOR == 87 || CEF_VERSION_MAJOR == 89
bool QCefBrowserHandler::OnBeforePopup(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& target_url,
    const CefString& target_frame_name,
    CefLifeSpanHandler::WindowOpenDisposition target_disposition,
    bool user_gesture,
    const CefPopupFeatures& popupFeatures,
    CefWindowInfo& windowInfo,
    CefRefPtr<CefClient>& client,
    CefBrowserSettings& settings,
    CefRefPtr<CefDictionaryValue>& extra_info,
    bool* no_javascript_access) {
  CEF_REQUIRE_UI_THREAD();

  if (isClosing_)
    return true;

  if (pImpl_->getWidgetType() == WT_Widget) {
    QCefWidget* p = qobject_cast<QCefWidget*>(pImpl_->getWidget());
    if (p)
      emit p->popupWindow(QString::fromStdWString(target_url.ToWString()));
  }
#ifndef QT_NO_OPENGL
  else if (pImpl_->getWidgetType() == WT_OpenGLWidget) {
    QCefOpenGLWidget* p = qobject_cast<QCefOpenGLWidget*>(pImpl_->getWidget());
    if (p)
      emit p->popupWindow(QString::fromStdWString(target_url.ToWString()));
  }
#endif
  return true;
}
#endif

void QCefBrowserHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();
  if (isClosing_)
    return;

  if (!pMessageRouter_) {
    // Create the browser-side router for query handling.
    CefMessageRouterConfig config;
    config.js_query_function = QCEF_QUERY_NAME;
    config.js_cancel_function = QCEF_QUERY_CANCEL_NAME;

    // Register handlers with the router.
    pMessageRouter_ = CefMessageRouterBrowserSide::Create(config);
    pMessageRouter_->AddHandler(pCefqueryHandler_.get(), false);
  }

  if (!pMainBrowser_) {
    do {
      std::lock_guard<std::recursive_mutex> lock(mtx_);
      pMainBrowser_ = browser;
    } while (false);

    pImpl_->browserCreatedNotify(browser);
  }
  else if (browser->IsPopup()) {
    popupBrowserList_.push_back(browser);
    CefPostTask(TID_UI,
                CefCreateClosureTask(base::Bind(
                    &CefBrowserHost::SetFocus, browser->GetHost().get(), true)));
  }

  do {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    browserCount_++;
  } while (false);
}

bool QCefBrowserHandler::DoClose(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();

  if (pMainBrowser_ && pMainBrowser_->IsSame(browser)) {
    isClosing_ = true;

    if (pImpl_) {
      pImpl_->browserClosingNotify(browser);
    }
  }

  return false;
}

void QCefBrowserHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();

  if (pMessageRouter_)
    pMessageRouter_->OnBeforeClose(browser);

  if (pMainBrowser_ && pMainBrowser_->IsSame(browser)) {
    // if the main browser is closing, we need to close all the pop up browsers.
    if (!popupBrowserList_.empty()) {
      for (auto& popup : popupBrowserList_) {
        if (popup) {
          popup->StopLoad();
          popup->GetHost()->CloseBrowser(true);
        }
      }
    }

    pMainBrowser_->StopLoad();
    pMainBrowser_ = nullptr;
  }
  else if (browser->IsPopup()) {
    // Remove from the browser popup list.
    for (auto it = popupBrowserList_.begin(); it != popupBrowserList_.end();
         ++it) {
      if ((*it)->IsSame(browser)) {
        popupBrowserList_.erase(it);
        break;
      }
    }
  }

  // Decrease the browser count
  do {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    --browserCount_;

    if (browserCount_ == 0) {
      if (pMessageRouter_) {
        pMessageRouter_->RemoveHandler(pCefqueryHandler_.get());
        pCefqueryHandler_ = nullptr;
        pMessageRouter_ = nullptr;
      }

      Q_ASSERT(pImpl_);
      if (pImpl_) {
        pImpl_->browserDestoryedNotify(browser);
      }
    }
  } while (false);
}

void QCefBrowserHandler::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                              bool isLoading,
                                              bool canGoBack,
                                              bool canGoForward) {
  CEF_REQUIRE_UI_THREAD();

  if (pImpl_->getWidgetType() == WT_Widget) {
    QCefWidget* p = qobject_cast<QCefWidget*>(pImpl_->getWidget());
    if (p)
      emit p->loadingStateChanged(isLoading, canGoBack, canGoForward);
  }
#ifndef QT_NO_OPENGL
  else if (pImpl_->getWidgetType() == WT_OpenGLWidget) {
    QCefOpenGLWidget* p = qobject_cast<QCefOpenGLWidget*>(pImpl_->getWidget());
    if (p)
      emit p->loadingStateChanged(isLoading, canGoBack, canGoForward);
  }
#endif
}

void QCefBrowserHandler::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transition_type) {
  CEF_REQUIRE_UI_THREAD();

  if (pImpl_->getWidgetType() == WT_Widget) {
    QCefWidget* p = qobject_cast<QCefWidget*>(pImpl_->getWidget());
    if (p)
      emit p->loadStart(frame->IsMain());
  }
#ifndef QT_NO_OPENGL
  else if (pImpl_->getWidgetType() == WT_OpenGLWidget) {
    QCefOpenGLWidget* p = qobject_cast<QCefOpenGLWidget*>(pImpl_->getWidget());
    if (p)
      emit p->loadStart(frame->IsMain());
  }
#endif
}

void QCefBrowserHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) {
  CEF_REQUIRE_UI_THREAD();

  if (pImpl_) {
    if (pImpl_->getWidgetType() == WT_Widget) {
      QCefWidget* p = qobject_cast<QCefWidget*>(pImpl_->getWidget());
      if (p)
        emit p->loadEnd(frame->IsMain(), httpStatusCode);
    }
#ifndef QT_NO_OPENGL
    else if (pImpl_->getWidgetType() == WT_OpenGLWidget) {
      QCefOpenGLWidget* p =
          qobject_cast<QCefOpenGLWidget*>(pImpl_->getWidget());
      if (p)
        emit p->loadEnd(frame->IsMain(), httpStatusCode);
    }
#endif
    if (frame->IsMain()) {
      pImpl_->mainFrameLoadFinishedNotify();
    }
  }
}

void QCefBrowserHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     ErrorCode errorCode,
                                     const CefString& errorText,
                                     const CefString& failedUrl) {
  CEF_REQUIRE_UI_THREAD();
  if (errorCode == ERR_ABORTED)
    return;

  // Don't display an error for external protocols that we allow the OS to
  // handle. See OnProtocolExecution().
  if (errorCode == ERR_UNKNOWN_URL_SCHEME) {
    return;
  }

  QString msg = QString::fromStdString(errorText.ToString());
  QString url = QString::fromStdString(failedUrl.ToString());

  if (pImpl_->getWidgetType() == WT_Widget) {
    QCefWidget* p = qobject_cast<QCefWidget*>(pImpl_->getWidget());
    if (p)
      emit p->loadError(frame->IsMain(), errorCode, msg, url);
  }
#ifndef QT_NO_OPENGL
  else if (pImpl_->getWidgetType() == WT_OpenGLWidget) {
    QCefOpenGLWidget* p = qobject_cast<QCefOpenGLWidget*>(pImpl_->getWidget());
    if (p)
      emit p->loadError(frame->IsMain(), errorCode, msg, url);
  }
#endif
}

bool QCefBrowserHandler::OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefFrame> frame,
                                        CefRefPtr<CefRequest> request,
                                        bool user_gesture,
                                        bool is_redirect) {
  CEF_REQUIRE_UI_THREAD();

  // TODO:

  if (pMessageRouter_)
    pMessageRouter_->OnBeforeBrowse(browser, frame);
  return false;
}

bool QCefBrowserHandler::OnOpenURLFromTab(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& target_url,
    CefRequestHandler::WindowOpenDisposition target_disposition,
    bool user_gesture) {
  CEF_REQUIRE_UI_THREAD();

  return false;  // return true to cancel this navigation.
}

#if CEF_VERSION_MAJOR == 76 || CEF_VERSION_MAJOR == 86 || CEF_VERSION_MAJOR == 87 || CEF_VERSION_MAJOR == 89
CefRefPtr<CefResourceRequestHandler> QCefBrowserHandler::GetResourceRequestHandler(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    bool is_navigation,
    bool is_download,
    const CefString& request_initiator,
    bool& disable_default_handling) {
  return this;
}
#endif

bool QCefBrowserHandler::OnQuotaRequest(
    CefRefPtr<CefBrowser> browser,
    const CefString& origin_url,
    int64 new_size,
    CefRefPtr<CefRequestCallback> callback) {
  CEF_REQUIRE_IO_THREAD();
  static const int maxSize = 10 * 1024 * 1024;
  callback->Continue(new_size <= maxSize);
  return true;
}

void QCefBrowserHandler::OnRenderProcessTerminated(
    CefRefPtr<CefBrowser> browser,
    TerminationStatus status) {
  CEF_REQUIRE_UI_THREAD();

  if (pMessageRouter_)
    pMessageRouter_->OnRenderProcessTerminated(browser);

  if (pMainBrowser_ && pMainBrowser_->GetMainFrame()) {
    CefString url = pMainBrowser_->GetMainFrame()->GetURL();
    if (!url.empty()) {
      pMainBrowser_->GetMainFrame()->LoadURL(url);
    }
  }

  if (pImpl_->getWidgetType() == WT_Widget) {
    QCefWidget* p = qobject_cast<QCefWidget*>(pImpl_->getWidget());
    if (p)
      emit p->renderProcessTerminated((int)status);
  }
#ifndef QT_NO_OPENGL
  else if (pImpl_->getWidgetType() == WT_OpenGLWidget) {
    QCefOpenGLWidget* p = qobject_cast<QCefOpenGLWidget*>(pImpl_->getWidget());
    if (p)
      emit p->renderProcessTerminated((int)status);
  }
#endif
}

#if CEF_VERSION_MAJOR == 72
CefRequestHandler::ReturnValue QCefBrowserHandler::OnBeforeResourceLoad(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    CefRefPtr<CefRequestCallback> callback) {
  return pResourceManager_->OnBeforeResourceLoad(
      browser, frame, request, callback);
}

CefRefPtr<CefResourceHandler>
QCefBrowserHandler::GetResourceHandler(CefRefPtr<CefBrowser> browser,
                                       CefRefPtr<CefFrame> frame,
                                       CefRefPtr<CefRequest> request) {
  return pResourceManager_->GetResourceHandler(browser, frame, request);
}

void QCefBrowserHandler::OnProtocolExecution(CefRefPtr<CefBrowser> browser,
                                             const CefString& url,
                                             bool& allow_os_execution) {
  if (pImpl_) {
    allow_os_execution = pImpl_->browserSetting().executeUnknownProtocolViaOS;
  }
}

#elif CEF_VERSION_MAJOR == 76 || CEF_VERSION_MAJOR == 86 || CEF_VERSION_MAJOR == 87 || CEF_VERSION_MAJOR == 89
CefResourceRequestHandler::ReturnValue QCefBrowserHandler::OnBeforeResourceLoad(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    CefRefPtr<CefRequestCallback> callback) {
  return pResourceManager_->OnBeforeResourceLoad(
      browser, frame, request, callback);
}

CefRefPtr<CefResourceHandler>
QCefBrowserHandler::GetResourceHandler(CefRefPtr<CefBrowser> browser,
                                       CefRefPtr<CefFrame> frame,
                                       CefRefPtr<CefRequest> request) {
  return pResourceManager_->GetResourceHandler(browser, frame, request);
}

void QCefBrowserHandler::OnProtocolExecution(CefRefPtr<CefBrowser> browser,
                                             CefRefPtr<CefFrame> frame,
                                             CefRefPtr<CefRequest> request,
                                             bool& allow_os_execution) {
  if (pImpl_) {
    allow_os_execution = pImpl_->browserSetting().executeUnknownProtocolViaOS;
  }
}

#endif

bool QCefBrowserHandler::GetRootScreenRect(CefRefPtr<CefBrowser> browser,
                                           CefRect& rect) {
  CEF_REQUIRE_UI_THREAD();
  return false;
}

bool QCefBrowserHandler::GetScreenPoint(CefRefPtr<CefBrowser> browser,
                                        int viewX,
                                        int viewY,
                                        int& screenX,
                                        int& screenY) {
  CEF_REQUIRE_UI_THREAD();
  QWidget* pWidget = pImpl_->getWidget();

  DCHECK(pWidget);
  if (!pWidget)
    return false;

  float fScale = pImpl_->deviceScaleFactor();

  QPoint p = {viewX, viewY};
  QPoint globalPos = pWidget->mapToGlobal(p);

  screenX = fScale * (float)globalPos.x();
  screenY = fScale * (float)globalPos.y();

  // qDebug().noquote() << viewX << "," << viewY <<  " -> " << screenX << ", " << screenY;
  return true;
}

void QCefBrowserHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) {
  CEF_REQUIRE_UI_THREAD();
  QWidget* pWidget = pImpl_->getWidget();

  DCHECK(pWidget);
  if (!pWidget)
    return;

  QRect viewRect = pWidget->rect();
  rect.x = 0;
  rect.y = 0;
  rect.width = viewRect.width();
  rect.height = viewRect.height();

  if (rect.width == 0)
    rect.width = 1;
  if (rect.height == 0)
    rect.height = 1;
}

bool QCefBrowserHandler::GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info) {
  CEF_REQUIRE_UI_THREAD();
  DCHECK(pImpl_);
  if (!pImpl_)
    return false;

  CefRect view_rect;
  GetViewRect(browser, view_rect);

  screen_info.device_scale_factor = pImpl_->deviceScaleFactor();
  screen_info.rect = view_rect;
  screen_info.available_rect = view_rect;
  return true;
}

void QCefBrowserHandler::OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) {
  CEF_REQUIRE_UI_THREAD();
  if (!show) {
    popupRect_.Set(0, 0, 0, 0);
    originalPopupRect_.Set(0, 0, 0, 0);
    browser->GetHost()->Invalidate(PET_VIEW);
  }
}

void QCefBrowserHandler::OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) {
  CEF_REQUIRE_UI_THREAD();
  if (rect.width <= 0 || rect.height <= 0)
    return;
  originalPopupRect_ = rect;
  popupRect_ = getPopupRectInWebView(originalPopupRect_);
}

void QCefBrowserHandler::OnPaint(CefRefPtr<CefBrowser> browser,
                                 CefRenderHandler::PaintElementType type,
                                 const CefRenderHandler::RectList& dirtyRects,
                                 const void* buffer,
                                 int width,
                                 int height) {
  CEF_REQUIRE_UI_THREAD();

  if (type == PET_VIEW) {
    viewWidth_ = width;
    viewHeight_ = height;

    do {
      std::lock_guard<std::recursive_mutex> lg(viewRenderBufMtx_);
      size_t newSize = width * height * 4;

      if (newSize != viewRenderBuffer_.bufferSize) {
        viewRenderBuffer_.buffer.reset(new uint8_t[newSize]);
        if (viewRenderBuffer_.buffer.get())
          viewRenderBuffer_.bufferSize = newSize;
      }

      Q_ASSERT(viewRenderBuffer_.buffer.get());
      if (viewRenderBuffer_.buffer.get()) {
        memcpy(viewRenderBuffer_.buffer.get(), buffer, newSize);
      }

      viewRenderBuffer_.x = 0;
      viewRenderBuffer_.y = 0;
      viewRenderBuffer_.width = width;
      viewRenderBuffer_.height = height;
    } while (false);

    if (!isPaintingPopup_) {
      if (!popupRect_.IsEmpty()) {
        isPaintingPopup_ = true;
        browser->GetHost()->Invalidate(PET_POPUP);
        isPaintingPopup_ = false;
      }
    }
  }
  else if (type == PET_POPUP && popupRect_.width > 0 && popupRect_.height > 0) {
    int skip_pixels = 0, x = popupRect_.x;
    int skip_rows = 0, y = popupRect_.y;
    int w = width;
    int h = height;

    // Adjust the popup to fit inside the view.
    if (x < 0) {
      skip_pixels = -x;
      x = 0;
    }
    if (y < 0) {
      skip_rows = -y;
      y = 0;
    }
    if (x + w > viewWidth_)
      w -= x + w - viewWidth_;
    if (y + h > viewHeight_)
      h -= y + h - viewHeight_;

    do {
      std::lock_guard<std::recursive_mutex> lg(popupRenderBufMtx_);
      size_t newSize = width * height * 4;

      if (newSize != popupRenderBuffer_.bufferSize) {
        popupRenderBuffer_.buffer.reset(new uint8_t[newSize]);
        if (popupRenderBuffer_.buffer.get())
          popupRenderBuffer_.bufferSize = newSize;
      }

      Q_ASSERT(popupRenderBuffer_.buffer.get());
      if (popupRenderBuffer_.buffer.get()) {
        memcpy(popupRenderBuffer_.buffer.get(), buffer, newSize);
      }

      popupRenderBuffer_.x = x;
      popupRenderBuffer_.y = y;
      popupRenderBuffer_.width = w;
      popupRenderBuffer_.height = h;
    } while (false);
  }

  float dpiScale = pImpl_->deviceScaleFactor();

  QRect updateRegion = {0, 0, 0, 0};
  for (auto& dr : dirtyRects) {
    updateRegion = updateRegion.united(QRect((float)dr.x / dpiScale,
                                             (float)dr.y / dpiScale,
                                             (float)dr.width / dpiScale,
                                             (float)dr.height / dpiScale));
  }

  if (pImpl_)
    pImpl_->updateCefWidget(updateRegion);
}

#if CEF_VERSION_MAJOR == 72 || CEF_VERSION_MAJOR == 86 || CEF_VERSION_MAJOR == 76 || CEF_VERSION_MAJOR == 87
void QCefBrowserHandler::OnCursorChange(
    CefRefPtr<CefBrowser> browser,
    CefCursorHandle cursor,
    CursorType type,
    const CefCursorInfo& custom_cursor_info) {
  CEF_REQUIRE_UI_THREAD();
  if (pImpl_ && !pImpl_->isOsrEnabled()) {
    return;
  }

  QWidget* pWidget = pImpl_->getWidget();
  if (pWidget) {
    SetClassLongPtr((HWND)pWidget->winId(),
                    GCLP_HCURSOR,
                    static_cast<LONG>(reinterpret_cast<LONG_PTR>(cursor)));
    ::SetCursor(cursor);
  }
}
#elif CEF_VERSION_MAJOR == 89
bool QCefBrowserHandler::OnCursorChange(
    CefRefPtr<CefBrowser> browser,
    CefCursorHandle cursor,
    cef_cursor_type_t type,
    const CefCursorInfo& custom_cursor_info) {
  CEF_REQUIRE_UI_THREAD();
  if (pImpl_ && !pImpl_->isOsrEnabled()) {
    return false;
  }
  QWidget* pWidget = pImpl_->getWidget();
  if (pWidget) {
    SetClassLongPtr((HWND)pWidget->winId(),
                    GCLP_HCURSOR,
                    static_cast<LONG>(reinterpret_cast<LONG_PTR>(cursor)));
    ::SetCursor(cursor);
    return true;
  }
  return false;
}
#endif

void QCefBrowserHandler::OnImeCompositionRangeChanged(
    CefRefPtr<CefBrowser> browser,
    const CefRange& selection_range,
    const CefRenderHandler::RectList& character_bounds) {
  if (pImpl_)
    pImpl_->imeCompositionRangeChangedNotify(
        browser, selection_range, character_bounds);
}

CefRefPtr<CefBrowser> QCefBrowserHandler::browser() const {
  std::lock_guard<std::recursive_mutex> lock(mtx_);
  return pMainBrowser_;
}

bool QCefBrowserHandler::triggerEvent(const CefRefPtr<CefProcessMessage> msg) {
  if (msg->GetName().empty())
    return false;

#if CEF_VERSION_MAJOR == 72
  CefRefPtr<CefBrowser> pBrowser = browser();
  if (pBrowser) {
    pBrowser->SendProcessMessage(PID_RENDERER, msg);
    return true;
  }
#elif CEF_VERSION_MAJOR == 76 || CEF_VERSION_MAJOR == 86 || CEF_VERSION_MAJOR == 87 || CEF_VERSION_MAJOR == 89
  CefRefPtr<CefBrowser> pBrowser = browser();
  if (pBrowser && pBrowser->GetMainFrame()) {
    pBrowser->GetMainFrame()->SendProcessMessage(PID_RENDERER, msg);
    return true;
  }
#endif

  return false;
}

bool QCefBrowserHandler::responseQuery(const int64_t query,
                                       bool success,
                                       const CefString& response,
                                       int error) {
  if (pCefqueryHandler_)
    return pCefqueryHandler_->Response(query, success, response, error);

  return false;
}

bool QCefBrowserHandler::dispatchNotifyRequest(
    CefRefPtr<CefBrowser> browser,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message) {
  if (message->GetName() == INVOKEMETHOD_NOTIFY_MESSAGE) {
    CefRefPtr<CefListValue> messageArguments = message->GetArgumentList();
    if (messageArguments && (messageArguments->GetSize() >= 2)) {
      size_t idx = 0;
      if (CefValueType::VTYPE_INT == messageArguments->GetType(idx)) {
        int browserId = browser->GetIdentifier();
        int frameId = messageArguments->GetInt(idx++);

        if (CefValueType::VTYPE_STRING == messageArguments->GetType(idx)) {
          CefString functionName = messageArguments->GetString(idx++);
          if (functionName == QCEF_INVOKEMETHOD) {
            QString method;
            if (CefValueType::VTYPE_STRING == messageArguments->GetType(idx)) {
#if defined(CEF_STRING_TYPE_UTF16)
              method = QString::fromWCharArray(
                  messageArguments->GetString(idx++).c_str());
#elif defined(CEF_STRING_TYPE_UTF8)
              method =
                  QString::fromUtf8(messageArguments->GetString(idx++).c_str());
#elif defined(CEF_STRING_TYPE_WIDE)
              method = QString::fromWCharArray(
                  messageArguments->GetString(idx++).c_str());
#endif
            }

            QVariantList arguments;
            QString qStr;
            for (; idx < messageArguments->GetSize(); idx++) {
              if (CefValueType::VTYPE_NULL == messageArguments->GetType(idx))
                arguments.push_back(0);
              else if (CefValueType::VTYPE_BOOL ==
                       messageArguments->GetType(idx))
                arguments.push_back(
                    QVariant::fromValue(messageArguments->GetBool(idx)));
              else if (CefValueType::VTYPE_INT ==
                       messageArguments->GetType(idx))
                arguments.push_back(
                    QVariant::fromValue(messageArguments->GetInt(idx)));
              else if (CefValueType::VTYPE_DOUBLE ==
                       messageArguments->GetType(idx))
                arguments.push_back(
                    QVariant::fromValue(messageArguments->GetDouble(idx)));
              else if (CefValueType::VTYPE_STRING ==
                       messageArguments->GetType(idx)) {
#if defined(CEF_STRING_TYPE_UTF16)
                qStr = QString::fromWCharArray(
                    messageArguments->GetString(idx).c_str());
#elif defined(CEF_STRING_TYPE_UTF8)
                qStr =
                    QString::fromUtf8(messageArguments->GetString(idx).c_str());
#elif defined(CEF_STRING_TYPE_WIDE)
                qStr = QString::fromWCharArray(
                    messageArguments->GetString(idx).c_str());
#endif
                arguments.push_back(qStr);
              }
              else {
                DCHECK(false);
              }
            }

            if (pImpl_->getWidgetType() == WT_Widget) {
              QCefWidget* p = (QCefWidget*)pImpl_->getWidget();
              if (p)
                emit p->invokeMethodNotify(
                    browserId, frameId, method, arguments);
            }
#ifndef QT_NO_OPENGL
            else if (pImpl_->getWidgetType() == WT_OpenGLWidget) {
              QCefOpenGLWidget* p = (QCefOpenGLWidget*)pImpl_->getWidget();
              if (p)
                emit p->invokeMethodNotify(
                    browserId, frameId, method, arguments);
            }
#endif
            return true;
          }
        }
      }
    }
  }

  return false;
}

CefRenderBuffer* QCefBrowserHandler::lockViewBuffer() {
  viewRenderBufMtx_.lock();
  return &viewRenderBuffer_;
}

void QCefBrowserHandler::unlockViewBuffer() {
  viewRenderBufMtx_.unlock();
}

CefRenderBuffer* QCefBrowserHandler::lockPopupBuffer() {
  popupRenderBufMtx_.lock();
  return &popupRenderBuffer_;
}

void QCefBrowserHandler::unlockPopupBuffer() {
  popupRenderBufMtx_.unlock();
}

bool QCefBrowserHandler::isPopupShow() {
  return !popupRect_.IsEmpty();
}

bool QCefBrowserHandler::isOverPopupWidget(int x, int y) const {
  const CefRect& rc = popupRect_;
  int popup_right = rc.x + rc.width;
  int popup_bottom = rc.y + rc.height;
  return (x >= rc.x) && (x < popup_right) && (y >= rc.y) && (y < popup_bottom);
}

int QCefBrowserHandler::getPopupXOffset() const {
  return originalPopupRect_.x - popupRect_.x;
}

int QCefBrowserHandler::getPopupYOffset() const {
  return originalPopupRect_.y - popupRect_.y;
}

bool QCefBrowserHandler::addResourceProvider(QCefResourceProvider* provider, const QString& identifier) {
  if (!pResourceManager_)
    return false;
  CustomResourceProvider* pCustomProvider =
      new CustomResourceProvider(provider);
  pResourceManager_->AddProvider(
      pCustomProvider, 101, identifier.toStdString());
  return true;
}

bool QCefBrowserHandler::removeResourceProvider(const QString& identifier) {
  if (pResourceManager_) {
    pResourceManager_->RemoveProviders(identifier.toStdString());
    return true;
  }
  return false;
}

bool QCefBrowserHandler::removeAllResourceProvider() {
  if (pResourceManager_) {
    pResourceManager_->RemoveAllProviders();
    return true;
  }
  return false;
}

CefRect QCefBrowserHandler::getPopupRectInWebView(const CefRect& original_rect) const {
  CefRect rc(original_rect);
  // if x or y are negative, move them to 0.
  if (rc.x < 0)
    rc.x = 0;
  if (rc.y < 0)
    rc.y = 0;
  // if popup goes outside the view, try to reposition origin
  if (rc.x + rc.width > viewWidth_)
    rc.x = viewWidth_ - rc.width;
  if (rc.y + rc.height > viewHeight_)
    rc.y = viewHeight_ - rc.height;
  // if x or y became negative, move them to 0 again.
  if (rc.x < 0)
    rc.x = 0;
  if (rc.y < 0)
    rc.y = 0;
  return rc;
}
