[ >>> 中文版](README_ch.md)

# 1. QCefWidget

The QCefWidget project provide a widget that used to render and interact with webpage. 

Support:

✅ Supports both CEF OSR and non-OSR mode;

✅ Show background transparent webpage in irregularity window;

✅ Javascript and Qt interaction;

✅ OpenGL graphics acceleration;

✅ Third-party input method software;

✅ Perfect exit app without any CEF assert/exception in debug/release mode;

✅ Separate CEF rendering process and plugin process;

✅ Support Adobe Flash without any warnning, eg "Control-click to run Adobe Flash Player".

✅ Drag/Drop frameless window like Electron throught setting CSS property.

✅ Move between mixed scale factor monitors.

---


# 2. Build Instruction
## 2.1 CMake

Download and install [CMake](https://cmake.org/).

## 2.2 Qt SDK

Download and install Qt SDK from [Qt Downloads](https://download.qt.io/archive/qt/).

## 2.3 CEF binary distribution.

Download CEF binary distribution from [Chromium Embedded Framework (CEF) Automated Builds](https://cef-builds.spotifycdn.com/index.html) and extract it to ***dep***   (Optional) directory.

For example:

```txt
root
├─dep
│  └─cef_binary_80.1.15+g7b802c9+chromium-80.0.3987.163_windows32
├─src
└─test
```

## 2.4 config.cmake
Update the [config.cmake](config.cmake) to set the required build configurations.

### 2.4.1 QCefWidget SDK Version

```bash
SET(QCEF_VERSION_MAJOR 1)
SET(QCEF_VERSION_MINOR 0)
SET(QCEF_VERSION_PATCH 3)
```


### 2.4.2 CEF SDK
#### Special CEF SDK folder:

```bash
set(CEF_SDK_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/dep/cef_binary_76.1.13+gf19c584+chromium-76.0.3809.132_windows32")
```

#### Special CEF SDK version:

```bash
SET(CEF_VERSION_MAJOR 76)
SET(CEF_VERSION_MINOR 1)
SET(CEF_VERSION_PATCH 13)
```

CEF has two version format:
- 3.3683.1920.g9f41a27 / Chromium **73.0.3683**.75
- **73.1.3**+g46cf800+chromium-73.0.3683.75 / Chromium 73.0.3683.75

For CEF_VERSION_XXX macro, the first format type borrows the first 3 paragraphs of the Chromium's version number, the second format can use CEF's version directly.

QCefWidget library will use these version macro to support different CEF version, such as:

``` c++
#if CEF_VERSION_MAJOR == 72
virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
			CefProcessId source_process,
			CefRefPtr<CefProcessMessage> message) override;
#elif CEF_VERSION_MAJOR == 76 || CEF_VERSION_MAJOR == 89
virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefProcessId source_process,
			CefRefPtr<CefProcessMessage> message) override;
#endif
```


## 2.5 Using CMake to build the project.
For example:

```bash
mkdir build && cd build
cmake .. && cmake --build .
```

# 3. Test
QCefWidget has been tested with follow Qt and CEF version:

|Qt Version|CEF Version|Pass|
|---|---|---|
|5.12.10|3.3626.1895|✅|
|5.12.10|76.1.13|✅|
|5.12.10|89.0.18|✅|
|5.14.2|3.3626.1895|✅|
|5.15.2|74.1.19|✅|
|5.15.2|76.1.13|✅|
|5.15.2|89.0.18|✅|
|5.15.2|91.1.2|✅|

In theory, other `Qt5+` versions and CEF versions are also supported.

# 4. Usage
QCefWidgetTest is a perfect demo about how to use QCefWidget in `test` folder.

![screenshot1 on windows](./test/Screenshot/screenshot1.png)

![screenshot2 on windows](./test/Screenshot/screenshot2.png)

# 5. Donate
Open source is not a easy work, please support me! If you don't want to donate, just Star⭐ it.

# 6. Code structure
The following figure shows about how to destroy the QCefWidget and window when user clicked the X button (or sends a WM_CLOSE message to the window):

![user clieck x button](./x-button-close.png)

> When the window closed by `QWidget::close()`, the process of QCefWidget and window destruction is slightly different from the above picture, but it is roughly similar.
