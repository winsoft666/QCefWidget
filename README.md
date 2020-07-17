[ >>> 中文版](README_ch.md)

The QCefWidget project provide a widget that is used to view webpage. 

Support:

✅ Supports both CEF OSR and non-OSR mode;
   
✅ Show background transparent webpage in irregularity window;

✅ Javascript and Qt interaction;

✅ OpenGL graphics acceleration;

✅ Third-party input method software;

✅ Perfect exit app without any CEF assert/exception in debug/release mode;

✅ Separate CEF rendering process and plugin process;

✅ Support Adobe Flash without any warnning, eg "Control-click to run Adobe Flash Player".

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
