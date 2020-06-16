#include "CefBrowserApp.h"
#include "CefGlobalSetting.h"
#include <include/cef_browser.h>
#include <include/cef_command_line.h>
#include <include/wrapper/cef_helpers.h>
#include <string>

CefBrowserApp::CefBrowserApp() {}

CefBrowserApp::~CefBrowserApp() {}

void CefBrowserApp::OnBeforeCommandLineProcessing(const CefString &process_type,
                                                      CefRefPtr<CefCommandLine> command_line) {
  // Chromium has removed --disable-surfaces as per
  // https://codereview.chromium.org/1603133003 so the workaround specified
  // here is no longer applicable
  // command_line->AppendSwitch("disable-surfaces");

  command_line->AppendSwitch("no-proxy-server");
  command_line->AppendSwitch("disable-extensions");
  command_line->AppendSwitch("allow-file-access-from-files");
  command_line->AppendSwitchWithValue("disable-features", "NetworkService");

  if (!CefGlobalSetting::gpu_enabled) {
    // D3D11模式不能禁用GPU加速，2623版本不会使用D3D11模式
    command_line->AppendSwitch("disable-gpu");
    command_line->AppendSwitch("disable-gpu-compositing");
  }

  if (CefGlobalSetting::flush_plugin_path.length() > 0 &&
      CefGlobalSetting::flush_plugin_ver.length() > 0) {
    command_line->AppendSwitchWithValue("ppapi-flash-path", CefGlobalSetting::flush_plugin_path);
    command_line->AppendSwitchWithValue("ppapi-flash-version", CefGlobalSetting::flush_plugin_ver);
  }
}

void CefBrowserApp::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) {}

CefRefPtr<CefResourceBundleHandler> CefBrowserApp::GetResourceBundleHandler() {
  return nullptr;
}

CefRefPtr<CefBrowserProcessHandler> CefBrowserApp::GetBrowserProcessHandler() { return this; }

CefRefPtr<CefRenderProcessHandler> CefBrowserApp::GetRenderProcessHandler() { return nullptr; }

//////////////////////////////////////////////////////////////////////////
void CefBrowserApp::OnContextInitialized() {
  CEF_REQUIRE_UI_THREAD();

  // Register cookieable schemes with the global cookie manager.
  CefRefPtr<CefCookieManager> manager = CefCookieManager::GetGlobalManager(nullptr);
  DCHECK(manager.get());
  manager->SetSupportedSchemes(cookieable_schemes_, true, nullptr);
}

void CefBrowserApp::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) {}

void CefBrowserApp::OnRenderProcessThreadCreated(CefRefPtr<CefListValue> extra_info) {
  CEF_REQUIRE_IO_THREAD();
}

CefRefPtr<CefPrintHandler> CefBrowserApp::GetPrintHandler() { return nullptr; }

void CefBrowserApp::OnScheduleMessagePumpWork(int64 delay_ms) {}