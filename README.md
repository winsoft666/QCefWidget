[ >>> 中文版](README_ch.md)

The QCefWidget project provide a widget that is used to view webpage. QCefWidget is implemented based on the OSR mode of [CEF](https://bitbucket.org/chromiumembedded/cef/src/master/).

Support:

✅ Show background transparent webpage in irregularity window;

✅ Javascript and Qt interaction;

✅ OpenGL graphics acceleration;

✅ Third-party input method software;

✅ Perfect exit app without any CEF assert in debug/release mode;

✅ Separate CEF rendering process and plugin process;

---


## Build Instruction

1. Download and install [CMake](https://cmake.org/)

2. Download and install Qt SDK from [Qt Downloads](https://download.qt.io/archive/qt/)

3. Download CEF binary distribution [Chromium Embedded Framework (CEF) Automated Builds](http://opensource.spotify.com/cefbuilds/index.html) and extract it to ***dep*** directory, for example:
    ```
    root
    ├─dep
    │  └─cef_binary_80.1.15+g7b802c9+chromium-80.0.3987.163_windows32
    ├─src
    └─test
    ```

4. Update the [config.cmake](config.cmake) to set the required build configurations

5. Using CMake to build the project, for example:
    ``` bat
    mkdir build && cd build
    cmake .. && cmake --build .
    ```

---

Thanks to [QCefView](https://github.com/tishion/QCefView) project, `QCefWidget` reference for the project in terms of engineering structure. However `QCefWidget` based on CEF OSR mode, this is quite different from `QCefView`. 
