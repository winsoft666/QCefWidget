#ifndef QCEFWIDGET_RENDERAPP_H_
#define QCEFWIDGET_RENDERAPP_H_
#pragma once

#include <set>
#include <include/cef_app.h>
#include "../../QCefWidget/Include/QCefVersion.h"

class QCefRenderApp : public CefApp, public CefRenderProcessHandler {
 public:
  QCefRenderApp();

  ~QCefRenderApp();

  class RenderDelegate : public virtual CefBaseRefCounted {
   public:
    virtual void OnRenderThreadCreated(CefRefPtr<QCefRenderApp> app,
                                       CefRefPtr<CefListValue> extra_info) {}

    virtual void OnWebKitInitialized(CefRefPtr<QCefRenderApp> app) {}

    virtual void OnBrowserCreated(CefRefPtr<QCefRenderApp> app, CefRefPtr<CefBrowser> browser) {}

    virtual void OnBrowserDestroyed(CefRefPtr<QCefRenderApp> app, CefRefPtr<CefBrowser> browser) {}

    virtual CefRefPtr<CefLoadHandler> GetLoadHandler(CefRefPtr<QCefRenderApp> app) {
      return nullptr;
    }

    virtual void OnContextCreated(CefRefPtr<QCefRenderApp> app,
                                  CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  CefRefPtr<CefV8Context> context) {}

    virtual void OnContextReleased(CefRefPtr<QCefRenderApp> app,
                                   CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefV8Context> context) {}

    virtual void OnUncaughtException(CefRefPtr<QCefRenderApp> app,
                                     CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     CefRefPtr<CefV8Context> context,
                                     CefRefPtr<CefV8Exception> exception,
                                     CefRefPtr<CefV8StackTrace> stackTrace) {}

    virtual void OnFocusedNodeChanged(CefRefPtr<QCefRenderApp> app,
                                      CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      CefRefPtr<CefDOMNode> node) {}

    // Called when a process message is received. Return true if the message was
    // handled and should not be passed on to other handlers. RenderDelegates
    // should check for unique message names to avoid interfering with each other.
    virtual bool OnProcessMessageReceived(CefRefPtr<QCefRenderApp> app,
                                          CefRefPtr<CefBrowser> browser,
                                          CefRefPtr<CefFrame> frame,
                                          CefProcessId source_process,
                                          CefRefPtr<CefProcessMessage> message) {
      return false;
    }
  };

  typedef std::set<CefRefPtr<RenderDelegate>> RenderDelegateSet;

 private:
  static void CreateRenderDelegates(RenderDelegateSet& delegates);

#pragma region CefApp

  //////////////////////////////////////////////////////////////////////////
  // CefApp methods:
  virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) override;

  virtual void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override;

  virtual CefRefPtr<CefResourceBundleHandler> GetResourceBundleHandler() override;

  virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override;

  virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;

#pragma endregion CefApp

#pragma region CefRenderProcessHandler

  // CefRenderProcessHandler methods:
#if CEF_VERSION_MAJOR == 72 || CEF_VERSION_MAJOR == 76
  virtual void OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) override;
#endif

  virtual void OnWebKitInitialized() override;

#if CEF_VERSION_MAJOR == 72
  virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser) override;
#elif CEF_VERSION_MAJOR == 76 || CEF_VERSION_MAJOR == 86 || CEF_VERSION_MAJOR == 87 || CEF_VERSION_MAJOR == 89
  virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDictionaryValue> extra_info) override;
#endif

  virtual void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) override;

  virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override;

  virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefV8Context> context) override;

  virtual void OnContextReleased(CefRefPtr<CefBrowser> browser,
                                 CefRefPtr<CefFrame> frame,
                                 CefRefPtr<CefV8Context> context) override;

  virtual void OnUncaughtException(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefV8Context> context,
                                   CefRefPtr<CefV8Exception> exception,
                                   CefRefPtr<CefV8StackTrace> stackTrace) override;

  virtual void OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefDOMNode> node) override;

#if CEF_VERSION_MAJOR == 72
  virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                        CefProcessId source_process,
                                        CefRefPtr<CefProcessMessage> message) override;
#elif CEF_VERSION_MAJOR == 76 || CEF_VERSION_MAJOR == 86 || CEF_VERSION_MAJOR == 87 || CEF_VERSION_MAJOR == 89
  virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefFrame> frame,
                                        CefProcessId source_process,
                                        CefRefPtr<CefProcessMessage> message) override;
#endif

#pragma endregion CefRenderProcessHandler

 private:
  // Set of supported RenderDelegates. Only used in the renderer process.
  RenderDelegateSet render_delegates_;

  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(QCefRenderApp);
};

#endif  // QCEFWIDGET_RENDERAPP_H_
