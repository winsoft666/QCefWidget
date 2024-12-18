#ifndef COMPONENT_BASE_H_
#define COMPONENT_BASE_H_
#if defined(_MSVC_) && _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#include <windows.h>

class QWebViewWebView2Impl;
class ComponentBase {
   public:
    ComponentBase(QWebViewWebView2Impl* d) :
        d_(d) {
    }

    // *result defaults to 0
    virtual bool HandleWindowMessage(
        HWND hWnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam,
        LRESULT* result) {
        return false;
    }
    virtual ~ComponentBase() {}

    QWebViewWebView2Impl* GetWebVew2Imp() {
        return d_;
    }

   protected:
    QWebViewWebView2Impl* d_ = nullptr;
};
#endif // !COMPONENT_BASE_H_
