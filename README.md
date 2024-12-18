<h1 align="center">QWebView</h1>

QWebView 提供一个 Qt 小部件来显示网页并与网页交互，支持 CEF 和 Microsoft WebView2 浏览器内核。

针对两种不同的浏览器内核，QWebView 提供了统一的接口，详见 QWebView 类。

使用 CreateCEF 创建 CEF 内核的 QWebView 对象；使用 CreateWebView2 创建 Microsoft WebView2 内核的 QWebView 对象。

QWebView 对于 CEF 和 Microsoft WebView2 的运行时文件（如 libcef.dll 或 EmbeddedBrowserWebView.dll）不是强依赖的。基于该特性，我们可以根据软件的实际运行环境来动态地切换浏览器内核，例如检测到用户系统上已安装 Microsoft WebView2 运行时，则软件使用 WebView2 内核，否则选择下载 CEF 运行时 或 Microsoft WebView2 运行时。

仓库原名 QCefWidget（目前位于 [QCefWidget 分支](https://github.com/winsoft666/QCefWidget/tree/QCefWidget)），原项目仅支持 CEF 内核，而且在实现方式上采用了全部自己实现 CEF 接口的方式，这种方式看似优雅，但工作量很大，而且灵活性欠佳，特别是需要适配 CEF 不同版本时，需要做大量的判断和特殊处理。

QWebView 在 CEF 内核实现方面，采用了另外的方式，直接复用 cefclient 示例的代码，仅在其基础上做少量修改，既保证了功能的稳定性，也方便适配不同的 CEF 版本。
