#ifndef NO_CLIENT_REGION_SUPPORT_COMPONENT_H_
#define NO_CLIENT_REGION_SUPPORT_COMPONENT_H_
#if defined(_MSVC_) && _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include "ComponentBase.h"
#include <WebView2.h>

// WebView2 Win32 version at least 1.0.2420.47
//
class NonClientRegionSupportComponent : public ComponentBase {
   public:
    NonClientRegionSupportComponent(QWebViewWebView2Impl* d);
    ~NonClientRegionSupportComponent();

    void AddChangeListener();

   private:
    wil::com_ptr<ICoreWebView2> m_webView;
    wil::com_ptr<ICoreWebView2Settings> m_settings;

    wil::com_ptr<ICoreWebView2Settings9> m_settings9;
    wil::com_ptr<ICoreWebView2CompositionController4> m_compController4;

    EventRegistrationToken m_navigationStartingToken = {};
    EventRegistrationToken m_ContentLoadingToken = {};
    EventRegistrationToken m_nonClientRegionChanged = {};
};

#endif  // !NO_CLIENT_REGION_SUPPORT_COMPONENT_H_