#if defined(_MSVC_) && _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <ShellScalingApi.h>

class DpiUtil {
   public:
    static void SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT dpiAwarenessContext);
    static int GetDpiForWindow(HWND window);
    static double GetDpiScaleForWindow(HWND window);
   private:
    static HMODULE GetUser32Module();
    static HMODULE GetShcoreModule();
    static PROCESS_DPI_AWARENESS ProcessDpiAwarenessFromDpiAwarenessContext(
        DPI_AWARENESS_CONTEXT dpiAwarenessContext);
};
