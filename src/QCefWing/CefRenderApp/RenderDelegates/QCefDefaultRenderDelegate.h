#pragma once
#include <include/wrapper/cef_message_router.h>
#include <unordered_map>
#include "../QCefRenderApp.h"
#include "QCefClient.h"

namespace QCefDefaultRenderDelegate {

void CreateBrowserDelegate(QCefRenderApp::RenderDelegateSet& delegates);

class RenderDelegate : public QCefRenderApp::RenderDelegate {
  typedef std::unordered_map<int64, CefRefPtr<QCefClient>>
      FrameID2QCefClientMap;

 public:
  RenderDelegate();

  virtual void OnWebKitInitialized(CefRefPtr<QCefRenderApp> app);

  virtual void OnContextCreated(CefRefPtr<QCefRenderApp> app,
                                CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefV8Context> context);

  virtual void OnContextReleased(CefRefPtr<QCefRenderApp> app,
                                 CefRefPtr<CefBrowser> browser,
                                 CefRefPtr<CefFrame> frame,
                                 CefRefPtr<CefV8Context> context);

  virtual bool OnProcessMessageReceived(CefRefPtr<QCefRenderApp> app,
                                        CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefFrame> frame,
                                        CefProcessId source_process,
                                        CefRefPtr<CefProcessMessage> message);

 protected:
  bool OnTriggerEventNotifyMessage(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefProcessId source_process,
                                   CefRefPtr<CefProcessMessage> message);

  void ExecuteEventListener(CefRefPtr<CefBrowser> browser,
                            CefRefPtr<CefFrame> frame,
                            const CefString& name,
                            CefRefPtr<CefDictionaryValue> dict);

 private:
  CefRefPtr<CefMessageRouterRendererSide> render_message_router_;

  FrameID2QCefClientMap frame_id_to_client_map_;

 private:
  IMPLEMENT_REFCOUNTING(RenderDelegate);
};

}  // namespace QCefDefaultRenderDelegate
