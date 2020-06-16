#include "UIEventHandlerWin.h"
#include <QDebug>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
namespace {
int LogicalToDevice(int value, float device_scale_factor) {
  float scaled_val = static_cast<float>(value) * device_scale_factor;
  return static_cast<int>(std::floor(scaled_val));
}

CefRect LogicalToDevice(const CefRect &value, float device_scale_factor) {
  return CefRect(LogicalToDevice(value.x, device_scale_factor),
                 LogicalToDevice(value.y, device_scale_factor),
                 LogicalToDevice(value.width, device_scale_factor),
                 LogicalToDevice(value.height, device_scale_factor));
}

int DeviceToLogical(int value, float device_scale_factor) {
  float scaled_val = static_cast<float>(value) / device_scale_factor;
  return static_cast<int>(std::floor(scaled_val));
}

void DeviceToLogical(CefMouseEvent &value, float device_scale_factor) {
  value.x = DeviceToLogical(value.x, device_scale_factor);
  value.y = DeviceToLogical(value.y, device_scale_factor);
}
} // namespace

UIEventHandlerWin::UIEventHandlerWin(HWND h, CefRefPtr<CefBrowser> pCefBrowser)
    : deviceScaleFactor_(1.0f)
    , hwnd_(h)
    , pCefBrowser_(pCefBrowser)
    , last_mouse_pos_()
    , current_mouse_pos_()
    , mouse_rotation_(false)
    , mouse_tracking_(false)
    , last_click_x_(0)
    , last_click_y_(0)
    , last_click_button_(MBT_LEFT)
    , last_click_count_(0)
    , last_click_time_(0)
    , last_mouse_down_on_view_(false) {
  ime_handler_ = std::make_unique<IMEHandlerWin>(hwnd_);
}

UIEventHandlerWin::~UIEventHandlerWin() {}

void UIEventHandlerWin::OnSize(UINT message, WPARAM wparam, LPARAM lparam) {
  CefRefPtr<CefBrowserHost> host = browserHost();
  if (host) {
    host->WasResized();
  }
}

bool UIEventHandlerWin::IsKeyDown(WPARAM wparam) { return (GetKeyState(wparam) & 0x8000) != 0; }

int UIEventHandlerWin::GetCefMouseModifiers(WPARAM wparam) {
  int modifiers = 0;
  if (wparam & MK_CONTROL)
    modifiers |= EVENTFLAG_CONTROL_DOWN;
  if (wparam & MK_SHIFT)
    modifiers |= EVENTFLAG_SHIFT_DOWN;
  if (IsKeyDown(VK_MENU))
    modifiers |= EVENTFLAG_ALT_DOWN;
  if (wparam & MK_LBUTTON)
    modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
  if (wparam & MK_MBUTTON)
    modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
  if (wparam & MK_RBUTTON)
    modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;

  // Low bit set from GetKeyState indicates "toggled".
  if (::GetKeyState(VK_NUMLOCK) & 1)
    modifiers |= EVENTFLAG_NUM_LOCK_ON;
  if (::GetKeyState(VK_CAPITAL) & 1)
    modifiers |= EVENTFLAG_CAPS_LOCK_ON;
  return modifiers;
}

int UIEventHandlerWin::GetCefKeyboardModifiers(WPARAM wparam, LPARAM lparam) {
  int modifiers = 0;
  if (IsKeyDown(VK_SHIFT))
    modifiers |= EVENTFLAG_SHIFT_DOWN;
  if (IsKeyDown(VK_CONTROL))
    modifiers |= EVENTFLAG_CONTROL_DOWN;
  if (IsKeyDown(VK_MENU))
    modifiers |= EVENTFLAG_ALT_DOWN;

  // Low bit set from GetKeyState indicates "toggled".
  if (::GetKeyState(VK_NUMLOCK) & 1)
    modifiers |= EVENTFLAG_NUM_LOCK_ON;
  if (::GetKeyState(VK_CAPITAL) & 1)
    modifiers |= EVENTFLAG_CAPS_LOCK_ON;

  switch (wparam) {
  case VK_RETURN:
    if ((lparam >> 16) & KF_EXTENDED)
      modifiers |= EVENTFLAG_IS_KEY_PAD;
    break;
  case VK_INSERT:
  case VK_DELETE:
  case VK_HOME:
  case VK_END:
  case VK_PRIOR:
  case VK_NEXT:
  case VK_UP:
  case VK_DOWN:
  case VK_LEFT:
  case VK_RIGHT:
    if (!((lparam >> 16) & KF_EXTENDED))
      modifiers |= EVENTFLAG_IS_KEY_PAD;
    break;
  case VK_NUMLOCK:
  case VK_NUMPAD0:
  case VK_NUMPAD1:
  case VK_NUMPAD2:
  case VK_NUMPAD3:
  case VK_NUMPAD4:
  case VK_NUMPAD5:
  case VK_NUMPAD6:
  case VK_NUMPAD7:
  case VK_NUMPAD8:
  case VK_NUMPAD9:
  case VK_DIVIDE:
  case VK_MULTIPLY:
  case VK_SUBTRACT:
  case VK_ADD:
  case VK_DECIMAL:
  case VK_CLEAR:
    modifiers |= EVENTFLAG_IS_KEY_PAD;
    break;
  case VK_SHIFT:
    if (IsKeyDown(VK_LSHIFT))
      modifiers |= EVENTFLAG_IS_LEFT;
    else if (IsKeyDown(VK_RSHIFT))
      modifiers |= EVENTFLAG_IS_RIGHT;
    break;
  case VK_CONTROL:
    if (IsKeyDown(VK_LCONTROL))
      modifiers |= EVENTFLAG_IS_LEFT;
    else if (IsKeyDown(VK_RCONTROL))
      modifiers |= EVENTFLAG_IS_RIGHT;
    break;
  case VK_MENU:
    if (IsKeyDown(VK_LMENU))
      modifiers |= EVENTFLAG_IS_LEFT;
    else if (IsKeyDown(VK_RMENU))
      modifiers |= EVENTFLAG_IS_RIGHT;
    break;
  case VK_LWIN:
    modifiers |= EVENTFLAG_IS_LEFT;
    break;
  case VK_RWIN:
    modifiers |= EVENTFLAG_IS_RIGHT;
    break;
  }
  return modifiers;
}

void UIEventHandlerWin::OnIMEComposition(UINT message, WPARAM wParam, LPARAM lParam) {
  CefRefPtr<CefBrowserHost> host = browserHost();

  if (host && ime_handler_) {
    CefString cTextStr;
    if (ime_handler_->GetResult(lParam, cTextStr)) {
      host->ImeCommitText(cTextStr, CefRange(UINT32_MAX, UINT32_MAX), 0);
      ime_handler_->ResetComposition();
    }

    std::vector<CefCompositionUnderline> underlines;
    int composition_start = 0;

    if (ime_handler_->GetComposition(lParam, cTextStr, underlines, composition_start)) {
      host->ImeSetComposition(
          cTextStr, underlines, CefRange(UINT32_MAX, UINT32_MAX),
          CefRange(composition_start, static_cast<int>(composition_start + cTextStr.length())));
      ime_handler_->UpdateCaretPosition(composition_start - 1);
    }
    else {
      OnIMECancelCompositionEvent();
    }
  }
}

void UIEventHandlerWin::OnIMECancelCompositionEvent() {
  CefRefPtr<CefBrowserHost> host = browserHost();
  if (host && ime_handler_) {
    host->ImeCancelComposition();
    ime_handler_->ResetComposition();
    ime_handler_->DestroyImeWindow();
  }
}

CefRefPtr<CefBrowserHost> UIEventHandlerWin::browserHost() {
  if (!pCefBrowser_)
    return nullptr;

  CefRefPtr<CefBrowserHost> host = pCefBrowser_->GetHost();
  return host;
}

void UIEventHandlerWin::OnKeyboardEvent(UINT message, WPARAM wparam, LPARAM lparam) {
  CefRefPtr<CefBrowserHost> host = browserHost();
  if (!host)
    return;

  CefKeyEvent event;
  event.windows_key_code = wparam;
  event.native_key_code = lparam;
  event.is_system_key = message == WM_SYSCHAR || message == WM_SYSKEYDOWN || message == WM_SYSKEYUP;

  if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN)
    event.type = KEYEVENT_RAWKEYDOWN;
  else if (message == WM_KEYUP || message == WM_SYSKEYUP)
    event.type = KEYEVENT_KEYUP;
  else
    event.type = KEYEVENT_CHAR;
  event.modifiers = GetCefKeyboardModifiers(wparam, lparam);

  host->SendKeyEvent(event);
}

// Helper funtion to check if it is Windows8 or greater.
// https://msdn.microsoft.com/en-us/library/ms724833(v=vs.85).aspx
inline BOOL IsWindows_8_Or_Newer() {
  OSVERSIONINFOEX osvi = {0};
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  osvi.dwMajorVersion = 6;
  osvi.dwMinorVersion = 2;
  DWORDLONG dwlConditionMask = 0;
  VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
  VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
  return ::VerifyVersionInfo(&osvi, VER_MAJORVERSION | VER_MINORVERSION, dwlConditionMask);
}

// Helper function to detect mouse messages coming from emulation of touch
// events. These should be ignored.
static bool IsMouseEventFromTouch(UINT message) {
#define MOUSEEVENTF_FROMTOUCH 0xFF515700
  return (message >= WM_MOUSEFIRST) && (message <= WM_MOUSELAST) &&
         (GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH;
}

void UIEventHandlerWin::OnMouseEvent(UINT message, WPARAM wParam, LPARAM lParam) {
  CefRefPtr<CefBrowserHost> host = browserHost();
  if (!host)
    return;

  if (IsMouseEventFromTouch(message))
    return;

  LONG currentTime = 0;
  bool cancelPreviousClick = false;

  if (message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN || message == WM_MBUTTONDOWN ||
      message == WM_MOUSEMOVE || message == WM_MOUSELEAVE) {
    currentTime = GetMessageTime();
    int x = GET_X_LPARAM(lParam);
    int y = GET_Y_LPARAM(lParam);
    cancelPreviousClick = (abs(last_click_x_ - x) > (GetSystemMetrics(SM_CXDOUBLECLK) / 2)) ||
                          (abs(last_click_y_ - y) > (GetSystemMetrics(SM_CYDOUBLECLK) / 2)) ||
                          ((currentTime - last_click_time_) > GetDoubleClickTime());
    if (cancelPreviousClick && (message == WM_MOUSEMOVE || message == WM_MOUSELEAVE)) {
      last_click_count_ = 0;
      last_click_x_ = 0;
      last_click_y_ = 0;
      last_click_time_ = 0;
    }
  }

  switch (message) {
  case WM_LBUTTONDOWN:
  case WM_RBUTTONDOWN:
  case WM_MBUTTONDOWN: {
    ::SetCapture(hwnd_);
    ::SetFocus(hwnd_);
    int x = GET_X_LPARAM(lParam);
    int y = GET_Y_LPARAM(lParam);
    if (wParam & MK_SHIFT) {
      last_mouse_pos_.x = current_mouse_pos_.x = x;
      last_mouse_pos_.y = current_mouse_pos_.y = y;
      mouse_rotation_ = true;
    }
    else {
      CefBrowserHost::MouseButtonType btnType =
          (message == WM_LBUTTONDOWN ? MBT_LEFT
                                     : (message == WM_RBUTTONDOWN ? MBT_RIGHT : MBT_MIDDLE));
      if (!cancelPreviousClick && (btnType == last_click_button_)) {
        ++last_click_count_;
      }
      else {
        last_click_count_ = 1;
        last_click_x_ = x;
        last_click_y_ = y;
      }
      last_click_time_ = currentTime;
      last_click_button_ = btnType;

      CefMouseEvent mouse_event;
      mouse_event.x = x;
      mouse_event.y = y;

      //last_mouse_down_on_view_ = !IsOverPopupWidget(x, y);
      //ApplyPopupOffset(mouse_event.x, mouse_event.y);
      DeviceToLogical(mouse_event, deviceScaleFactor_);
      mouse_event.modifiers = GetCefMouseModifiers(wParam);
      host->SendMouseClickEvent(mouse_event, btnType, false, last_click_count_);
    }
  } break;

  case WM_LBUTTONUP:
  case WM_RBUTTONUP:
  case WM_MBUTTONUP:
    if (GetCapture() == hwnd_)
      ReleaseCapture();
    if (mouse_rotation_) {
      mouse_rotation_ = false;
      //render_handler_->SetSpin(0, 0);
    }
    else {
      if (last_click_time_ == 0) {
        // winsoft666:
        // double-click window title will only receive a WM_MBUTTONUP message but not WM_MBUTTONDOWN.
        break;
      }
      int x = GET_X_LPARAM(lParam);
      int y = GET_Y_LPARAM(lParam);
      CefBrowserHost::MouseButtonType btnType =
          (message == WM_LBUTTONUP ? MBT_LEFT : (message == WM_RBUTTONUP ? MBT_RIGHT : MBT_MIDDLE));
      CefMouseEvent mouse_event;
      mouse_event.x = x;
      mouse_event.y = y;

      //if (last_mouse_down_on_view_ && IsOverPopupWidget(x, y) &&
      //        (GetPopupXOffset() || GetPopupYOffset())) {
      //    break;
      //}
      //ApplyPopupOffset(mouse_event.x, mouse_event.y);
      DeviceToLogical(mouse_event, deviceScaleFactor_);
      mouse_event.modifiers = GetCefMouseModifiers(wParam);
      host->SendMouseClickEvent(mouse_event, btnType, true, last_click_count_);
    }
    break;

  case WM_MOUSEMOVE: {
    int x = GET_X_LPARAM(lParam);
    int y = GET_Y_LPARAM(lParam);
    if (mouse_rotation_) {
      // Apply rotation effect.
      current_mouse_pos_.x = x;
      current_mouse_pos_.y = y;
      //render_handler_->IncrementSpin(
      //    current_mouse_pos_.x - last_mouse_pos_.x,
      //    current_mouse_pos_.y - last_mouse_pos_.y);
      last_mouse_pos_.x = current_mouse_pos_.x;
      last_mouse_pos_.y = current_mouse_pos_.y;
    }
    else {
      if (!mouse_tracking_) {
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(TRACKMOUSEEVENT);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = hwnd_;
        TrackMouseEvent(&tme);
        mouse_tracking_ = true;
      }

      CefMouseEvent mouse_event;
      mouse_event.x = x;
      mouse_event.y = y;
      //ApplyPopupOffset(mouse_event.x, mouse_event.y);
      DeviceToLogical(mouse_event, deviceScaleFactor_);
      mouse_event.modifiers = GetCefMouseModifiers(wParam);
      host->SendMouseMoveEvent(mouse_event, false);
    }
    break;
  }

  case WM_MOUSELEAVE: {
    int x = GET_X_LPARAM(lParam);
    int y = GET_Y_LPARAM(lParam);

    if (mouse_tracking_) {
      // Stop tracking mouse leave.
      TRACKMOUSEEVENT tme;
      tme.cbSize = sizeof(TRACKMOUSEEVENT);
      tme.dwFlags = TME_LEAVE & TME_CANCEL;
      tme.hwndTrack = hwnd_;
      TrackMouseEvent(&tme);
      mouse_tracking_ = false;
    }

    CefMouseEvent mouse_event;
    mouse_event.x = x;
    mouse_event.y = y;
    DeviceToLogical(mouse_event, deviceScaleFactor_);
    mouse_event.modifiers = GetCefMouseModifiers(wParam);
    host->SendMouseMoveEvent(mouse_event, true);
  } break;

  case WM_MOUSEWHEEL:
    POINT screen_point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
    HWND scrolled_wnd = ::WindowFromPoint(screen_point);
    if (scrolled_wnd != hwnd_)
      break;

    ScreenToClient(hwnd_, &screen_point);
    int delta = GET_WHEEL_DELTA_WPARAM(wParam);

    CefMouseEvent mouse_event;
    mouse_event.x = screen_point.x;
    mouse_event.y = screen_point.y;
    //ApplyPopupOffset(mouse_event.x, mouse_event.y);
    DeviceToLogical(mouse_event, deviceScaleFactor_);
    mouse_event.modifiers = GetCefMouseModifiers(wParam);
    host->SendMouseWheelEvent(mouse_event, IsKeyDown(VK_SHIFT) ? delta : 0,
                              !IsKeyDown(VK_SHIFT) ? delta : 0);
    break;
  }
}

void UIEventHandlerWin::OnTouchEvent(UINT message, WPARAM wParam, LPARAM lParam) {
  CefRefPtr<CefBrowserHost> host = browserHost();
  if (!host)
    return;

  // Handle touch events on Windows.
  int num_points = LOWORD(wParam);
  // Chromium only supports upto 16 touch points.
  if (num_points < 0 || num_points > 16)
    return;
  std::unique_ptr<TOUCHINPUT[]> input(new TOUCHINPUT[num_points]);
  if (GetTouchInputInfo(reinterpret_cast<HTOUCHINPUT>(lParam), num_points, input.get(),
                        sizeof(TOUCHINPUT))) {
    CefTouchEvent touch_event;
    for (int i = 0; i < num_points; ++i) {
      POINT point;
      point.x = TOUCH_COORD_TO_PIXEL(input[i].x);
      point.y = TOUCH_COORD_TO_PIXEL(input[i].y);

      if (!IsWindows_8_Or_Newer()) {
        // Windows 7 sends touch events for touches in the non-client area,
        // whereas Windows 8 does not. In order to unify the behaviour, always
        // ignore touch events in the non-client area.
        LPARAM l_param_ht = MAKELPARAM(point.x, point.y);
        LRESULT hittest = SendMessage(hwnd_, WM_NCHITTEST, 0, l_param_ht);
        if (hittest != HTCLIENT)
          return;
      }

      ScreenToClient(hwnd_, &point);
      touch_event.x = DeviceToLogical(point.x, deviceScaleFactor_);
      touch_event.y = DeviceToLogical(point.y, deviceScaleFactor_);

      // Touch point identifier stays consistent in a touch contact sequence
      touch_event.id = input[i].dwID;

      if (input[i].dwFlags & TOUCHEVENTF_DOWN) {
        touch_event.type = CEF_TET_PRESSED;
      }
      else if (input[i].dwFlags & TOUCHEVENTF_MOVE) {
        touch_event.type = CEF_TET_MOVED;
      }
      else if (input[i].dwFlags & TOUCHEVENTF_UP) {
        touch_event.type = CEF_TET_RELEASED;
      }

      touch_event.radius_x = 0;
      touch_event.radius_y = 0;
      touch_event.rotation_angle = 0;
      touch_event.pressure = 0;
      touch_event.modifiers = 0;

      // Notify the browser of touch event
      host->SendTouchEvent(touch_event);
    }
    CloseTouchInputHandle(reinterpret_cast<HTOUCHINPUT>(lParam));
  }
}

void UIEventHandlerWin::OnFocusEvent(UINT message, WPARAM wParam, LPARAM lParam) {
  CefRefPtr<CefBrowserHost> host = browserHost();
  if (!host)
    return;

  if (message == WM_SETFOCUS) {
    host->SendFocusEvent(true);
  }
  else if (message == WM_KILLFOCUS) {
    host->SendFocusEvent(false);
  }
}

void UIEventHandlerWin::OnIMEEvent(UINT message, WPARAM wParam, LPARAM lParam) {
  if (message == WM_IME_SETCONTEXT) {
    qInfo() << "WM_IME_SETCONTEXT";
    // We handle the IME Composition Window ourselves (but let the IME Candidates
    // Window be handled by IME through DefWindowProc()), so clear the
    // ISC_SHOWUICOMPOSITIONWINDOW flag:
    lParam &= ~ISC_SHOWUICOMPOSITIONWINDOW;
    ::DefWindowProc(hwnd_, message, wParam, lParam);

    // Create Caret Window if required
    if (ime_handler_) {
      ime_handler_->CreateImeWindow();
      ime_handler_->MoveImeWindow();
    }
  }
  else if (message == WM_IME_STARTCOMPOSITION) {
    qInfo() << "WM_IME_STARTCOMPOSITION";
    if (ime_handler_) {
      ime_handler_->CreateImeWindow();
      ime_handler_->MoveImeWindow();
      ime_handler_->ResetComposition();
    }
  }
  else if (message == WM_IME_COMPOSITION) {
    qInfo() << "WM_IME_COMPOSITION";
    OnIMEComposition(message, wParam, lParam);
  }
  else if (message == WM_IME_ENDCOMPOSITION) {
    qInfo() << "WM_IME_ENDCOMPOSITION";

    OnIMECancelCompositionEvent();
  }
}

void UIEventHandlerWin::OnCaptureLostEvent(UINT message, WPARAM wParam, LPARAM lParam) {
  if (mouse_rotation_)
    return;
  CefRefPtr<CefBrowserHost> host = browserHost();
  if (host)
    host->SendCaptureLostEvent();
}

void UIEventHandlerWin::OnImeCompositionRangeChanged(
    CefRefPtr<CefBrowser> browser, const CefRange &selection_range,
    const CefRenderHandler::RectList &character_bounds) {
  if (ime_handler_) {
    // Convert from view coordinates to device coordinates.
    CefRenderHandler::RectList device_bounds;
    CefRenderHandler::RectList::const_iterator it = character_bounds.begin();
    for (; it != character_bounds.end(); ++it) {
      device_bounds.push_back(LogicalToDevice(*it, deviceScaleFactor_));
    }

    ime_handler_->ChangeCompositionRange(selection_range, device_bounds);
  }
}

void UIEventHandlerWin::setDeviceScaleFactor(float factor) { deviceScaleFactor_ = factor; }

#endif