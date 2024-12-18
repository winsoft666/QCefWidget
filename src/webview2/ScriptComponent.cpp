#include "stdafx.h"
#include "ScriptComponent.h"
#include <algorithm>
#include <sstream>
#include "QWebViewWebView2Impl.h"

using namespace Microsoft::WRL;

//! [AdditionalAllowedFrameAncestors_1]
const std::wstring myTrustedSite = L"https://appassets.example";

// The trusted page is using <iframe name="my_site_embedding_frame">
// element to embed other sites.
const std::wstring siteEmbeddingFrameName = L"my_site_embedding_frame";

bool AreSitesSame(PCWSTR url1, PCWSTR url2) {
    wil::com_ptr<IUri> uri1;
    CHECK_FAILURE(CreateUri(url1, Uri_CREATE_CANONICALIZE, 0, &uri1));
    DWORD scheme1 = -1;
    DWORD port1 = 0;
    wil::unique_bstr host1;
    CHECK_FAILURE(uri1->GetScheme(&scheme1));
    CHECK_FAILURE(uri1->GetHost(&host1));
    CHECK_FAILURE(uri1->GetPort(&port1));
    wil::com_ptr<IUri> uri2;
    CHECK_FAILURE(CreateUri(url2, Uri_CREATE_CANONICALIZE, 0, &uri2));
    DWORD scheme2 = -1;
    DWORD port2 = 0;
    wil::unique_bstr host2;
    CHECK_FAILURE(uri2->GetScheme(&scheme2));
    CHECK_FAILURE(uri2->GetHost(&host2));
    CHECK_FAILURE(uri2->GetPort(&port2));
    return (scheme1 == scheme2) && (port1 == port2) && (wcscmp(host1.get(), host2.get()) == 0);
}

// App specific logic to decide whether the page is fully trusted.
bool IsAppContentUri(PCWSTR pageUrl) {
    return AreSitesSame(pageUrl, myTrustedSite.c_str());
}

//! [AdditionalAllowedFrameAncestors_1]

// Simple functions to retrieve fields from a JSON message.
// For production code, you should use a real JSON parser library.
const std::wstring GetJSONStringField(PCWSTR JsonMessage, PCWSTR fieldName) {
    std::wstring message(JsonMessage);
    std::wstring startSubStr = L"\"";
    startSubStr.append(fieldName);
    startSubStr.append(L"\":\"");
    std::string::size_type start = message.find(startSubStr);
    if (start == std::wstring::npos)
        return std::wstring();
    start += startSubStr.length();
    std::string::size_type end = message.find(L'\"', start);
    if (end == std::wstring::npos)
        return std::wstring();
    return message.substr(start, end - start);
}

const int64_t GetJSONIntegerField(PCWSTR JsonMessage, PCWSTR fieldName) {
    std::wstring message(JsonMessage);
    std::wstring startSubStr = L"\"";
    startSubStr.append(fieldName);
    startSubStr.append(L"\":");
    std::string::size_type start = message.find(startSubStr);
    if (start == std::wstring::npos)
        return 0;
    start += startSubStr.length();
    return _wtoi64(message.substr(start).c_str());
}

ScriptComponent::ScriptComponent(QWebViewWebView2Impl* d) :
    ComponentBase(d) {
    m_webView = d_->GetWebView();

    HandleIFrames();
}

HRESULT ScriptComponent::InjectScript(const std::wstring& s, ResultCallback cb) {
    HRESULT hr = E_FAIL;
    if (m_webView && !s.empty()) {
        hr = m_webView->ExecuteScript(
            s.c_str(),
            Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
                [cb](HRESULT error, PCWSTR result) -> HRESULT {
                    if (cb) {
                        cb(error, result);
                    }
                    return S_OK;
                })
                .Get());
    }
    return hr;
}

HRESULT ScriptComponent::InjectScriptInIFrame(int index, const std::wstring& s, ResultCallback cb) {
    HRESULT hr = E_FAIL;
    if (s.empty()) {
        return hr;
    }

    if (index < 0 || index >= static_cast<int>(m_frames.size())) {
        ShowFailure(S_OK, L"Can not read frame index or it is out of available range");
        return hr;
    }

    wil::com_ptr<ICoreWebView2Frame2> frame2 = m_frames[index].try_query<ICoreWebView2Frame2>();
    if (frame2) {
        hr = frame2->ExecuteScript(
            s.c_str(),
            Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
                [cb](HRESULT error, PCWSTR result) -> HRESULT {
                    if (cb) {
                        cb(error, result);
                    }
                    return S_OK;
                })
                .Get());
    }

    return hr;
}

HRESULT ScriptComponent::AddInitializeScript(const std::wstring& s, std::function<void(HRESULT, std::wstring id)> cb) {
    HRESULT hr = E_FAIL;
    if (!s.empty()) {
        hr = m_webView->AddScriptToExecuteOnDocumentCreated(
            s.c_str(),
            Callback<ICoreWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler>(
                [cb](HRESULT error, PCWSTR id) -> HRESULT {
                    if (error != S_OK) {
                        ShowFailure(error, L"Add initialize script failed");
                    }

                    if (cb) {
                        cb(error, id);
                    }

                    return S_OK;
                })
                .Get());
    }

    return hr;
}

HRESULT ScriptComponent::RemoveInitializeScript(const std::wstring& id) {
    HRESULT hr = E_FAIL;
    if (!id.empty()) {
        hr = m_webView->RemoveScriptToExecuteOnDocumentCreated(id.c_str());
    }
    return hr;
}

HRESULT ScriptComponent::PostStringWebMessage(const std::wstring& s) {
    HRESULT hr = E_FAIL;
    if (!s.empty()) {
        hr = m_webView->PostWebMessageAsString(s.c_str());
    }
    return hr;
}

HRESULT ScriptComponent::PostJsonWebMessage(const std::wstring& s) {
    HRESULT hr = E_FAIL;
    if (!s.empty()) {
        hr = m_webView->PostWebMessageAsJson(s.c_str());
    }
    return hr;
}

HRESULT ScriptComponent::PostStringWebMessageFirstIFrame(const std::wstring& s) {
    HRESULT hr = E_FAIL;
    if (s.empty()) {
        return hr;
    }

    if (m_frames.empty()) {
        ShowFailure(S_OK, L"No iframes found");
        return hr;
    }

    wil::com_ptr<ICoreWebView2Frame2> frame2 = m_frames[0].try_query<ICoreWebView2Frame2>();
    if (frame2) {
        hr = frame2->PostWebMessageAsString(s.c_str());
    }

    return hr;
}

HRESULT ScriptComponent::PostJsonWebMessageFirstIFrame(const std::wstring& s) {
    HRESULT hr = E_FAIL;
    if (s.empty()) {
        return hr;
    }

    if (m_frames.empty()) {
        ShowFailure(S_OK, L"No iframes found");
        return hr;
    }

    wil::com_ptr<ICoreWebView2Frame2> frame2 = m_frames[0].try_query<ICoreWebView2Frame2>();
    if (frame2) {
        hr = frame2->PostWebMessageAsJson(s.c_str());
    }

    return hr;
}

HRESULT ScriptComponent::AddComObject(const std::wstring& comId) {
    HRESULT hr = E_FAIL;
    if (comId.empty())
        return hr;

    CLSID classId = {};
    hr = CLSIDFromProgID(comId.c_str(), &classId);
    if (FAILED(hr)) {
        hr = CLSIDFromString(comId.c_str(), &classId);
    }

    if (FAILED(hr)) {
        ShowFailure(hr, L"Failed to convert string to CLSID or ProgID");
        return hr;
    }

    wil::com_ptr_nothrow<IDispatch> objectAsDispatch;
    hr = CoCreateInstance(
        classId,
        nullptr,
        CLSCTX_LOCAL_SERVER | CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER,
        IID_PPV_ARGS(&objectAsDispatch));
    if (FAILED(hr)) {
        ShowFailure(hr, L"CoCreateInstance failed");
        return hr;
    }

    wil::unique_variant objectAsVariant;
    objectAsVariant.vt = VT_DISPATCH;
    hr = objectAsDispatch.query_to(IID_PPV_ARGS(&objectAsVariant.pdispVal));
    if (FAILED(hr)) {
        ShowFailure(hr, L"COM object doesn't support IDispatch");
        return hr;
    }

    hr = m_webView->AddHostObjectToScript(L"example", &objectAsVariant);
    if (FAILED(hr)) {
        ShowFailure(hr, L"AddHostObjectToScript failed");
    }
    return hr;
}

HRESULT ScriptComponent::ExecuteScriptWithResult(const std::wstring& s, ScriptResultCallback cb) {
    HRESULT hr = E_FAIL;
    if (s.empty()) {
        return hr;
    }

    wil::com_ptr<ICoreWebView2_21> webView = m_webView.try_query<ICoreWebView2_21>();

    if (!webView) {
        ShowFailure(S_OK, L"Get ICoreWebView2_21 failed");
        return E_NOINTERFACE;
    }

    // The main interface for excute script, the first param is the string
    // which user want to execute, the second param is the callback to process
    // the result, here use a lamada to the param.
    hr = webView->ExecuteScriptWithResult(
        s.c_str(),
        // The callback function has two param, the first one is the status of call.s
        // it will always be the S_OK for now, and the second is the result struct.
        Callback<ICoreWebView2ExecuteScriptWithResultCompletedHandler>(
            [cb](HRESULT errorCode, ICoreWebView2ExecuteScriptResult* result) -> HRESULT {
                if (errorCode != S_OK || result == nullptr) {
                    if (cb) {
                        cb(errorCode, "", "");
                    }
                    return S_OK;
                }

                wil::com_ptr<ICoreWebView2ScriptException> exception;
                BOOL isSuccess;

                HRESULT hr;
                // User should always invoke the get_Success firstly to get if execute  success.
                hr = result->get_Succeeded(&isSuccess);
                if (hr != S_OK) {
                    if (cb) {
                        cb(hr, "", "Get execute status failed");
                    }
                    return S_OK;
                }

                // If execute success, then we can get the raw json data, and try to get the string.
                if (isSuccess) {
                    wil::unique_cotaskmem_string rawJsonData;
                    // Get the raw json.
                    if (result->get_ResultAsJson(&rawJsonData) == S_OK) {
                        if (cb) {
                            cb(S_OK, QString::fromStdWString(rawJsonData.get()), "");
                        }
                        return S_OK;
                    }

                    // Get the string, and if the result is not the string type,
                    // it will return the E_INVALIDARG.
                    wil::unique_cotaskmem_string stringData;
                    BOOL isString = FALSE;
                    hr = result->TryGetResultAsString(&stringData, &isString);
                    if (hr == S_OK && isString) {
                        if (cb) {
                            cb(S_OK, QString::fromStdWString(stringData.get()), "");
                        }
                    }
                    else {
                        if (cb) {
                            cb(hr, "", "Get string failed");
                        }
                    }
                }
                else {
                    // If execute failed, then we can get the exception struct to get
                    // the reason of failed.
                    wil::unique_cotaskmem_string exceptionName;
                    wil::unique_cotaskmem_string exceptionMessage;
                    wil::unique_cotaskmem_string exceptionDetail;
                    uint32_t lineNumber = 0;
                    uint32_t columnNumber = 0;

                    if (result->get_Exception(&exception) == S_OK) {
                        if (exception) {
                            exception->get_Name(&exceptionName);
                            exception->get_Message(&exceptionMessage);
                            exception->get_ToJson(&exceptionDetail);
                            exception->get_LineNumber(&lineNumber);
                            exception->get_ColumnNumber(&columnNumber);

                            std::wstring excptionInfo =
                                (exceptionName.get() ? std::wstring(exceptionName.get()) : L"") +
                                L" : " + (exceptionMessage.get() ? std::wstring(exceptionMessage.get()) : L"") +
                                L" : " + (exceptionDetail.get() ? std::wstring(exceptionDetail.get()) : L"") +
                                L" : (" + std::to_wstring(lineNumber) + L":" + std::to_wstring(columnNumber) + L")";

                            if (cb) {
                                cb(E_FAIL, "", QString::fromStdWString(excptionInfo));
                            }
                        }
                        else {
                            if (cb) {
                                cb(E_FAIL, "", "Unable to get exception information");
                            }
                        }
                    }
                    else {
                        if (cb) {
                            cb(E_FAIL, "", "Unable to get exception information");
                        }
                    }
                }
                return S_OK;
            })
            .Get());
    return hr;
}

void ScriptComponent::HandleIFrames() {
    wil::com_ptr<ICoreWebView2_4> webview2_4 = m_webView.try_query<ICoreWebView2_4>();
    if (webview2_4) {
        CHECK_FAILURE(webview2_4->add_FrameCreated(
            Callback<ICoreWebView2FrameCreatedEventHandler>(
                [this](ICoreWebView2* sender, ICoreWebView2FrameCreatedEventArgs* args) -> HRESULT {
                    wil::com_ptr<ICoreWebView2Frame> webviewFrame;
                    CHECK_FAILURE(args->get_Frame(&webviewFrame));

                    m_frames.emplace_back(webviewFrame);

                    webviewFrame->add_Destroyed(
                        Callback<ICoreWebView2FrameDestroyedEventHandler>(
                            [this](ICoreWebView2Frame* sender, IUnknown* args) -> HRESULT {
                                auto frame = std::find(m_frames.begin(), m_frames.end(), sender);
                                if (frame != m_frames.end()) {
                                    m_frames.erase(frame);
                                }
                                return S_OK;
                            })
                            .Get(),
                        NULL);
                    return S_OK;
                })
                .Get(),
            NULL));

        //! [AdditionalAllowedFrameAncestors_2]
        // Set up the event listeners to handle site embedding scenario. The code will take effect
        // when the site embedding page is navigated to and the embedding iframe navigates to the
        // site that we want to embed.

        // This part is trying to scope the API usage to the specific scenario where we are
        // embedding a site. The result is recorded in m_siteEmbeddingIFrameCount.
        CHECK_FAILURE(webview2_4->add_FrameCreated(
            Callback<ICoreWebView2FrameCreatedEventHandler>(
                [this](ICoreWebView2* sender, ICoreWebView2FrameCreatedEventArgs* args) -> HRESULT {
                    wil::com_ptr<ICoreWebView2Frame> webviewFrame;
                    CHECK_FAILURE(args->get_Frame(&webviewFrame));

                    wil::unique_cotaskmem_string page_url;
                    CHECK_FAILURE(m_webView->get_Source(&page_url));

                    // IsAppContentUri verifies that page_url belongs to  the app.
                    if (IsAppContentUri(page_url.get())) {
                        // We are on trusted pages. Now check whether it is the iframe we plan
                        // to embed arbitrary sites.
                        wil::unique_cotaskmem_string frame_name;
                        CHECK_FAILURE(webviewFrame->get_Name(&frame_name));

                        if (siteEmbeddingFrameName == frame_name.get()) {
                            ++m_siteEmbeddingIFrameCount;
                            CHECK_FAILURE(webviewFrame->add_Destroyed(
                                Microsoft::WRL::Callback<ICoreWebView2FrameDestroyedEventHandler>(
                                    [this](ICoreWebView2Frame* sender, IUnknown* args) -> HRESULT {
                                        --m_siteEmbeddingIFrameCount;
                                        return S_OK;
                                    })
                                    .Get(),
                                nullptr));
                        }
                    }
                    return S_OK;
                })
                .Get(),
            nullptr));

        // Using FrameNavigationStarting event instead of NavigationStarting event of
        // CoreWebViewFrame to cover all possible nested iframes inside the embedded site as
        // CoreWebViewFrame object currently only support first level iframes in the top page.
        CHECK_FAILURE(m_webView->add_FrameNavigationStarting(
            Microsoft::WRL::Callback<ICoreWebView2NavigationStartingEventHandler>(
                [this](ICoreWebView2* sender, ICoreWebView2NavigationStartingEventArgs* args) -> HRESULT {
                    return S_OK;
                })
                .Get(),
            nullptr));
        //! [AdditionalAllowedFrameAncestors_2]
    }
}

std::wstring ScriptComponent::IFramesToString() {
    std::wstring data;
    for (size_t i = 0; i < m_frames.size(); i++) {
        wil::unique_cotaskmem_string name;
        CHECK_FAILURE(m_frames[i]->get_Name(&name));
        if (i > 0)
            data += L"; ";
        data += std::to_wstring(i) + L": " + (!std::wcslen(name.get()) ? L"<empty_name>" : name.get());
    }
    return data;
}

void ScriptComponent::OpenTaskManagerWindow() {
    auto webView6 = m_webView.try_query<ICoreWebView2_6>();

    if (webView6) {
        webView6->OpenTaskManagerWindow();
    }
}

ScriptComponent::~ScriptComponent() {
}
