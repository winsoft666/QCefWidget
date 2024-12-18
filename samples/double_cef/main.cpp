#include "SampleWnd.h"
#include <QtWidgets/QApplication>
#include "QWebView/Creator.h"
#include "QWebView/Manager.h"

int main(int argc, char* argv[]) {
  if (!InitializeCEF(false))
    return 0;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
  QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
#endif
  QApplication a(argc, argv);

  // ��Ҫ�������һ�����ڹرպ󣬲��Զ��˳�Ӧ�ó���
  a.setQuitOnLastWindowClosed(false);

  QObject::connect(QWebViewManager::Get(), &QWebViewManager::allWebViewsClosed, []() {
    qApp->quit();
  });

  SampleWnd w;
  w.resize(1200, 600);
  w.show();
  int iret = a.exec();
  qDebug() << ">>>> Exit Qt Messageloop";

  UnInitializeCEF();
  return iret;
}