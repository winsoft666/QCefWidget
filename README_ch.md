[ >>> English Version](README.md)

`QCefWidget`项目提供一个Widget来显示网页.

支持：

✅ 同时支持CEF OSR和非OSR模式;

✅ 在不规则窗体中显示背景透明的网页;

✅ Javascript与Qt交互;

✅ OpenGL图形加速;

✅ 第三方输入法;

✅ 在Debug/Release模式下完美退出程序，无任何CEF断言;

✅ 独立的CEF渲染进程和插件进程;

✅ 支持Adobe Flash, 没有任何警告，如"Control-click to run Adobe Flash Player".

✅ 只需要设置CSS属性，就可以实现像Electron一样拖拽无边框窗体.

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
    mkdir build && cd build
    cmake .. && cmake --build .
    ```

## 用法
test目录中的QCefWidgetTest是一个示例程序，演示了如何使用QCefWidget。

![screenshot1 on windows](test/Screenshot/screenshot1.png)

![screenshot2 on windows](test/Screenshot/screenshot2.png)