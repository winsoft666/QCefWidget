#ifndef WIN32_DPI_HELPER_H_
#define WIN32_DPI_HELPER_H_
#pragma once

namespace Win32DpiHelper {

float GetDeviceScaleFactor();

bool IsProcessPerMonitorDpiAware();

// Returns the device scale factor. For example, 200% display scaling will return 2.0.
float GetDeviceScaleFactor();
} // namespace Win32DpiHelper

#endif // !WIN32_DPI_HELPER_H_