#ifndef QCEFVIEWHANDLER_H_
#define QCEFVIEWHANDLER_H_
#pragma once
#include "QCefQueryHandler.h"
#include "Include/QCefQuery.h"
#include "Include/QCefVersion.h"
#include <QEvent>
#include <QPaintEvent>
#include <QTextStream>
#include <include/cef_client.h>
#include <include/wrapper/cef_message_router.h>
#include <include/wrapper/cef_resource_manager.h>
#include <list>
#include <map>
#include <mutex>
#include <set>
#include <string>

class QCefResourceProvider;
class QCefWidgetImpl;
class QCefBase;

typedef struct _CefRenderBuffer {
  int x;
  int y;
  int width;
  int height;

  std::unique_ptr<uint8_t[]> buffer;
  int bufferSize;

  _CefRenderBuffer() {
    x = 0;
    y = 0;
    width = 0;
    height = 0;

    buffer = nullptr;
    bufferSize = 0;
  }
} CefRenderBuffer;

class QCefBrowserHandler : public CefClient,
                           public CefContextMenuHandler,
                           public CefDisplayHandler,
                           public CefDragHandler,
                           public CefJSDialogHandler,
                           public CefKeyboardHandler,
                           public CefLifeSpanHandler,
                           public CefLoadHandler,
                           public CefRequestHandler,
#if CEF_VERSION_MAJOR == 76 || CEF_VERSION_MAJOR == 86 || CEF_VERSION_MAJOR == 87 || CEF_VERSION_MAJOR == 89
                           public CefResourceRequestHandler,
#endif
                           //public CefResourceHandler,
                           public CefRenderHandler {
 public:
  QCefBrowserHandler(QCefWidgetImpl* pImpl);
  ~QCefBrowserHandler();

#pragma region CefClient

  //////////////////////////////////////////////////////////////////////////
  // CefClient methods:
  virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override {
    return this;
  }
  virtual CefRefPtr<CefDialogHandler> GetDialogHandler() override {
    return nullptr;
  }
  virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override {
    return this;
  }
  virtual CefRefPtr<CefDownloadHandler> GetDownloadHandler() override {
    return nullptr;
  }
  virtual CefRefPtr<CefDragHandler> GetDragHandler() override {
    return this;
  }
  virtual CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() override {
    return this;
  }
  virtual CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() override {
    return this;
  }
  virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override {
    return this;
  }
  virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override {
    return this;
  }
  virtual CefRefPtr<CefRequestHandler> GetRequestHandler() override {
    return this;
  }
  virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override {
    return this;
  }
#if CEF_VERSION_MAJOR == 72
  virtual bool
  OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                           CefProcessId source_process,
                           CefRefPtr<CefProcessMessage> message) override;
#elif CEF_VERSION_MAJOR == 76 || CEF_VERSION_MAJOR == 86 || CEF_VERSION_MAJOR == 87 || CEF_VERSION_MAJOR == 89
  bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefProcessId source_process,
                                CefRefPtr<CefProcessMessage> message) override;
#endif

#pragma endregion CefClient

#pragma region CefContextMenuHandler

  // CefContextMenuHandler methods
  virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefContextMenuParams> params,
                                   CefRefPtr<CefMenuModel> model) override;
  virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefContextMenuParams> params,
                                    int command_id,
                                    EventFlags event_flags) override;

#pragma endregion CefContextMenuHandler

#pragma region CefDisplayHandler

  // CefDisplayHandler methods
  virtual void OnAddressChange(CefRefPtr<CefBrowser> browser,
                               CefRefPtr<CefFrame> frame,
                               const CefString& url) override;
  virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
                             const CefString& title) override;
  virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                                cef_log_severity_t level,
                                const CefString& message,
                                const CefString& source,
                                int line) override;
  void OnFaviconURLChange(CefRefPtr<CefBrowser> browser,
                          const std::vector<CefString>& icon_urls) override;
  void OnFullscreenModeChange(CefRefPtr<CefBrowser> browser,
                              bool fullscreen) override;

#pragma endregion CefDisplayHandler

#pragma region CefDragHandler

  // CefDragHandler methods
  virtual bool OnDragEnter(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefDragData> dragData,
                           CefDragHandler::DragOperationsMask mask) override;

#if CEF_VERSION_MAJOR == 72
  virtual void OnDraggableRegionsChanged(
      CefRefPtr<CefBrowser> browser,
      const std::vector<CefDraggableRegion>& regions) override;
#elif CEF_VERSION_MAJOR == 76 || CEF_VERSION_MAJOR == 86 || CEF_VERSION_MAJOR == 87 || CEF_VERSION_MAJOR == 89
  void OnDraggableRegionsChanged(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      const std::vector<CefDraggableRegion>& regions) override;
#endif

#pragma endregion CefDragHandler

#pragma region CefJSDialogHandler

  // CefJSDialogHandler methods
  virtual bool OnJSDialog(CefRefPtr<CefBrowser> browser,
                          const CefString& origin_url,
                          JSDialogType dialog_type,
                          const CefString& message_text,
                          const CefString& default_prompt_text,
                          CefRefPtr<CefJSDialogCallback> callback,
                          bool& suppress_message) override;

  virtual bool OnBeforeUnloadDialog(CefRefPtr<CefBrowser> browser,
                       const CefString& message_text,
                       bool is_reload,
                       CefRefPtr<CefJSDialogCallback> callback) override;
  virtual void OnResetDialogState(CefRefPtr<CefBrowser> browser) override;

#pragma endregion CefJSDialogHandler

#pragma region CefKeyboardHandler

  // CefKeyboardHandler methods
  virtual bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
                             const CefKeyEvent& event,
                             CefEventHandle os_event,
                             bool* is_keyboard_shortcut) override;

#pragma endregion CefKeyboardHandler

#pragma region CefLifeSpanHandler

  // CefLifeSpanHandler methods:
#if CEF_VERSION_MAJOR == 72
  virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
                CefRefPtr<CefFrame> frame,
                const CefString& target_url,
                const CefString& target_frame_name,
                CefLifeSpanHandler::WindowOpenDisposition target_disposition,
                bool user_gesture,
                const CefPopupFeatures& popupFeatures,
                CefWindowInfo& windowInfo,
                CefRefPtr<CefClient>& client,
                CefBrowserSettings& settings,
                bool* no_javascript_access) override;
#elif CEF_VERSION_MAJOR == 76 || CEF_VERSION_MAJOR == 86 || CEF_VERSION_MAJOR == 87 || CEF_VERSION_MAJOR == 89
  bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
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
                bool* no_javascript_access) override;
#endif

  virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
  virtual bool DoClose(CefRefPtr<CefBrowser> browser) override;
  virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

#pragma endregion CefLifeSpanHandler

#pragma region CefLoadHandler

  // CefLoadHandler methods
  virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                    bool isLoading,
                                    bool canGoBack,
                                    bool canGoForward) override;
  virtual void OnLoadStart(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           TransitionType transition_type) override;
  virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         int httpStatusCode) override;
  virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           ErrorCode errorCode,
                           const CefString& errorText,
                           const CefString& failedUrl) override;

#pragma endregion CefLoadHandler

#pragma region CefRequestHandler

  // CefRequestHandler methods
  virtual bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefFrame> frame,
                              CefRefPtr<CefRequest> request,
                              bool user_gesture,
                              bool is_redirect) override;

  virtual bool OnOpenURLFromTab(CefRefPtr<CefBrowser> browser,
                   CefRefPtr<CefFrame> frame,
                   const CefString& target_url,
                   CefRequestHandler::WindowOpenDisposition target_disposition,
                   bool user_gesture) override;

#if CEF_VERSION_MAJOR == 76 || CEF_VERSION_MAJOR == 86 || CEF_VERSION_MAJOR == 87 || CEF_VERSION_MAJOR == 89
  virtual CefRefPtr<CefResourceRequestHandler> GetResourceRequestHandler(CefRefPtr<CefBrowser> browser,
                            CefRefPtr<CefFrame> frame,
                            CefRefPtr<CefRequest> request,
                            bool is_navigation,
                            bool is_download,
                            const CefString& request_initiator,
                            bool& disable_default_handling) override;
#endif

  virtual bool OnQuotaRequest(CefRefPtr<CefBrowser> browser,
                              const CefString& origin_url,
                              int64 new_size,
                              CefRefPtr<CefRequestCallback> callback) override;

  virtual void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
                                         TerminationStatus status) override;

#pragma endregion CefRequestHandler

#pragma region CefRequestHandler
#if CEF_VERSION_MAJOR == 72
  virtual CefRequestHandler::ReturnValue OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
                       CefRefPtr<CefFrame> frame,
                       CefRefPtr<CefRequest> request,
                       CefRefPtr<CefRequestCallback> callback) override;

  virtual CefRefPtr<CefResourceHandler> GetResourceHandler(CefRefPtr<CefBrowser> browser,
                     CefRefPtr<CefFrame> frame,
                     CefRefPtr<CefRequest> request) override;

  virtual void OnProtocolExecution(CefRefPtr<CefBrowser> browser,
                                   const CefString& url,
                                   bool& allow_os_execution) override;
#elif CEF_VERSION_MAJOR == 76 || CEF_VERSION_MAJOR == 86 || CEF_VERSION_MAJOR == 87 || CEF_VERSION_MAJOR == 89
  virtual CefResourceRequestHandler::ReturnValue OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
                       CefRefPtr<CefFrame> frame,
                       CefRefPtr<CefRequest> request,
                       CefRefPtr<CefRequestCallback> callback) override;

  virtual CefRefPtr<CefResourceHandler> GetResourceHandler(CefRefPtr<CefBrowser> browser,
                     CefRefPtr<CefFrame> frame,
                     CefRefPtr<CefRequest> request) override;

  void OnProtocolExecution(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           CefRefPtr<CefRequest> request,
                           bool& allow_os_execution) override;
#endif
#pragma endregion CefRequestHandler

#pragma region CefRenderHandler
  bool GetRootScreenRect(CefRefPtr<CefBrowser> browser, CefRect& rect) OVERRIDE;
  bool GetScreenPoint(CefRefPtr<CefBrowser> browser,
                      int viewX,
                      int viewY,
                      int& screenX,
                      int& screenY) OVERRIDE;
  void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) OVERRIDE;
  bool GetScreenInfo(CefRefPtr<CefBrowser> browser,
                     CefScreenInfo& screen_info) OVERRIDE;
  void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) OVERRIDE;
  void OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) OVERRIDE;
  void OnPaint(CefRefPtr<CefBrowser> browser,
               CefRenderHandler::PaintElementType type,
               const CefRenderHandler::RectList& dirtyRects,
               const void* buffer,
               int width,
               int height) OVERRIDE;

#if CEF_VERSION_MAJOR == 72 || CEF_VERSION_MAJOR == 76 || CEF_VERSION_MAJOR == 86 || CEF_VERSION_MAJOR == 87
  void OnCursorChange(CefRefPtr<CefBrowser> browser,
                      CefCursorHandle cursor,
                      CursorType type,
                      const CefCursorInfo& custom_cursor_info) OVERRIDE;
#elif CEF_VERSION_MAJOR == 89
  bool OnCursorChange(CefRefPtr<CefBrowser> browser,
                      CefCursorHandle cursor,
                      cef_cursor_type_t type,
                      const CefCursorInfo& custom_cursor_info) OVERRIDE;
#endif
  void OnImeCompositionRangeChanged(
      CefRefPtr<CefBrowser> browser,
      const CefRange& selection_range,
      const CefRenderHandler::RectList& character_bounds) OVERRIDE;
#pragma endregion CefRenderHandler

  /////////////////////////////////////////////////////////////////////////

  CefRefPtr<CefBrowser> browser() const;

  bool triggerEvent(const CefRefPtr<CefProcessMessage> msg);

  bool responseQuery(const int64_t query, bool success, const CefString& response, int error);

  bool dispatchNotifyRequest(CefRefPtr<CefBrowser> browser,
                             CefProcessId source_process,
                             CefRefPtr<CefProcessMessage> message);

  CefRenderBuffer* lockViewBuffer();
  void unlockViewBuffer();

  CefRenderBuffer* lockPopupBuffer();
  void unlockPopupBuffer();

  bool isPopupShow();
  bool isOverPopupWidget(int x, int y) const;
  int getPopupXOffset() const;
  int getPopupYOffset() const;

  bool addResourceProvider(QCefResourceProvider* provider,
                           const QString& identifier);
  bool removeResourceProvider(const QString& identifier);
  bool removeAllResourceProvider();

 protected:
  CefRect getPopupRectInWebView(const CefRect& original_rect) const;

 private:
  QCefWidgetImpl* pImpl_;

  int browserCount_;

  bool isClosing_;
  mutable std::recursive_mutex mtx_;

  CefRefPtr<CefBrowser> pMainBrowser_;
  std::list<CefRefPtr<CefBrowser>> popupBrowserList_;
  CefRefPtr<CefResourceManager> pResourceManager_;
  CefRefPtr<CefMessageRouterBrowserSide> pMessageRouter_;
  CefRefPtr<QCefQueryHandler> pCefqueryHandler_;

  std::recursive_mutex viewRenderBufMtx_;
  std::recursive_mutex popupRenderBufMtx_;
  CefRenderBuffer viewRenderBuffer_;
  CefRenderBuffer popupRenderBuffer_;

  CefRect popupRect_;
  CefRect originalPopupRect_;
  int viewWidth_;
  int viewHeight_;
  bool isPaintingPopup_;
  QFile consoleLog_;
  QTextStream consoleLogSteam_;

  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(QCefBrowserHandler);
};
#endif
