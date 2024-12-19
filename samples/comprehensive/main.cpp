#include "SampleWnd.h"
#include <QtWidgets/QApplication>
#include "QWebView/Creator.h"
#include "QWebView/Manager.h"

#pragma execution_character_set("utf-8")

int main(int argc, char* argv[]) {
  if (!InitializeCEF(true))
    return 0;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
  QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
#endif
  QApplication a(argc, argv);
  // 需要设置最后一个窗口关闭后，不自动退出应用程序
  a.setQuitOnLastWindowClosed(false);

  SampleWnd w;
  w.show();
  int iret = a.exec();

  UnInitializeCEF();
  return iret;
}
