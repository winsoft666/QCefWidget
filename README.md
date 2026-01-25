[[简体中文](README_zh.md)|English]

<h1 align="center">QWebView</h1>

QWebView provides a Qt widget for displaying web pages and interacting with them, and supports the CEF and Microsoft WebView2 browser kernels.

## Uniform Interface
For the two different browser kernels, QWebView provides a unified interface. See the [QWebView class](./include/QWebView/Core.h) for details.

- Call the CreateCEF method to create a QWebView object with the CEF kernel.

- Call the CreateWebView2 method to create a QWebView object with the Microsoft WebView2 kernel.

## Non-Strong Dependency on Runtime
QWebView does not strongly depend on the runtime files of CEF and Microsoft WebView2 (such as libcef.dll or EmbeddedBrowserWebView.dll).

Based on this feature, we can dynamically switch the browser kernel according to the actual running environment of the software. For example, if the Microsoft WebView2 runtime is detected as installed on the user's system, the software will use the WebView2 kernel; otherwise, it will choose to download the CEF runtime or the Microsoft WebView2 runtime.

See the [dynamic_engine](./samples/dynamic_engine) example for details.

## New CEF Integration Method
This repository was originally named QCefWidget (currently located in the [QCefWidget branch](https://github.com/winsoft666/QCefWidget/tree/QCefWidget)). 

The original project only supported the CEF kernel, and in terms of implementation, it adopted the approach of implementing all CEF interfaces by itself. This approach may seem elegant, but it involves a large amount of work and has poor flexibility. Especially when adapting to different CEF versions, a large number of judgments and special treatments need to be done using precompiled macros.

**No matter how the CEF version and its interfaces are updated, its official examples can always run correctly.**

Therefore, in terms of the implementation of the CEF kernel, QWebView adopts a new approach by directly reusing the code of the cefclient official example, with only a few modifications on that basis. This ensures the stability of functions and facilitates adaptation to different CEF versions.

Currently, QWebView is only adapted to the Windows 32-bit version 74.1.19 of CEF. If it is necessary to support other CEF versions (not limited to operating systems and CEF versions), it is not difficult. Just download the corresponding cefclient example code from the [CEF official website](https://cef-builds.spotifycdn.com/index.html) and modify it by referring to the modification methods in this project.

> Search for "winsoft666: [update-cefclient]" in the dep\cef_binary_74.1.19+gb62bacf+chromium-74.0.3729.157_windows32 directory to find all modifications.

## Rich Examples
The abundant examples are not only for demonstrating the usage of QWebView but also for better testing its own functions.

For beginners, it is recommended to start with the single_cef or single_webview2 examples, and the comprehensive example is a comprehensive one, which is convenient for testing all functions of QWebView.

![Screenshot of the Comprehensive Example](./screenshots/Comprehensive.png "Screenshot of the Comprehensive Example")

## Precautions
In programs using QWebView, it is necessary to call the `QApplication::setQuitOnLastWindowClosed` method to set that the Qt application will not automatically exit when the last window is closed. 

Use the `QWebViewManager::allWebViewsClosed` signal to determine whether to exit the application.

For the host window of QWebView, the `Qt::WA_DeleteOnClose` attribute cannot be set, and for the top-level host window of QWebView, additional processing of the QCloseEvent event is required. 

See the example program for details.
