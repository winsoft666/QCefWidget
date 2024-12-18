#ifndef HOST_OBJECT_COMPONENT_H_
#define HOST_OBJECT_COMPONENT_H_
#if defined(_MSVC_) && _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <string>
#include "ComponentBase.h"
#include "HostObjectSimple_h.h"
#include <WebView2.h>
#include "QWebView2Options.h"

class HostObjectComponent : public ComponentBase {
   public:
    HostObjectComponent(QWebViewWebView2Impl* d);
    ~HostObjectComponent();

    ScriptCallCallback GetScriptCallNotify();
   private:
    wil::com_ptr<ICoreWebView2> m_webView;
    wil::com_ptr<HostObjectSimple> m_hostObject;

    EventRegistrationToken m_navigationStartingToken = {};
    EventRegistrationToken m_frameCreatedToken = {};
};

#endif  // !HOST_OBJECT_COMPONENT_H_
