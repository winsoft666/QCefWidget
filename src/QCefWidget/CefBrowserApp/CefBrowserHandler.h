#ifndef QCEFVIEWHANDLER_H_
#define QCEFVIEWHANDLER_H_
#pragma once
#include "CefQueryHandler.h"
#include "Include/QCefQuery.h"
#include <QEvent>
#include <QPaintEvent>
#include <include/cef_client.h>
#include <include/wrapper/cef_message_router.h>
#include <include/wrapper/cef_resource_manager.h>
#include <list>
#include <map>
#include <mutex>
#include <set>
#include <string>

class QCefWidgetImplement;
class QCefBase;

struct DrawImageParam {
  int imageWidth;
  int imageHeight;
  std::unique_ptr<uint8_t[]> imageArray;

  DrawImageParam() {
    imageWidth = 0;
    imageHeight = 0;
  }
};

class CefBrowserHandler : public CefClient,
                              public CefContextMenuHandler,
                              public CefDisplayHandler,
                              public CefDragHandler,
                              public CefJSDialogHandler,
                              public CefKeyboardHandler,
                              public CefLifeSpanHandler,
                              public CefLoadHandler,
                              public CefRequestHandler,
                              public CefResourceRequestHandler,
                              public CefRenderHandler {
public:
  enum {
    MAIN_FRAME = (int64_t)0,
    ALL_FRAMES = (int64_t)-1,
  };

  CefBrowserHandler(QCefWidgetImplement *pCefViewImpl);
  ~CefBrowserHandler();

#pragma region CefClient

  //////////////////////////////////////////////////////////////////////////
  // CefClient methods:
  virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override { return this; }
  virtual CefRefPtr<CefDialogHandler> GetDialogHandler() override { return nullptr; }
  virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override { return this; }
  virtual CefRefPtr<CefDownloadHandler> GetDownloadHandler() override { return nullptr; }
  // virtual CefRefPtr<CefDragHandler> GetDragHandler() override {
  //    return this;
  //}
  virtual CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() override { return this; }
  virtual CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() override { return this; }
  virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
  virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override { return this; }
  virtual CefRefPtr<CefRequestHandler> GetRequestHandler() override { return this; }
  virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override { return this; }

  virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                        CefProcessId source_process,
                                        CefRefPtr<CefProcessMessage> message) override;

#pragma endregion CefClient

#pragma region CefContextMenuHandler

  // CefContextMenuHandler methods
  virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefContextMenuParams> params,
                                   CefRefPtr<CefMenuModel> model) override;
  virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefContextMenuParams> params, int command_id,
                                    EventFlags event_flags) override;

#pragma endregion CefContextMenuHandler

#pragma region CefDisplayHandler

  // CefDisplayHandler methods
  virtual void OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                               const CefString &url) override;
  virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString &title) override;
  virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser, cef_log_severity_t level,
                                const CefString &message, const CefString &source,
                                int line) override;

#pragma endregion CefDisplayHandler

#pragma region CefDragHandler

  // CefDragHandler methods
  virtual bool OnDragEnter(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDragData> dragData,
                           CefDragHandler::DragOperationsMask mask) override;

#pragma endregion CefDragHandler

#pragma region CefJSDialogHandler

  // CefJSDialogHandler methods
  virtual bool OnJSDialog(CefRefPtr<CefBrowser> browser, const CefString &origin_url,
                          JSDialogType dialog_type, const CefString &message_text,
                          const CefString &default_prompt_text,
                          CefRefPtr<CefJSDialogCallback> callback, bool &suppress_message) override;

  virtual bool OnBeforeUnloadDialog(CefRefPtr<CefBrowser> browser, const CefString &message_text,
                                    bool is_reload,
                                    CefRefPtr<CefJSDialogCallback> callback) override;
  virtual void OnResetDialogState(CefRefPtr<CefBrowser> browser) override;

#pragma endregion CefJSDialogHandler

#pragma region CefKeyboardHandler

  // CefKeyboardHandler methods
  virtual bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent &event,
                             CefEventHandle os_event, bool *is_keyboard_shortcut) override;

#pragma endregion CefKeyboardHandler

#pragma region CefLifeSpanHandler

  // CefLifeSpanHandler methods:
  virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                             const CefString &target_url, const CefString &target_frame_name,
                             CefLifeSpanHandler::WindowOpenDisposition target_disposition,
                             bool user_gesture, const CefPopupFeatures &popupFeatures,
                             CefWindowInfo &windowInfo, CefRefPtr<CefClient> &client,
                             CefBrowserSettings &settings,
                             CefRefPtr<CefDictionaryValue> &extra_info,
                             bool *no_javascript_access) override;
  virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
  virtual bool DoClose(CefRefPtr<CefBrowser> browser) override;
  virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

#pragma endregion CefLifeSpanHandler

#pragma region CefLoadHandler

  // CefLoadHandler methods
  virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack,
                                    bool canGoForward) override;
  virtual void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                           TransitionType transition_type) override;
  virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                         int httpStatusCode) override;
  virtual void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                           ErrorCode errorCode, const CefString &errorText,
                           const CefString &failedUrl) override;

#pragma endregion CefLoadHandler

#pragma region CefRequestHandler

  // CefRequestHandler methods
  virtual bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                              CefRefPtr<CefRequest> request, bool user_gesture,
                              bool is_redirect) override;

  virtual bool OnOpenURLFromTab(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                const CefString &target_url,
                                CefRequestHandler::WindowOpenDisposition target_disposition,
                                bool user_gesture) override;

  virtual CefRefPtr<CefResourceRequestHandler>
  GetResourceRequestHandler(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                            CefRefPtr<CefRequest> request, bool is_navigation, bool is_download,
                            const CefString &request_initiator,
                            bool &disable_default_handling) override;

  virtual bool OnQuotaRequest(CefRefPtr<CefBrowser> browser, const CefString &origin_url,
                              int64 new_size, CefRefPtr<CefRequestCallback> callback) override;

  virtual void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
                                         TerminationStatus status) override;

#pragma endregion CefRequestHandler

#pragma region CefResourceRequestHandler

  virtual ReturnValue OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                           CefRefPtr<CefRequest> request,
                                           CefRefPtr<CefRequestCallback> callback) override;

  virtual CefRefPtr<CefResourceHandler> GetResourceHandler(CefRefPtr<CefBrowser> browser,
                                                           CefRefPtr<CefFrame> frame,
                                                           CefRefPtr<CefRequest> request) override;

  virtual void OnProtocolExecution(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefRequest> request,
                                   bool &allow_os_execution) override;

#pragma endregion CefResourceRequestHandler

#pragma region CefRenderHandler
  bool GetRootScreenRect(CefRefPtr<CefBrowser> browser, CefRect &rect) OVERRIDE;
  bool GetScreenPoint(CefRefPtr<CefBrowser> browser, int viewX, int viewY, int &screenX,
                      int &screenY) OVERRIDE;
  void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) OVERRIDE;
  bool GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo &screen_info) OVERRIDE;
  void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) OVERRIDE;
  void OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect &rect) OVERRIDE;
  void OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type,
               const CefRenderHandler::RectList &dirtyRects, const void *buffer, int width,
               int height) OVERRIDE;

  void OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, CursorType type,
                      const CefCursorInfo &custom_cursor_info) OVERRIDE;
  void OnImeCompositionRangeChanged(CefRefPtr<CefBrowser> browser, const CefRange &selection_range,
                                    const CefRenderHandler::RectList &character_bounds) OVERRIDE;
#pragma endregion CefRenderHandler

  /////////////////////////////////////////////////////////////////////////

  CefRefPtr<CefBrowser> browser();

  bool triggerEvent(const int64_t frame_id, const CefRefPtr<CefProcessMessage> msg);

  bool responseQuery(const int64_t query, bool success, const CefString &response, int error);

  bool dispatchNotifyRequest(CefRefPtr<CefBrowser> browser, CefProcessId source_process,
                             CefRefPtr<CefProcessMessage> message);

  DrawImageParam *lockImage();
  void releaseImage();

private:
  QCefWidgetImplement *pCefViewImpl_;

  int browserCount_;

  bool isClosing_;
  std::recursive_mutex mtx_;

  CefRefPtr<CefBrowser> pMainBrowser_;
  std::list<CefRefPtr<CefBrowser>> popupBrowserList_;
  CefRefPtr<CefResourceManager> pResourceManager_;
  CefRefPtr<CefMessageRouterBrowserSide> pMessageRouter_;
  CefRefPtr<CefQueryHandler> pCefqueryHandler_;

  std::recursive_mutex imageMtx_;
  DrawImageParam drawImageParam_;

  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(CefBrowserHandler);
};
#endif
