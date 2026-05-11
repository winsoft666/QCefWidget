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
      // winsoft666:
      // terminate_when_all_windows_closed must be set to true because within MainMessageLoopQt::Quit(),
      // a determination is made as to whether the allWebViewsClosed signal needs to be sent.
      true));

  CefSettings settings;

#if !defined(CEF_USE_SANDBOX)
  settings.no_sandbox = true;
#endif

  // Populate the settings based on command line arguments.
  context->PopulateSettings(&settings);

  if (supportOSR)
    settings.windowless_rendering_enabled = true;

  // winsoft666: CEF Thread Model
  //
  // When multi_threaded_message_loop = 0,
  //   the CEF UI thread is the same as the main thread of the program.
  //   In this case, you can directly call the CefRunMessageLoop function. This is a blocking message loop function used to handle both program messages and CEF messages, and it can replace the program's message loop.
  //   Alternatively, you can implement the program's message loop separately and additionally call the non-blocking CefDoMessageLoopWork function within the program's message loop to handle CEF messages.
  //
  // When multi_threaded_message_loop = 1,
  //   the CEF UI thread is different from the main thread of the program.
  //
  //   You can use CefCurrentlyOn(TID_UI) to determine whether the current thread is the CEF UI thread, and use CefPostTask(TID_UI,...) to send a closure to the CEF UI thread for execution.
  //   You can use CURRENTLY_ON_MAIN_THREAD() to determine whether the current thread is the main thread, and use MAIN_POST_CLOSURE or MAIN_POST_TASK to send a closure/task to the main thread for execution.
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
#ifdef QT_DEBUG
  qDebug() << ">>>> QWebViewCEF::UnInitialize";
#endif

  // Shut down CEF.
  context->Shutdown();

  // Release objects in reverse order of creation.
  message_loop.reset();
  context.reset();
}
