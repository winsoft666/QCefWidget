#ifndef CONTROL_COMPONENT_H_
#define CONTROL_COMPONENT_H_
#pragma once
#include "stdafx.h"
#include <string>
#include "ComponentBase.h"
#include <WebView2.h>
#include "QWebView2Options.h"

class ControlComponent : public ComponentBase {
   public:
    ControlComponent(QWebViewWebView2Impl* d);
    ~ControlComponent() override;

   private:
    wil::com_ptr<ICoreWebView2Controller> m_controller;
    wil::com_ptr<ICoreWebView2> m_webView;

    EventRegistrationToken m_navigationStartingToken = {};
    EventRegistrationToken m_sourceChangedToken = {};
    EventRegistrationToken m_historyChangedToken = {};
    EventRegistrationToken m_navigationCompletedToken = {};
    EventRegistrationToken m_moveFocusRequestedToken = {};
    EventRegistrationToken m_acceleratorKeyPressedToken = {};
    EventRegistrationToken m_unhandledKeyPressedToken = {};
    EventRegistrationToken m_frameNavigationCompletedToken = {};
};
#endif  // !CONTROL_COMPONENT_H_
