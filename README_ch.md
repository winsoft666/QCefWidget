[ >>> English Version](README.md)

`QCefWidget`项目提供一个widget来显示网页，基于[CEF](https://bitbucket.org/chromiumembedded/cef/src/master/)离屏渲染(Offscreen Rendering)模式实现。

支持：

✅ 显示背景透明网页;

✅ Javascript与Qt交互;

✅ OpenGL图形加速;

---


## 快速构建

1. 下载安装[CMake](https://cmake.org/).

2. 从[Qt Downloads](https://download.qt.io/archive/qt/)下载安装Qt SDK.

3. 从[Chromium Embedded Framework (CEF) Automated Builds](http://opensource.spotify.com/cefbuilds/index.html)下载CEF预编译版本，并解压到 ***dep*** 目录, 目录结构类似:
    ```
    root
    ├─dep
    │  └─cef_binary_80.1.15+g7b802c9+chromium-80.0.3987.163_windows32
    ├─src
    └─test
    ```

4. 在[config.cmake](config.cmake)文件中设置必要的配置参数.

5. 使用`CMake`命令生成项目文件并构建:
    ``` bat
    REM create the build folder 
    mkdir build && cd build

    REM generate and build the project
    cmake .. && cmake --build .
    ```

> 感谢[QCefView](https://github.com/tishion/QCefView)项目，`QCefWidget`在工程结构方面极大的参考了该项目.