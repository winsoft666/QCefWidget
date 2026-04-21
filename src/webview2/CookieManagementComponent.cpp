#include "stdafx.h"
#include "CookieManagementComponent.h"
#include "CheckFailure.h"
#include "QWebViewWebView2Impl.h"

using namespace Microsoft::WRL;

namespace {
static WebView2Cookie CookieToStruct(ICoreWebView2Cookie* cookie) {
    WebView2Cookie wv2Cookie;

    wil::unique_cotaskmem_string name;
    CHECK_FAILURE(cookie->get_Name(&name));
    wv2Cookie.name = QString::fromStdWString(name.get());

    wil::unique_cotaskmem_string value;
    CHECK_FAILURE(cookie->get_Value(&value));
    wv2Cookie.value = QString::fromStdWString(value.get());

    wil::unique_cotaskmem_string domain;
    CHECK_FAILURE(cookie->get_Domain(&domain));
    wv2Cookie.domain = QString::fromStdWString(domain.get());

    wil::unique_cotaskmem_string path;
    CHECK_FAILURE(cookie->get_Path(&path));
    wv2Cookie.path = QString::fromStdWString(path.get());

    double expires;
    CHECK_FAILURE(cookie->get_Expires(&expires));
    wv2Cookie.expires = expires;

    BOOL isHttpOnly = FALSE;
    CHECK_FAILURE(cookie->get_IsHttpOnly(&isHttpOnly));
    wv2Cookie.isHttpsOnly = !!isHttpOnly;

    COREWEBVIEW2_COOKIE_SAME_SITE_KIND same_site;
    std::wstring same_site_as_string;
    CHECK_FAILURE(cookie->get_SameSite(&same_site));
    switch (same_site) {
        case COREWEBVIEW2_COOKIE_SAME_SITE_KIND_NONE:
            wv2Cookie.sameSiteKind = CookieSameSiteKind::None;
            break;
        case COREWEBVIEW2_COOKIE_SAME_SITE_KIND_LAX:
            wv2Cookie.sameSiteKind = CookieSameSiteKind::Lax;
            break;
        case COREWEBVIEW2_COOKIE_SAME_SITE_KIND_STRICT:
            wv2Cookie.sameSiteKind = CookieSameSiteKind::Strict;
            break;
    }

    BOOL isSecure = FALSE;
    CHECK_FAILURE(cookie->get_IsSecure(&isSecure));
    wv2Cookie.isSecure = !!isSecure;

    BOOL isSession = FALSE;
    CHECK_FAILURE(cookie->get_IsSession(&isSession));
    wv2Cookie.isSession = !!isSession;

    return wv2Cookie;
}
}  // namespace

CookieManagementComponent::CookieManagementComponent(QWebViewWebView2Impl* d, bool isFromProfile) :
    ComponentBase(d) {
    m_webView = d_->GetWebView();

    wil::com_ptr<ICoreWebView2Settings> settings;
    CHECK_FAILURE(m_webView->get_Settings(&settings));
    CHECK_FAILURE(settings->put_IsWebMessageEnabled(TRUE));

    if (isFromProfile) {
        //! [CookieManagerProfile]
        auto webView2_13 = m_webView.try_query<ICoreWebView2_13>();
        CHECK_FEATURE_RETURN_EMPTY(webView2_13);
        wil::com_ptr<ICoreWebView2Profile> webView2Profile;
        CHECK_FAILURE(webView2_13->get_Profile(&webView2Profile));
        auto webView2Profile5 = webView2Profile.try_query<ICoreWebView2Profile5>();
        CHECK_FEATURE_RETURN_EMPTY(webView2Profile5);
        CHECK_FAILURE(webView2Profile5->get_CookieManager(&m_cookieManager));
        //! [CookieManagerProfile]
    }
    else {
        //! [CookieManager]
        auto webview2_2 = m_webView.try_query<ICoreWebView2_2>();
        CHECK_FEATURE_RETURN_EMPTY(webview2_2);
        CHECK_FAILURE(webview2_2->get_CookieManager(&m_cookieManager));
        //! [CookieManager]
    }
}

CookieManagementComponent::~CookieManagementComponent() {
}

void CookieManagementComponent::GetCookies(const std::wstring& uri, QueryCookieCallback cb) {
    //! [GetCookies]
    if (m_cookieManager) {
        HRESULT hr = m_cookieManager->GetCookies(
            uri.c_str(),
            Callback<ICoreWebView2GetCookiesCompletedHandler>([this, cb, uri](HRESULT error_code, ICoreWebView2CookieList* list) -> HRESULT {
                QVector<WebView2Cookie> cookiesList;
                CHECK_FAILURE(error_code);
                if (SUCCEEDED(error_code)) {
                    UINT cookie_list_size;
                    CHECK_FAILURE(list->get_Count(&cookie_list_size));
                    for (UINT i = 0; i < cookie_list_size; ++i) {
                        wil::com_ptr<ICoreWebView2Cookie> cookie;
                        CHECK_FAILURE(list->GetValueAtIndex(i, &cookie));

                        if (cookie.get()) {
                            cookiesList.append(CookieToStruct(cookie.get()));
                        }
                    }
                }

                if (cb) {
                    d_->RunAsync([cb, error_code, cookiesList]() {
                        cb(error_code, cookiesList);
                    });
                }

                return S_OK;
            })
                .Get());

        if (FAILED(hr)) {
            if (cb) {
                d_->RunAsync([cb, hr]() {
                    cb(hr, QVector<WebView2Cookie>());
                });
            }
        }
    }
    //! [GetCookies]
}

void CookieManagementComponent::ClearCookies() {
    CHECK_FAILURE(m_cookieManager->DeleteAllCookies());
}

HRESULT CookieManagementComponent::AddOrUpdateCookie(const std::wstring& name,
                                                     const std::wstring& value,
                                                     const std::wstring& domain,
                                                     const std::wstring& path) {
    wil::com_ptr<ICoreWebView2Cookie> cookie;
    HRESULT hr = m_cookieManager->CreateCookie(name.c_str(), value.c_str(), domain.c_str(), path.c_str(), &cookie);
    if (SUCCEEDED(hr)) {
        hr = m_cookieManager->AddOrUpdateCookie(cookie.get());
    }
    return hr;
}
