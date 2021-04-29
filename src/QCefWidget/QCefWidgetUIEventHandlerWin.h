#ifndef UI_EVENT_HANDLER_WIN_H_
#define UI_EVENT_HANDLER_WIN_H_
#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <include/cef_client.h>
#include <windowsx.h>
#include "CefBrowserApp/QCefBrowserHandler.h"
#include "QCefIMEHandlerWin.h"

class QCefWidgetUIEventHandlerWin {
 public:
  QCefWidgetUIEventHandlerWin(HWND h,
                              CefRefPtr<CefBrowser> pCefBrowser,
                              CefRefPtr<QCefBrowserHandler> pBrowserHandler);
  ~QCefWidgetUIEventHandlerWin();

  void OnSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  void OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  void OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  void OnTouchEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  void OnFocusEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  void OnIMEEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  void
  OnCaptureLostEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  void OnImeCompositionRangeChanged(
      CefRefPtr<CefBrowser> browser,
      const CefRange& selection_range,
      const CefRenderHandler::RectList& character_bounds);

  void setDeviceScaleFactor(float factor);
  bool IsOverPopupWidget(int x, int y) const;
  int GetPopupXOffset() const;
  int GetPopupYOffset() const;
  void ApplyPopupOffset(int& x, int& y) const;

 private:
  bool IsKeyDown(WPARAM wParam);
  int GetCefMouseModifiers(WPARAM wParam);
  int GetCefKeyboardModifiers(WPARAM wParam, LPARAM lParam);
  void OnIMEComposition(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  void OnIMECancelCompositionEvent();

  CefRefPtr<CefBrowserHost> browserHost();

 private:
  HWND hwnd_;
  CefRefPtr<CefBrowser> pCefBrowser_;
  CefRefPtr<QCefBrowserHandler> pBrowserHandler_;
  float deviceScaleFactor_;

  // Mouse state tracking.
  POINT lastMousePos_;
  POINT currentMousePos_;
  bool mouseRotation_;
  bool mouseTracking_;
  int lastClickX_;
  int lastClickY_;
  CefBrowserHost::MouseButtonType lastClickButton_;
  int lastClickCount_;
  double lastClickTime_;
  bool lastMouseDownOnView_;

  std::unique_ptr<QCefIMEHandlerWin> imeHandler_;
};

#endif
#endif  // !UI_EVENT_HANDLER_WIN_H_