#include "SampleWnd.h"
#include <QtWidgets>
#include "QWebView/Creator.h"
#include "QWebView/Manager.h"

int main(int argc, char* argv[]) {
  bool isCEFSubProcess = false;
  if (argc >= 2) {
    for (int i = 0; i < argc; i++) {
      if (strcmp(argv[i], "--type=renderer") == 0 ||
          strcmp(argv[i], "--type=gpu-process") == 0 ||
          strcmp(argv[i], "--type=utility") == 0) {
        isCEFSubProcess = true;
        break;
      }
    }
  }

  if (isCEFSubProcess) {
    if (!InitializeCEF(false))
      return 0;
  }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
  QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
#endif
  QApplication a(argc, argv);

  QWebView::BrowserEngine engine = QWebView::BrowserEngine::None;

  QMessageBox msgBox;
  msgBox.setText("当前示例用于演示如何选择性的加载浏览器内核运行时。\n请选择需要使用的浏览器内核。");
  msgBox.setIcon(QMessageBox::Icon::Question);
  msgBox.setWindowTitle("浏览器内核");
  QAbstractButton* cefButton = msgBox.addButton("CEF内核", QMessageBox::ButtonRole::ActionRole);
  QAbstractButton* webView2Button = msgBox.addButton("WebView2内核", QMessageBox::ButtonRole::ActionRole);

  msgBox.exec();

  QAbstractButton* clickedBtn = msgBox.clickedButton();
  if (clickedBtn == cefButton) {
    engine = QWebView::BrowserEngine::CEF;
  }
  else if (clickedBtn == webView2Button) {
    engine = QWebView::BrowserEngine::WebView2;
  }

  if (engine == QWebView::BrowserEngine::CEF) {
    if (!InitializeCEF(false))
      return 0;
  }

  // 需要设置最后一个窗口关闭后，不自动退出应用程序
  a.setQuitOnLastWindowClosed(false);

  QObject::connect(QWebViewManager::Get(), &QWebViewManager::allWebViewsClosed, []() {
    qApp->quit();
  });

  SampleWnd w(engine);
  w.resize(1200, 600);
  w.show();
  int iret = a.exec();
  qDebug() << ">>>> Exit Qt Messageloop";

  if (engine == QWebView::BrowserEngine::CEF) {
    UnInitializeCEF();
  }
  return iret;
}
