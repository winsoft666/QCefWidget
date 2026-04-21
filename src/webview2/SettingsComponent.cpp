#include "stdafx.h"
#include "SettingsComponent.h"
#include <shellapi.h>
#include <Shlwapi.h>
#include "QWebViewWebView2Impl.h"

using namespace Microsoft::WRL;

SettingsComponent::SettingsComponent(
    QWebViewWebView2Impl* d,
    SettingsComponent* old) :
    ComponentBase(d) {
    m_webView = d_->GetWebView();
    m_controller = d_->GetController();
    m_webViewEnvironment = d_->GetEnvironment();

    CHECK_FAILURE(m_webView->get_Settings(&m_settings));

    m_settings2 = m_settings.try_query<ICoreWebView2Settings2>();
    m_settings3 = m_settings.try_query<ICoreWebView2Settings3>();
    m_settings4 = m_settings.try_query<ICoreWebView2Settings4>();
    m_settings5 = m_settings.try_query<ICoreWebView2Settings5>();
    m_settings6 = m_settings.try_query<ICoreWebView2Settings6>();
    m_settings7 = m_settings.try_query<ICoreWebView2Settings7>();
    m_settings8 = m_settings.try_query<ICoreWebView2Settings8>();
    m_controller3 = m_controller.try_query<ICoreWebView2Controller3>();
    m_webView2_5 = m_webView.try_query<ICoreWebView2_5>();
    m_webView2_11 = m_webView.try_query<ICoreWebView2_11>();
    m_webView2_12 = m_webView.try_query<ICoreWebView2_12>();
    m_webView2_13 = m_webView.try_query<ICoreWebView2_13>();
    m_webView2_14 = m_webView.try_query<ICoreWebView2_14>();
    m_webView2_15 = m_webView.try_query<ICoreWebView2_15>();
    m_webView2_18 = m_webView.try_query<ICoreWebView2_18>();

    // Copy old settings if desired
    if (old) {
        BOOL setting;
        CHECK_FAILURE(old->m_settings->get_IsScriptEnabled(&setting));
        CHECK_FAILURE(m_settings->put_IsScriptEnabled(setting));
        CHECK_FAILURE(old->m_settings->get_IsWebMessageEnabled(&setting));
        CHECK_FAILURE(m_settings->put_IsWebMessageEnabled(setting));
        CHECK_FAILURE(old->m_settings->get_AreDefaultScriptDialogsEnabled(&setting));
        CHECK_FAILURE(m_settings->put_AreDefaultScriptDialogsEnabled(setting));
        CHECK_FAILURE(old->m_settings->get_IsStatusBarEnabled(&setting));
        CHECK_FAILURE(m_settings->put_IsStatusBarEnabled(setting));
        CHECK_FAILURE(old->m_settings->get_AreDevToolsEnabled(&setting));
        CHECK_FAILURE(m_settings->put_AreDevToolsEnabled(setting));
        CHECK_FAILURE(old->m_settings->get_IsBuiltInErrorPageEnabled(&setting));
        CHECK_FAILURE(m_settings->put_IsBuiltInErrorPageEnabled(setting));
        CHECK_FAILURE(old->m_settings->get_AreDefaultScriptDialogsEnabled(&setting));
        CHECK_FAILURE(m_settings->put_AreDefaultScriptDialogsEnabled(setting));
        CHECK_FAILURE(old->m_settings->get_AreDefaultContextMenusEnabled(&setting));
        CHECK_FAILURE(m_settings->put_AreDefaultContextMenusEnabled(setting));
        CHECK_FAILURE(old->m_settings->get_AreHostObjectsAllowed(&setting));
        CHECK_FAILURE(m_settings->put_AreHostObjectsAllowed(setting));
        CHECK_FAILURE(old->m_settings->get_IsZoomControlEnabled(&setting));
        CHECK_FAILURE(m_settings->put_IsZoomControlEnabled(setting));
        if (old->m_settings2 && m_settings2) {
            LPWSTR user_agent;
            CHECK_FAILURE(old->m_settings2->get_UserAgent(&user_agent));
            CHECK_FAILURE(m_settings2->put_UserAgent(user_agent));
        }
        if (old->m_settings3 && m_settings3) {
            CHECK_FAILURE(old->m_settings3->get_AreBrowserAcceleratorKeysEnabled(&setting));
            CHECK_FAILURE(m_settings3->put_AreBrowserAcceleratorKeysEnabled(setting));
        }
        if (old->m_settings4 && m_settings4) {
            CHECK_FAILURE(old->m_settings4->get_IsPasswordAutosaveEnabled(&setting));
            CHECK_FAILURE(m_settings4->put_IsPasswordAutosaveEnabled(setting));
            CHECK_FAILURE(old->m_settings4->get_IsGeneralAutofillEnabled(&setting));
            CHECK_FAILURE(m_settings4->put_IsGeneralAutofillEnabled(setting));
        }
        if (old->m_settings5 && m_settings5) {
            CHECK_FAILURE(old->m_settings5->get_IsPinchZoomEnabled(&setting));
            CHECK_FAILURE(m_settings5->put_IsPinchZoomEnabled(setting));
        }
        if (old->m_settings6 && m_settings6) {
            CHECK_FAILURE(old->m_settings6->get_IsSwipeNavigationEnabled(&setting));
            CHECK_FAILURE(m_settings6->put_IsSwipeNavigationEnabled(setting));
        }
        if (old->m_settings7 && m_settings7) {
            COREWEBVIEW2_PDF_TOOLBAR_ITEMS hiddenPdfToolbarItems;
            CHECK_FAILURE(old->m_settings7->get_HiddenPdfToolbarItems(&hiddenPdfToolbarItems));
            CHECK_FAILURE(m_settings7->put_HiddenPdfToolbarItems(hiddenPdfToolbarItems));
        }
        if (old->m_settings8 && m_settings8) {
            CHECK_FAILURE(old->m_settings8->get_IsReputationCheckingRequired(&setting));
            CHECK_FAILURE(m_settings8->put_IsReputationCheckingRequired(setting));
        }
        SetBlockImages(old->m_blockImages);
        SetReplaceImages(old->m_replaceImages);
        m_isScriptEnabled = old->m_isScriptEnabled;
        m_blockedSitesSet = old->m_blockedSitesSet;
        m_blockedSites = std::move(old->m_blockedSites);
        EnableCustomClientCertificateSelection();
        ToggleCustomServerCertificateSupport();
    }

    //! [NavigationStarting]
    // Register a handler for the NavigationStarting event.
    // This handler will check the domain being navigated to, and if the domain
    // matches a list of blocked sites, it will cancel the navigation and
    // possibly display a warning page.  It will also disable JavaScript on
    // selected websites.
    CHECK_FAILURE(m_webView->add_NavigationStarting(
        Callback<ICoreWebView2NavigationStartingEventHandler>(
            [this](ICoreWebView2* sender, ICoreWebView2NavigationStartingEventArgs* args)
                -> HRESULT {
                wil::unique_cotaskmem_string uri;
                CHECK_FAILURE(args->get_Uri(&uri));

                if (ShouldBlockUri(uri.get())) {
                    CHECK_FAILURE(args->put_Cancel(true));

                    // If the user clicked a link to navigate, show a warning page.
                    BOOL userInitiated;
                    CHECK_FAILURE(args->get_IsUserInitiated(&userInitiated));
                    //! [NavigateToString]
                    static const PCWSTR htmlContent =
                        L"<h1>Domain Blocked</h1>"
                        L"<p>You've attempted to navigate to a domain in the blocked "
                        L"sites list. Press back to return to the previous page.</p>";
                    CHECK_FAILURE(sender->NavigateToString(htmlContent));
                    //! [NavigateToString]
                }
                //! [IsScriptEnabled]
                // Changes to settings will apply at the next navigation, which includes the
                // navigation after a NavigationStarting event.  We can use this to change
                // settings according to what site we're visiting.
                if (ShouldBlockScriptForUri(uri.get())) {
                    m_settings->put_IsScriptEnabled(FALSE);
                }
                else {
                    m_settings->put_IsScriptEnabled(m_isScriptEnabled);
                }
                //! [IsScriptEnabled]
                //! [UserAgent]
                if (m_settings2) {
                    static const PCWSTR url_compare_example = L"fourthcoffee.com";
                    wil::unique_bstr domain = GetDomainOfUri(uri.get());
                    const wchar_t* domains = domain.get();

                    if (wcscmp(url_compare_example, domains) == 0) {
                        SetUserAgent(L"example_navigation_ua");
                    }
                }
                //! [UserAgent]
                // [NavigationKind]
                wil::com_ptr<ICoreWebView2NavigationStartingEventArgs3> args3;
                if (SUCCEEDED(args->QueryInterface(IID_PPV_ARGS(&args3)))) {
                    COREWEBVIEW2_NAVIGATION_KIND kind =
                        COREWEBVIEW2_NAVIGATION_KIND_NEW_DOCUMENT;
                    CHECK_FAILURE(args3->get_NavigationKind(&kind));
                }
                // ! [NavigationKind]
                return S_OK;
            })
            .Get(),
        &m_navigationStartingToken));
    //! [NavigationStarting]

    //! [FrameNavigationStarting]
    // Register a handler for the FrameNavigationStarting event.
    // This handler will prevent a frame from navigating to a blocked domain.
    CHECK_FAILURE(m_webView->add_FrameNavigationStarting(
        Callback<ICoreWebView2NavigationStartingEventHandler>(
            [this](ICoreWebView2* sender, ICoreWebView2NavigationStartingEventArgs* args)
                -> HRESULT {
                wil::unique_cotaskmem_string uri;
                CHECK_FAILURE(args->get_Uri(&uri));

                if (ShouldBlockUri(uri.get())) {
                    CHECK_FAILURE(args->put_Cancel(true));
                }
                return S_OK;
            })
            .Get(),
        &m_frameNavigationStartingToken));
    //! [FrameNavigationStarting]

    //! [PermissionRequested0]
    // Register a handler for the PermissionRequested event.
    // This handler prompts the user to allow or deny the request, and remembers
    // the user's choice for later.
    CHECK_FAILURE(m_webView->add_PermissionRequested(
        Callback<ICoreWebView2PermissionRequestedEventHandler>(this, &SettingsComponent::OnPermissionRequested).Get(),
        &m_permissionRequestedToken));
    //! [PermissionRequested0]
}

//! [PermissionRequested1]
HRESULT SettingsComponent::OnPermissionRequested(ICoreWebView2* sender, ICoreWebView2PermissionRequestedEventArgs* args) {
    // Obtain a deferral for the event so that the CoreWebView2
    // doesn't examine the properties we set on the event args until
    // after we call the Complete method asynchronously later.
    wil::com_ptr<ICoreWebView2Deferral> deferral;
    CHECK_FAILURE(args->GetDeferral(&deferral));

    // Do not save state to the profile so that the PermissionRequested event is
    // always raised and the app is in control of all permission requests. In
    // this example, the app listens to all requests and caches permission on
    // its own to decide whether to show custom UI to the user.
    wil::com_ptr<ICoreWebView2PermissionRequestedEventArgs3> extended_args;
    CHECK_FAILURE(args->QueryInterface(IID_PPV_ARGS(&extended_args)));
    CHECK_FAILURE(extended_args->put_SavesInProfile(FALSE));

    wil::unique_cotaskmem_string uri;
    COREWEBVIEW2_PERMISSION_KIND kind = COREWEBVIEW2_PERMISSION_KIND_UNKNOWN_PERMISSION;
    BOOL userInitiated = FALSE;
    CHECK_FAILURE(args->get_Uri(&uri));
    CHECK_FAILURE(args->get_PermissionKind(&kind));
    CHECK_FAILURE(args->get_IsUserInitiated(&userInitiated));

    // Give all permission
    CHECK_FAILURE(args->put_State(COREWEBVIEW2_PERMISSION_STATE_ALLOW));
    CHECK_FAILURE(deferral->Complete());

    return S_OK;
}
//! [PermissionRequested1]

void SettingsComponent::ChangeBlockedSites(const std::vector<std::wstring>& sites) {
    m_blockedSites = sites;
}

bool SettingsComponent::ShouldBlockUri(PWSTR uri) {
    wil::unique_bstr domain = GetDomainOfUri(uri);

    for (auto site = m_blockedSites.begin(); site != m_blockedSites.end(); site++) {
        if (wcscmp(site->c_str(), domain.get()) == 0) {
            return true;
        }
    }
    return false;
}

// Decide whether a website should have script disabled.  Since we're only using this
// for sample code and we don't actually want to break any websites, just return false.
bool SettingsComponent::ShouldBlockScriptForUri(PWSTR uri) {
    return false;
}

// Turn on or off image blocking by adding or removing a WebResourceRequested handler
// which selectively intercepts requests for images.
void SettingsComponent::SetBlockImages(bool blockImages) {
    if (blockImages != m_blockImages) {
        m_blockImages = blockImages;

        //! [WebResourceRequested0]
        if (m_blockImages) {
            m_webView->AddWebResourceRequestedFilter(L"*", COREWEBVIEW2_WEB_RESOURCE_CONTEXT_IMAGE);
            CHECK_FAILURE(m_webView->add_WebResourceRequested(
                Callback<ICoreWebView2WebResourceRequestedEventHandler>(
                    [this](
                        ICoreWebView2* sender, ICoreWebView2WebResourceRequestedEventArgs* args) {
                        COREWEBVIEW2_WEB_RESOURCE_CONTEXT resourceContext;
                        CHECK_FAILURE(args->get_ResourceContext(&resourceContext));
                        // Ensure that the type is image
                        if (resourceContext != COREWEBVIEW2_WEB_RESOURCE_CONTEXT_IMAGE) {
                            return E_INVALIDARG;
                        }
                        // Override the response with an empty one to block the image.
                        // If put_Response is not called, the request will
                        // continue as normal.
                        wil::com_ptr<ICoreWebView2WebResourceResponse> response;
                        wil::com_ptr<ICoreWebView2Environment> environment;
                        wil::com_ptr<ICoreWebView2_2> webview2;
                        CHECK_FAILURE(m_webView->QueryInterface(IID_PPV_ARGS(&webview2)));
                        CHECK_FAILURE(webview2->get_Environment(&environment));
                        CHECK_FAILURE(environment->CreateWebResourceResponse(nullptr, 403 /*NoContent*/, L"Blocked", L"Content-Type: image/jpeg", &response));
                        CHECK_FAILURE(args->put_Response(response.get()));
                        return S_OK;
                    })
                    .Get(),
                &m_webResourceRequestedTokenForImageBlocking));
        }
        else {
            CHECK_FAILURE(m_webView->remove_WebResourceRequested(
                m_webResourceRequestedTokenForImageBlocking));
        }
        //! [WebResourceRequested0]
    }
}

// Turn on or off image replacing by adding or removing a WebResourceRequested handler
// which selectively intercepts requests for images. It will replace all images with another
// image.
void SettingsComponent::SetReplaceImages(bool replaceImages) {
    if (replaceImages != m_replaceImages) {
        m_replaceImages = replaceImages;
        //! [WebResourceRequested1]
        if (m_replaceImages) {
            m_webView->AddWebResourceRequestedFilter(L"*", COREWEBVIEW2_WEB_RESOURCE_CONTEXT_IMAGE);
            CHECK_FAILURE(m_webView->add_WebResourceRequested(
                Callback<ICoreWebView2WebResourceRequestedEventHandler>(
                    [this](ICoreWebView2* sender, ICoreWebView2WebResourceRequestedEventArgs* args) {
                        COREWEBVIEW2_WEB_RESOURCE_CONTEXT resourceContext;
                        CHECK_FAILURE(args->get_ResourceContext(&resourceContext));
                        // Ensure that the type is image
                        if (resourceContext != COREWEBVIEW2_WEB_RESOURCE_CONTEXT_IMAGE) {
                            return E_INVALIDARG;
                        }
                        // Override the response with an another image.
                        // If put_Response is not called, the request will
                        // continue as normal.
                        // It's not required for this scenario, but generally you should examine
                        // relevant HTTP request headers just like an HTTP server would do when
                        // producing a response stream.
                        wil::com_ptr<IStream> stream;
                        CHECK_FAILURE(SHCreateStreamOnFileEx(L"assets/EdgeWebView2-80.jpg", STGM_READ, FILE_ATTRIBUTE_NORMAL, FALSE, nullptr, &stream));
                        wil::com_ptr<ICoreWebView2WebResourceResponse> response;
                        wil::com_ptr<ICoreWebView2Environment> environment;
                        wil::com_ptr<ICoreWebView2_2> webview2;
                        CHECK_FAILURE(m_webView->QueryInterface(IID_PPV_ARGS(&webview2)));
                        CHECK_FAILURE(webview2->get_Environment(&environment));
                        CHECK_FAILURE(environment->CreateWebResourceResponse(stream.get(), 200, L"OK", L"Content-Type: image/jpeg", &response));
                        CHECK_FAILURE(args->put_Response(response.get()));
                        return S_OK;
                    })
                    .Get(),
                &m_webResourceRequestedTokenForImageReplacing));
        }
        else {
            CHECK_FAILURE(m_webView->remove_WebResourceRequested(m_webResourceRequestedTokenForImageReplacing));
        }
        //! [WebResourceRequested1]
    }
}

void SettingsComponent::SetUserAgent(const std::wstring& userAgent) {
    if (m_settings2) {
        m_overridingUserAgent = userAgent;
        if (m_overridingUserAgent.empty()) {
            m_changeUserAgent = false;
        }
        else {
            m_changeUserAgent = true;
            CHECK_FAILURE(m_settings2->put_UserAgent(m_overridingUserAgent.c_str()));
        }
    }
}

// Turn off client certificate selection dialog using ClientCertificateRequested event handler
// that disables the dialog. This example hides the default client certificate dialog and
// always chooses the last certificate without prompting the user.
//! [ClientCertificateRequested1]
void SettingsComponent::EnableCustomClientCertificateSelection() {
    if (m_webView2_5) {
        if (m_ClientCertificateRequestedToken.value == 0) {
            CHECK_FAILURE(m_webView2_5->add_ClientCertificateRequested(
                Callback<ICoreWebView2ClientCertificateRequestedEventHandler>(
                    [this](ICoreWebView2* sender, ICoreWebView2ClientCertificateRequestedEventArgs* args) {
                        wil::com_ptr<ICoreWebView2ClientCertificateCollection> certificateCollection;
                        CHECK_FAILURE(args->get_MutuallyTrustedCertificates(&certificateCollection));

                        UINT certificateCollectionCount = 0;
                        CHECK_FAILURE(certificateCollection->get_Count(&certificateCollectionCount));
                        wil::com_ptr<ICoreWebView2ClientCertificate> certificate = nullptr;

                        if (certificateCollectionCount > 0) {
                            // There is no significance to the order, picking a certificate arbitrarily.
                            CHECK_FAILURE(certificateCollection->GetValueAtIndex(certificateCollectionCount - 1, &certificate));
                            // Continue with the selected certificate to respond to the server.
                            CHECK_FAILURE(args->put_SelectedCertificate(certificate.get()));
                            CHECK_FAILURE(args->put_Handled(TRUE));
                        }
                        else {
                            // Continue without a certificate to respond to the server if certificate collection is empty.
                            CHECK_FAILURE(args->put_Handled(TRUE));
                        }
                        return S_OK;
                    })
                    .Get(),
                &m_ClientCertificateRequestedToken));
        }
        else {
            CHECK_FAILURE(m_webView2_5->remove_ClientCertificateRequested(m_ClientCertificateRequestedToken));
            m_ClientCertificateRequestedToken.value = 0;
        }
    }
}
//! [ClientCertificateRequested1]

// Function to validate the server certificate for untrusted root or self-signed certificate.
// You may also choose to defer server certificate validation.
static bool ValidateServerCertificate(ICoreWebView2Certificate* certificate) {
    // You may want to validate certificates in different ways depending on your app and
    // scenario. One way might be the following:
    // First, get the list of host app trusted certificates and its thumbprint.
    //
    // Then get the last chain element using
    // `ICoreWebView2Certificate::get_PemEncodedIssuerCertificateChain` that contains the raw
    // data of the untrusted root CA/self-signed certificate. Get the untrusted root CA/self
    // signed certificate thumbprint from the raw certificate data and validate the thumbprint
    // against the host app trusted certificate list.
    //
    // Finally, return true if it exists in the host app's certificate trusted list, or
    // otherwise return false.
    return true;
}

//! [ServerCertificateErrorDetected1]
// When WebView2 doesn't trust a TLS certificate but host app does, this example bypasses
// the default TLS interstitial page using the ServerCertificateErrorDetected event handler and
// continues the request to a server. Otherwise, cancel the request.
void SettingsComponent::ToggleCustomServerCertificateSupport() {
    if (m_webView2_14) {
        if (m_ServerCertificateErrorToken.value == 0) {
            CHECK_FAILURE(m_webView2_14->add_ServerCertificateErrorDetected(
                Callback<ICoreWebView2ServerCertificateErrorDetectedEventHandler>(
                    [this](ICoreWebView2* sender, ICoreWebView2ServerCertificateErrorDetectedEventArgs* args) {
                        COREWEBVIEW2_WEB_ERROR_STATUS errorStatus;
                        CHECK_FAILURE(args->get_ErrorStatus(&errorStatus));

                        wil::com_ptr<ICoreWebView2Certificate> certificate = nullptr;
                        CHECK_FAILURE(args->get_ServerCertificate(&certificate));

                        // Continues the request to a server with a TLS certificate if the error
                        // status is of type
                        // `COREWEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_IS_INVALID` and trusted by
                        // the host app.
                        if (errorStatus == COREWEBVIEW2_WEB_ERROR_STATUS_CERTIFICATE_IS_INVALID && ValidateServerCertificate(certificate.get())) {
                            CHECK_FAILURE(args->put_Action(COREWEBVIEW2_SERVER_CERTIFICATE_ERROR_ACTION_ALWAYS_ALLOW));
                        }
                        else {
                            // Cancel the request for other TLS certificate error types or if
                            // untrusted by the host app.
                            CHECK_FAILURE(args->put_Action(COREWEBVIEW2_SERVER_CERTIFICATE_ERROR_ACTION_CANCEL));
                        }
                        return S_OK;
                    })
                    .Get(),
                &m_ServerCertificateErrorToken));
        }
        else {
            CHECK_FAILURE(m_webView2_14->remove_ServerCertificateErrorDetected(m_ServerCertificateErrorToken));
            m_ServerCertificateErrorToken.value = 0;
        }
    }
    else {
        FeatureNotAvailable();
    }
}

//! [ServerCertificateErrorDetected1]

HRESULT SettingsComponent::EnableDefaultContextMenu(bool enabled) {
    HRESULT hr = m_settings->put_AreDefaultContextMenusEnabled(enabled ? TRUE : FALSE);
    return hr;
}

//! [SetTrackingPreventionLevel]
void SettingsComponent::SetTrackingPreventionLevel(COREWEBVIEW2_TRACKING_PREVENTION_LEVEL value) {
    wil::com_ptr<ICoreWebView2_13> webView2_13;
    webView2_13 = m_webView.try_query<ICoreWebView2_13>();

    if (webView2_13) {
        wil::com_ptr<ICoreWebView2Profile> profile;
        CHECK_FAILURE(webView2_13->get_Profile(&profile));

        auto profile3 = profile.try_query<ICoreWebView2Profile3>();
        if (profile3) {
            CHECK_FAILURE(profile3->put_PreferredTrackingPreventionLevel(value));
            MessageBox(
                nullptr, L"Tracking prevention level is set successfully",
                L"Tracking Prevention Level", MB_OK);
        }
    }
}

HRESULT SettingsComponent::SetBuiltInErrorPageEnabled(bool enabled) {
    BOOL oldEnabled = FALSE;
    if (SUCCEEDED(m_settings->get_IsBuiltInErrorPageEnabled(&oldEnabled)) && ((!!oldEnabled) == enabled)) {
        return S_OK;
    }

    return m_settings->put_IsBuiltInErrorPageEnabled(enabled ? TRUE : FALSE);
}

bool SettingsComponent::IsBuiltInErrorPageEnabled() {
    BOOL enabled = TRUE;
    CHECK_FAILURE(m_settings->get_IsBuiltInErrorPageEnabled(&enabled));
    return !!enabled;
}

//! [SetTrackingPreventionLevel]

SettingsComponent::~SettingsComponent() {
    m_webView->remove_NavigationStarting(m_navigationStartingToken);
    m_webView->remove_FrameNavigationStarting(m_frameNavigationStartingToken);
    m_webView->remove_WebResourceRequested(m_webResourceRequestedTokenForImageBlocking);
    m_webView->remove_PermissionRequested(m_permissionRequestedToken);
}

// Take advantage of urlmon's URI library to parse a URI
wil::unique_bstr GetDomainOfUri(PWSTR uri) {
    wil::com_ptr<IUri> uriObject;
    HRESULT hr = CreateUri(uri, Uri_CREATE_CANONICALIZE | Uri_CREATE_NO_DECODE_EXTRA_INFO, 0, &uriObject);
    if (FAILED(hr)) {
        return nullptr;
    }
    wil::unique_bstr domain;
    uriObject->GetHost(&domain);
    return domain;
}
