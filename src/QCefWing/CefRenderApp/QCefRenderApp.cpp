#include <string>
#include <include/wrapper/cef_helpers.h>
#include <include/cef_browser.h>
#include <include/cef_command_line.h>
#include "QCefRenderApp.h"
#include "RenderDelegates/QCefDefaultRenderDelegate.h"

QCefRenderApp::QCefRenderApp() {}

QCefRenderApp::~QCefRenderApp() {}

void QCefRenderApp::CreateRenderDelegates(RenderDelegateSet& delegates) {
  QCefDefaultRenderDelegate::CreateBrowserDelegate(delegates);
}

//////////////////////////////////////////////////////////////////////////
void QCefRenderApp::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) {}

void QCefRenderApp::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) {}

CefRefPtr<CefRenderProcessHandler> QCefRenderApp::GetRenderProcessHandler() {
  return this;
}

CefRefPtr<CefResourceBundleHandler> QCefRenderApp::GetResourceBundleHandler() {
  return nullptr;
}

CefRefPtr<CefBrowserProcessHandler> QCefRenderApp::GetBrowserProcessHandler() {
  return nullptr;
}

//////////////////////////////////////////////////////////////////////////
#if CEF_VERSION_MAJOR == 72 || CEF_VERSION_MAJOR == 76
void QCefRenderApp::OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) {
  CEF_REQUIRE_RENDERER_THREAD();

  CreateRenderDelegates(render_delegates_);

  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end(); ++it)
    (*it)->OnRenderThreadCreated(this, extra_info);
}
#endif

void QCefRenderApp::OnWebKitInitialized() {
  CEF_REQUIRE_RENDERER_THREAD();

#if CEF_VERSION_MAJOR == 86 || CEF_VERSION_MAJOR == 87 || CEF_VERSION_MAJOR == 89
  CreateRenderDelegates(render_delegates_);
#endif

  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end(); ++it)
    (*it)->OnWebKitInitialized(this);
}

#if CEF_VERSION_MAJOR == 72
void QCefRenderApp::OnBrowserCreated(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_RENDERER_THREAD();
  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end(); ++it)
    (*it)->OnBrowserCreated(this, browser);
}
#elif CEF_VERSION_MAJOR == 76 || CEF_VERSION_MAJOR == 86 || CEF_VERSION_MAJOR == 87 || CEF_VERSION_MAJOR == 89
void QCefRenderApp::OnBrowserCreated(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefDictionaryValue> extra_info) {
  CEF_REQUIRE_RENDERER_THREAD();
  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end(); ++it)
    (*it)->OnBrowserCreated(this, browser);
}
#endif

void QCefRenderApp::OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_RENDERER_THREAD();
  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end(); ++it)
    (*it)->OnBrowserDestroyed(this, browser);
}

CefRefPtr<CefLoadHandler> QCefRenderApp::GetLoadHandler() {
  CefRefPtr<CefLoadHandler> load_handler;
  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end() && !load_handler.get(); ++it)
    load_handler = (*it)->GetLoadHandler(this);

  return load_handler;
}

void QCefRenderApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     CefRefPtr<CefV8Context> context) {
  CEF_REQUIRE_RENDERER_THREAD();

  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end(); ++it)
    (*it)->OnContextCreated(this, browser, frame, context);
}

void QCefRenderApp::OnContextReleased(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      CefRefPtr<CefV8Context> context) {
  CEF_REQUIRE_RENDERER_THREAD();
  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end(); ++it)
    (*it)->OnContextReleased(this, browser, frame, context);
}

void QCefRenderApp::OnUncaughtException(CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefFrame> frame,
                                        CefRefPtr<CefV8Context> context,
                                        CefRefPtr<CefV8Exception> exception,
                                        CefRefPtr<CefV8StackTrace> stackTrace) {
  CEF_REQUIRE_RENDERER_THREAD();
  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end(); ++it)
    (*it)->OnUncaughtException(
        this, browser, frame, context, exception, stackTrace);
}

void QCefRenderApp::OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
                                         CefRefPtr<CefFrame> frame,
                                         CefRefPtr<CefDOMNode> node) {
  CEF_REQUIRE_RENDERER_THREAD();
  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end(); ++it)
    (*it)->OnFocusedNodeChanged(this, browser, frame, node);
}

#if CEF_VERSION_MAJOR == 72
bool QCefRenderApp::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message) {
  CEF_REQUIRE_RENDERER_THREAD();
  DCHECK_EQ(source_process, PID_BROWSER);

  bool handled = false;

  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end() && !handled; ++it)
    handled = (*it)->OnProcessMessageReceived(
        this, browser, browser->GetMainFrame(), source_process, message);

  return handled;
}
#elif CEF_VERSION_MAJOR == 76 || CEF_VERSION_MAJOR == 86 || CEF_VERSION_MAJOR == 87 || CEF_VERSION_MAJOR == 89
bool QCefRenderApp::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message) {
  CEF_REQUIRE_RENDERER_THREAD();
  DCHECK_EQ(source_process, PID_BROWSER);

  bool handled = false;

  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end() && !handled; ++it)
    handled = (*it)->OnProcessMessageReceived(
        this, browser, frame, source_process, message);

  return handled;
}
#endif
