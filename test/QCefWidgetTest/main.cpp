#include "TestWnd.h"
#include <QtWidgets/QApplication>

#pragma execution_character_set("utf-8")

int main(int argc, char* argv[]) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
  QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
#endif
  QApplication a(argc, argv);

  TestWnd w;
  w.show();
  int iret = a.exec();
  return iret;
}
