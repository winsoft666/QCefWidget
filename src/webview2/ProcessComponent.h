#ifndef PROCESS_COMPONENT_H_
#define PROCESS_COMPONENT_H_
#pragma once

#include "stdafx.h"
#include "ComponentBase.h"
#include <WebView2.h>
#include "QWebView2Options.h"

class ProcessComponent : public ComponentBase {
   public:
    ProcessComponent(QWebViewWebView2Impl* d);
    ~ProcessComponent() override;

    void ShowBrowserProcessInfo();
    void CrashBrowserProcess();
    void CrashRenderProcess();
    void PerformanceInfo();
    void ShowProcessExtendedInfo();

    // Wait for process to exit for timeoutMs, then force quit it if it hasn't.
    static void EnsureProcessIsClosed(UINT processId, int timeoutMs);

   protected:
    std::wstring ProcessFailedKindToString(const COREWEBVIEW2_PROCESS_FAILED_KIND kind);
    std::wstring ProcessFailedReasonToString(const COREWEBVIEW2_PROCESS_FAILED_REASON reason);
    std::wstring ProcessKindToString(const COREWEBVIEW2_PROCESS_KIND kind);

   private:
    wil::com_ptr<ICoreWebView2> m_webView;
    wil::com_ptr<ICoreWebView2Environment> m_webViewEnvironment;

    UINT m_browserProcessId = 0;

    wil::com_ptr<ICoreWebView2ProcessInfoCollection> m_processCollection;
    EventRegistrationToken m_processFailedToken = {};
    EventRegistrationToken m_processInfosChangedToken = {};
    void AppendFrameInfo(wil::com_ptr<ICoreWebView2FrameInfo> frameInfo, std::wstringstream& result);
    wil::com_ptr<ICoreWebView2FrameInfo> GetAncestorMainFrameDirectChildFrameInfo(wil::com_ptr<ICoreWebView2FrameInfo> frameInfo);
    wil::com_ptr<ICoreWebView2FrameInfo> GetAncestorMainFrameInfo(wil::com_ptr<ICoreWebView2FrameInfo> frameInfo);
    std::wstring FrameKindToString(const COREWEBVIEW2_FRAME_KIND kind);
};
#endif  // !PROCESS_COMPONENT_H_
