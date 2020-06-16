This repository was originally cloned from [tishion/QCefView](https://github.com/tishion/QCefView). 

Since [tishion/QCefView](https://github.com/tishion/QCefView) don't support OSR (offscreen rendering), so I create a repository with the same name to implement these features. 

**[tishion/QCefView](https://github.com/tishion/QCefView) and [winsoft666/QCefView](https://github.com/winsoft666/QCefView) are vastly different in code structure.**

---

**Known Issues:**
- CefRenderHandler::GetViewRect will be called frequently when resize browser window, and browser content can't resize correctly. 

---

Build instruction:

1. Download and install [CMake](https://cmake.org/)

2. Download and install Qt SDK from [Qt Downloads](https://download.qt.io/archive/qt/)

3. Download CEF binary distribution [Chromium Embedded Framework (CEF) Automated Builds](http://opensource.spotify.com/cefbuilds/index.html) and extract it to ***dep*** directory, for example:
    ```
    root
    ├─dep
    │  └─cef_binary_76.1.13+gf19c584+chromium-76.0.3809.132_windows64
    ├─src
    └─test
    ```

4. Modify the [config.cmake](config.cmake) to set the required build configurations

5. Just use CMake to build the project, for example:
    ``` bat
    REM create the build folder 
    mkdir build.win && cd build.win

    REM generate and build the project
    cmake .. && cmake --build .
    ```
