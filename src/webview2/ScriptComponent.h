#ifndef SCRIPT_COMPONENT_H_
#define SCRIPT_COMPONENT_H_
#if defined(_MSVC_) && _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <map>
#include <set>
#include <sstream>
#include "ComponentBase.h"
#include "QWebView2Options.h"
#include <WebView2.h>

const std::wstring GetJSONStringField(PCWSTR jsonMessage, PCWSTR fieldName);

class ScriptComponent : public ComponentBase {
   public:
    typedef std::function<void(HRESULT errorCode, const std::wstring& resultObjectAsJson)> ResultCallback;
    ScriptComponent(QWebViewWebView2Impl* d);

    HRESULT InjectScript(const std::wstring& s, ResultCallback cb);
    HRESULT InjectScriptInIFrame(int index, const std::wstring& s, ResultCallback cb);

    // Register/Deregister that script script to execute whenever a new page loads.
    HRESULT AddInitializeScript(const std::wstring& s, std::function<void(HRESULT, std::wstring id)> cb);
    HRESULT RemoveInitializeScript(const std::wstring& id);

    // Post string as a web message.
    HRESULT PostStringWebMessage(const std::wstring& s);

    // Post JSON string as a web message.
    HRESULT PostJsonWebMessage(const std::wstring& s);

    // Post string as a web message to the first iframe.
    HRESULT PostStringWebMessageFirstIFrame(const std::wstring& s);

    // Post JSON string as a web message to the first iframe.
    HRESULT PostJsonWebMessageFirstIFrame(const std::wstring& s);

    // Require ICoreWebView2_21
    HRESULT ExecuteScriptWithResult(const std::wstring& s, ScriptResultCallback cb);

    HRESULT AddComObject(const std::wstring& comId);

    void OpenTaskManagerWindow();

   private:
    ~ScriptComponent() override;
    void HandleIFrames();
    std::wstring IFramesToString();

   private:
    std::vector<wil::com_ptr<ICoreWebView2Frame>> m_frames;
    wil::com_ptr<ICoreWebView2> m_webView;
    int m_siteEmbeddingIFrameCount = 0;
};

#endif  // !SCRIPT_COMPONENT_H_
