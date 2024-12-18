#include "stdafx.h"
#include "HostObjectComponent.h"
#include <algorithm>
#include "QWebViewWebView2Impl.h"
#include "HostObjectSimpleImpl.h"

using namespace Microsoft::WRL;

bool AreFileUrisEqual(std::wstring leftUri, std::wstring rightUri) {
    // Have to to lower due to current bug
    std::transform(leftUri.begin(), leftUri.end(), leftUri.begin(), ::tolower);
    std::transform(rightUri.begin(), rightUri.end(), rightUri.begin(), ::tolower);

    return leftUri == rightUri;
}

HostObjectComponent::HostObjectComponent(QWebViewWebView2Impl* d) :
    ComponentBase(d) {
    m_webView = d_->GetWebView();

    m_hostObject = Microsoft::WRL::Make<HostObjectSimple>(this);

    CHECK_FAILURE(m_webView->add_NavigationStarting(
        Microsoft::WRL::Callback<ICoreWebView2NavigationStartingEventHandler>(
            [this](ICoreWebView2* sender, ICoreWebView2NavigationStartingEventArgs* args) -> HRESULT {
                //! [AddHostObjectToScript]
                VARIANT remoteObjectAsVariant = {};
                m_hostObject.query_to<IDispatch>(&remoteObjectAsVariant.pdispVal);
                remoteObjectAsVariant.vt = VT_DISPATCH;

                // We can call AddHostObjectToScript multiple times in a row without
                // calling RemoveHostObject first. This will replace the previous object
                // with the new object. In our case this is the same object and everything is fine.
                CHECK_FAILURE(m_webView->AddHostObjectToScript(L"QWebView2", &remoteObjectAsVariant));
                remoteObjectAsVariant.pdispVal->Release();
                //! [AddHostObjectToScript]

                return S_OK;
            })
            .Get(),
        &m_navigationStartingToken));

    wil::com_ptr<ICoreWebView2_4> webview2_4 = m_webView.try_query<ICoreWebView2_4>();
    if (webview2_4) {
        // Register a handler for the FrameCreated event.
        // This handler can be used to add host objects to the created iframe.
        CHECK_FAILURE(webview2_4->add_FrameCreated(
            Callback<ICoreWebView2FrameCreatedEventHandler>(
                [this](
                    ICoreWebView2* sender,
                    ICoreWebView2FrameCreatedEventArgs* args) -> HRESULT {
                    wil::com_ptr<ICoreWebView2Frame> webviewFrame;
                    CHECK_FAILURE(args->get_Frame(&webviewFrame));

                    //wil::unique_cotaskmem_string name;
                    //CHECK_FAILURE(webviewFrame->get_Name(&name));

                    //! [AddHostObjectToScriptWithOrigins]
                    wil::unique_variant remoteObjectAsVariant;
                    // It will throw if m_hostObject fails the QI, but because it is our object
                    // it should always succeed.
                    m_hostObject.query_to<IDispatch>(&remoteObjectAsVariant.pdispVal);
                    remoteObjectAsVariant.vt = VT_DISPATCH;

                    // Create list of origins which will be checked.
                    // iframe will have access to host object only if its origin belongs
                    // to this list.
                    LPCWSTR origin = L"https://appassets.example/";

                    CHECK_FAILURE(webviewFrame->AddHostObjectToScriptWithOrigins(
                        L"QWebView2", &remoteObjectAsVariant, 1, &origin));
                    //! [AddHostObjectToScriptWithOrigins]

                    // Subscribe to frame name changed event
                    webviewFrame->add_NameChanged(
                        Callback<ICoreWebView2FrameNameChangedEventHandler>(
                            [this](ICoreWebView2Frame* sender, IUnknown* args) -> HRESULT {
                                wil::unique_cotaskmem_string newName;
                                CHECK_FAILURE(sender->get_Name(&newName));
                                // Handle name changed event
                                return S_OK;
                            })
                            .Get(),
                        NULL);

                    // Subscribe to frame destroyed event
                    webviewFrame->add_Destroyed(
                        Callback<ICoreWebView2FrameDestroyedEventHandler>(
                            [this](ICoreWebView2Frame* sender, IUnknown* args) -> HRESULT {
                                /*Cleanup on frame destruction*/
                                return S_OK;
                            })
                            .Get(),
                        NULL);
                    return S_OK;
                })
                .Get(),
            &m_frameCreatedToken));
    }
}

HostObjectComponent::~HostObjectComponent() {
    m_webView->RemoveHostObjectFromScript(L"QWebView2");
    m_webView->remove_NavigationStarting(m_navigationStartingToken);
    wil::com_ptr<ICoreWebView2_4> webview2_4 = m_webView.try_query<ICoreWebView2_4>();
    if (webview2_4) {
        webview2_4->remove_FrameCreated(m_frameCreatedToken);
    }
}

ScriptCallCallback HostObjectComponent::GetScriptCallNotify() {
    return d_->GetSciptCallNotify();
}
