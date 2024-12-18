#ifndef COOKIE_MANAGEMENT_COMPONENT_H_
#define COOKIE_MANAGEMENT_COMPONENT_H_
#pragma once
#include "stdafx.h"
#include <string>
#include "ComponentBase.h"
#include <WebView2.h>
#include "QWebView2Options.h"

class CookieManagementComponent : public ComponentBase {
   public:
    CookieManagementComponent(QWebViewWebView2Impl* d, bool isFromProfile = false);
    ~CookieManagementComponent() override;

    void GetCookies(const std::wstring& uri, QueryCookieCallback cb);
    void ClearCookies();
    HRESULT AddOrUpdateCookie(const std::wstring& name, const std::wstring& value, const std::wstring& domain, const std::wstring& path);

   private:
    wil::com_ptr<ICoreWebView2Environment> m_webViewEnvironment;
    wil::com_ptr<ICoreWebView2> m_webView;
    wil::com_ptr<ICoreWebView2CookieManager> m_cookieManager;
};
#endif  // !COOKIE_MANAGEMENT_COMPONENT_H_
