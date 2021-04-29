#include "Win32DpiHelper.h"
#include <windows.h>
#include <shellscalingapi.h>

namespace Win32DpiHelper {
// DPI value for 1x scale factor.
#define DPI_1X 96.0f

float GetDeviceScaleFactor() {
  static float scale_factor = 1.0f;
  static bool initialized = false;

  if (!initialized) {
    // This value is safe to cache for the life time of the app since the user
    // must logout to change the DPI setting. This value also applies to all
    // screens.
    HDC screen_dc = ::GetDC(NULL);
    int dpi_x = ::GetDeviceCaps(screen_dc, LOGPIXELSX);
    scale_factor = static_cast<float>(dpi_x) / 96.0f;
    ::ReleaseDC(NULL, screen_dc);
    initialized = true;
  }

  return scale_factor;
}

// Returns true if the process is per monitor DPI aware.
bool IsProcessPerMonitorDpiAware() {
  enum class PerMonitorDpiAware {
    UNKNOWN = 0,
    PER_MONITOR_DPI_UNAWARE,
    PER_MONITOR_DPI_AWARE,
  };
  static PerMonitorDpiAware per_monitor_dpi_aware = PerMonitorDpiAware::UNKNOWN;
  if (per_monitor_dpi_aware == PerMonitorDpiAware::UNKNOWN) {
    per_monitor_dpi_aware = PerMonitorDpiAware::PER_MONITOR_DPI_UNAWARE;
    HMODULE shcore_dll = ::LoadLibrary(L"shcore.dll");
    if (shcore_dll) {
      typedef HRESULT(WINAPI * GetProcessDpiAwarenessPtr)(
          HANDLE, PROCESS_DPI_AWARENESS*);
      GetProcessDpiAwarenessPtr func_ptr =
          reinterpret_cast<GetProcessDpiAwarenessPtr>(
              ::GetProcAddress(shcore_dll, "GetProcessDpiAwareness"));
      if (func_ptr) {
        PROCESS_DPI_AWARENESS awareness;
        if (SUCCEEDED(func_ptr(nullptr, &awareness)) &&
            awareness == PROCESS_PER_MONITOR_DPI_AWARE)
          per_monitor_dpi_aware = PerMonitorDpiAware::PER_MONITOR_DPI_AWARE;
      }
    }
  }
  return per_monitor_dpi_aware == PerMonitorDpiAware::PER_MONITOR_DPI_AWARE;
}

float GetWindowScaleFactor(HWND hwnd) {
  if (hwnd && IsProcessPerMonitorDpiAware()) {
    typedef UINT(WINAPI * GetDpiForWindowPtr)(HWND);
    static GetDpiForWindowPtr func_ptr = reinterpret_cast<GetDpiForWindowPtr>(
        GetProcAddress(GetModuleHandle(L"user32.dll"), "GetDpiForWindow"));
    if (func_ptr)
      return static_cast<float>(func_ptr(hwnd)) / DPI_1X;
  }

  return GetDeviceScaleFactor();
}
}  // namespace Win32DpiHelper