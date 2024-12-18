#ifndef SETTINGS_COMPONENT_H_
#define SETTINGS_COMPONENT_H_
#if defined(_MSVC_) && _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include "ComponentBase.h"
#include "WebView2.h"

// Some utility functions
wil::unique_bstr GetDomainOfUri(PWSTR uri);

class SettingsComponent : public ComponentBase {
   public:
    SettingsComponent(QWebViewWebView2Impl* d, SettingsComponent* old = nullptr);

    // a list of blocked domains
    void ChangeBlockedSites(const std::vector<std::wstring>& sites);

    // Check the URI's domain against the blocked sites list
    bool ShouldBlockUri(PWSTR uri);
    bool ShouldBlockScriptForUri(PWSTR uri);

    void SetBlockImages(bool blockImages);
    void SetReplaceImages(bool replaceImages);

    // Register a WebResourceRequested handler which adds a custom User-Agent HTTP header to all requests.
    void SetUserAgent(const std::wstring& userAgent);
    void EnableCustomClientCertificateSelection();
    void ToggleCustomServerCertificateSupport();

    HRESULT EnableDefaultContextMenu(bool enabled);

    void SetTrackingPreventionLevel(COREWEBVIEW2_TRACKING_PREVENTION_LEVEL value);

    HRESULT SetBuiltInErrorPageEnabled(bool enabled);
    bool IsBuiltInErrorPageEnabled();

    ~SettingsComponent() override;

   private:
    HRESULT OnPermissionRequested(ICoreWebView2* sender, ICoreWebView2PermissionRequestedEventArgs* args);

   private:
    wil::com_ptr<ICoreWebView2> m_webView;
    wil::com_ptr<ICoreWebView2_5> m_webView2_5;
    wil::com_ptr<ICoreWebView2_11> m_webView2_11;
    wil::com_ptr<ICoreWebView2_12> m_webView2_12;
    wil::com_ptr<ICoreWebView2_13> m_webView2_13;
    wil::com_ptr<ICoreWebView2_14> m_webView2_14;
    wil::com_ptr<ICoreWebView2_15> m_webView2_15;
    wil::com_ptr<ICoreWebView2_18> m_webView2_18;
    wil::com_ptr<ICoreWebView2Settings> m_settings;
    wil::com_ptr<ICoreWebView2Settings2> m_settings2;
    wil::com_ptr<ICoreWebView2Settings3> m_settings3;
    wil::com_ptr<ICoreWebView2Settings4> m_settings4;
    wil::com_ptr<ICoreWebView2Settings5> m_settings5;
    wil::com_ptr<ICoreWebView2Settings6> m_settings6;
    wil::com_ptr<ICoreWebView2Settings7> m_settings7;
    wil::com_ptr<ICoreWebView2Settings8> m_settings8;
    wil::com_ptr<ICoreWebView2Controller> m_controller;
    wil::com_ptr<ICoreWebView2Controller3> m_controller3;
    wil::com_ptr<ICoreWebView2Environment> m_webViewEnvironment;
    wil::com_ptr<ICoreWebView2ContextMenuItem> m_displayPageUrlContextSubMenuItem;

    bool m_blockImages = false;
    bool m_replaceImages = false;
    bool m_changeUserAgent = false;
    bool m_isScriptEnabled = true;
    bool m_blockedSitesSet = false;
    bool m_raiseClientCertificate = false;
    std::vector<std::wstring> m_blockedSites;
    std::wstring m_overridingUserAgent;
    bool m_raiseServerCertificateError = false;
    bool m_launchingExternalUriScheme = false;

    EventRegistrationToken m_navigationStartingToken = {};
    EventRegistrationToken m_frameNavigationStartingToken = {};
    EventRegistrationToken m_webResourceRequestedTokenForImageBlocking = {};
    EventRegistrationToken m_webResourceRequestedTokenForImageReplacing = {};
    EventRegistrationToken m_webResourceRequestedTokenForUserAgent = {};
    EventRegistrationToken m_permissionRequestedToken = {};
    EventRegistrationToken m_ClientCertificateRequestedToken = {};
    EventRegistrationToken m_ServerCertificateErrorToken = {};
    EventRegistrationToken m_launchingExternalUriSchemeToken = {};
};
#endif  // !SETTINGS_COMPONENT_H_
