#include "v8_handler_impl.h"
#include "include/base/cef_bind.h"
#include "include/base/cef_weak_ptr.h"
#include "include/wrapper/cef_closure_task.h"
#include "tests/cefclient/browser/main_context_impl.h"
#include "root_window_qt.h"

namespace {
const char kJSNotifyFunc[] = "mrtNotify";
const char kDoneMessageName[] = "mrtNotifyMsg";
}  // namespace

namespace client {
class V8HandlerImpl : public CefV8Handler {
 public:
  explicit V8HandlerImpl(CefRefPtr<ClientAppRenderer::Delegate> delegate) :
      delegate_(delegate) {
  }

  bool Execute(const CefString& name,
               CefRefPtr<CefV8Value> object,
               const CefV8ValueList& arguments,
               CefRefPtr<CefV8Value>& retval,
               CefString& exception) override {
    const std::string& message_name = name;
    if (message_name == kJSNotifyFunc) {
      DCHECK(1U == arguments.size()) << "Only supports 1 string type parameter.";
      DCHECK(arguments[0]->IsString()) << "Only supports 1 string type parameter.";
      if (1U != arguments.size() || !arguments[0]->IsString())
        return false;

      const CefString& msg = arguments[0]->GetStringValue();
      CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();
      CefRefPtr<CefBrowser> browser = context->GetBrowser();
      CefRefPtr<CefFrame> frame = context->GetFrame();
      const int64 frame_id = frame->GetIdentifier();
      const bool is_main_frame = frame->IsMain();

      CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create(kDoneMessageName);
      CefRefPtr<CefListValue> args = message->GetArgumentList();
      args->SetInt(0, CefInt64GetLow(frame_id));
      args->SetInt(1, CefInt64GetHigh(frame_id));
      args->SetBool(2, is_main_frame);
      args->SetString(3, msg);
      bool send_ret = browser->SendProcessMessage(PID_BROWSER, message);
      DCHECK(send_ret) << "Send process message to PID_BROWSER failed.";

      return send_ret;
    }
    return false;
  }

 private:
  CefRefPtr<ClientAppRenderer::Delegate> delegate_;

  IMPLEMENT_REFCOUNTING(V8HandlerImpl);
};

namespace renderer {
void RegisterJSNotifyFunction(CefRefPtr<CefV8Context> context, CefRefPtr<ClientAppRenderer::Delegate> delegate) {
  // Register function handlers with the 'window' object.
  CefRefPtr<CefV8Value> window = context->GetGlobal();

  CefRefPtr<V8HandlerImpl> handler = new V8HandlerImpl(delegate);
  CefV8Value::PropertyAttribute attributes =
      static_cast<CefV8Value::PropertyAttribute>(
          V8_PROPERTY_ATTRIBUTE_READONLY | V8_PROPERTY_ATTRIBUTE_DONTENUM |
          V8_PROPERTY_ATTRIBUTE_DONTDELETE);

  CefRefPtr<CefV8Value> notify_func = CefV8Value::CreateFunction(kJSNotifyFunc, handler.get());
  window->SetValue(kJSNotifyFunc, notify_func, attributes);
}
}  // namespace renderer

void NotifyRootWindowQt(CefRefPtr<CefBrowser> browser, std::string message) {
  if (!CURRENTLY_ON_MAIN_THREAD()) {
    MAIN_POST_CLOSURE(base::Bind(&NotifyRootWindowQt, browser, message));
    return;
  }

  scoped_refptr<RootWindow> rootWindow =
      MainContext::Get()->GetRootWindowManager()->GetWindowForBrowser(browser->GetIdentifier());
  if (!rootWindow)
    return;  // mark as handled

  RootWindowQt* q = reinterpret_cast<RootWindowQt*>(rootWindow.get());
  if (q) {
    q->OnReceiveJsNotify(message);
  }
}

bool HandleProcessMessage(CefRefPtr<CefBrowser> browser, CefRefPtr<CefProcessMessage> message) {
  const std::string& message_name = message->GetName();
  if (message_name == kDoneMessageName) {
    CefRefPtr<CefListValue> args = message->GetArgumentList();
    DCHECK(4U == args->GetSize());
    DCHECK(VTYPE_INT == args->GetType(0));
    DCHECK(VTYPE_INT == args->GetType(1));
    DCHECK(VTYPE_BOOL == args->GetType(2));
    DCHECK(VTYPE_STRING == args->GetType(3));

    if (4U != args->GetSize() ||
        VTYPE_INT != args->GetType(0) ||
        VTYPE_INT != args->GetType(1) ||
        VTYPE_BOOL != args->GetType(2) ||
        VTYPE_STRING != args->GetType(3))
      return true;  // mark as handled

    const int64 frame_id = CefInt64Set(args->GetInt(0), args->GetInt(1));
    const bool is_main_frame = args->GetBool(2);
    CefRefPtr<CefFrame> frame;
    if (is_main_frame)
      frame = browser->GetMainFrame();
    else
      frame = browser->GetFrame(frame_id);
    DCHECK(frame.get());

    std::string str = args->GetString(3);

    NotifyRootWindowQt(browser, str);

    return true;
  }

  return false;
}

}  // namespace client
