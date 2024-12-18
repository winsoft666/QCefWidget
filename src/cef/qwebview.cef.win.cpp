#include "QWebView.CEF.h"
#include <include/base/cef_logging.h>
#include <include/cef_app.h>
#include <include/cef_browser.h>
#include <include/cef_frame.h>
#include <include/cef_sandbox_win.h>
#include "include/base/cef_scoped_ptr.h"
#include "include/cef_command_line.h"
#include "tests/cefclient/browser/main_context_impl.h"
#include "tests/cefclient/browser/main_message_loop_multithreaded_win.h"
#include "tests/cefclient/browser/root_window_manager.h"
#include "tests/cefclient/browser/test_runner.h"
#include "tests/shared/browser/client_app_browser.h"
#include "tests/shared/browser/main_message_loop_external_pump.h"
#include "tests/shared/browser/main_message_loop_std.h"
#include "tests/shared/common/client_app_other.h"
#include "tests/shared/common/client_switches.h"
#include "tests/shared/renderer/client_app_renderer.h"
#include "main_message_loop_qt.h"
#include <QDebug>

using namespace client;

// When generating projects with CMake the CEF_USE_SANDBOX value will be defined
// automatically if using the required compiler version. Pass -DUSE_SANDBOX=OFF
// to the CMake command-line to disable use of the sandbox.
// Uncomment this line to manually enable sandbox support.
// #define CEF_USE_SANDBOX 1

#if defined(CEF_USE_SANDBOX)
// The cef_sandbox.lib static library may not link successfully with all VS
// versions.
#pragma comment(lib, "cef_sandbox.lib")
#endif

scoped_ptr<MainContextImpl> context;
scoped_ptr<MainMessageLoop> message_loop;

bool QWebViewCEF::Initialize(bool supportOSR) {
  // Enable High-DPI support on Windows 7 or newer.
  CefEnableHighDPISupport();

  CefMainArgs main_args(GetModuleHandle(NULL));

  void* sandbox_info = NULL;

#if defined(CEF_USE_SANDBOX)
  // Manage the life span of the sandbox information object. This is necessary
  // for sandbox support on Windows. See cef_sandbox_win.h for complete details.
  CefScopedSandboxInfo scoped_sandbox;
  sandbox_info = scoped_sandbox.sandbox_info();
#endif

  // Parse command-line arguments.
  CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
  command_line->InitFromString(::GetCommandLineW());

  // Create a ClientApp of the correct type.
  CefRefPtr<CefApp> app;
  ClientApp::ProcessType process_type = ClientApp::GetProcessType(command_line);
  if (process_type == ClientApp::BrowserProcess) {
    app = new ClientAppBrowser();
  }
  else if (process_type == ClientApp::RendererProcess) {
    app = new ClientAppRenderer();
  }
  else if (process_type == ClientApp::OtherProcess) {
    app = new ClientAppOther();
  }

  // Execute the secondary process, if any.
  int exit_code = CefExecuteProcess(main_args, app, sandbox_info);
  if (exit_code >= 0)
    return false;

  // Create the main context object.
  context.reset(new MainContextImpl(
      command_line,
      true  // 必须为true，因为在MainMessageLoopQt::Quit() 中判断是否需要退出app
      // todo
      ));

  CefSettings settings;

#if !defined(CEF_USE_SANDBOX)
  settings.no_sandbox = true;
#endif

  // Populate the settings based on command line arguments.
  context->PopulateSettings(&settings);

  if (supportOSR)
    settings.windowless_rendering_enabled = true;

  // winsoft666: CEF线程模型
  //
  // 当 multi_threaded_message_loop = 0 时，CEF的UI线程与程序的主线程为一个线程，
  //     此时可以直接调用 CefRunMessageLoop 函数，该函数为阻塞的消息循环函数，用于处理程序消息和CEF消息，可以代替程序的消息循环；
  //     也可以单独实现程序的消息循环，并在程序消息循环中额外调用非阻塞的 CefDoMessageLoopWork 函数来处理CEF消息。
  //
  // 当 multi_threaded_message_loop = 1 时，CEF的UI线程与程序主线程为不同的线程，
  //
  // 可以使用 CefCurrentlyOn(TID_UI) 判断当前线程是否为CEF的UI线程，并使用 CefPostTask(TID_UI, ...)  将闭合体发送到CEF UI线程中执行。
  // 可以使用 CURRENTLY_ON_MAIN_THREAD() 判断当前线程是否为主线程，并使用 MAIN_POST_CLOSURE 或 MAIN_POST_TASK 将闭合体/任务发送到主线程中执行。
  //
  settings.multi_threaded_message_loop = 1;

  // Create the main message loop object.
  message_loop.reset(new MainMessageLoopQt);

  // Initialize CEF.
  bool init = context->Initialize(main_args, settings, app, sandbox_info);

  DCHECK(init) << "Cef Initialize Failed.";

  return true;
}

void QWebViewCEF::UnInitialize() {
  qDebug() << ">>>> QWebViewCEF::UnInitialize";

  // Shut down CEF.
  context->Shutdown();

  // Release objects in reverse order of creation.
  message_loop.reset();
  context.reset();
}
