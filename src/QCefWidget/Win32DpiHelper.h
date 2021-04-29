#ifndef WIN32_DPI_HELPER_H_
#define WIN32_DPI_HELPER_H_
#pragma once

#include <windows.h>

namespace Win32DpiHelper {

float GetWindowScaleFactor(HWND hwnd);

bool IsProcessPerMonitorDpiAware();

// Returns the device scale factor. For example, 200% display scaling will return 2.0.
float GetDeviceScaleFactor();
}  // namespace Win32DpiHelper

#endif  // !WIN32_DPI_HELPER_H_