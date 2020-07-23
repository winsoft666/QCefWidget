#include "BuiltInUrl.h"
#include <QCoreApplication>

QStringList getBuiltInUrl() {
  return QStringList()
    << "http://qcefwidget/test.html"
    << "chrome://version"
    << "https://html5test.com"
    << "https://www.bing.com"
    << "https://www.google.com"
    << "https://map.baidu.com"
    << "https://ant.design/components/overview" 
    << QString("file:///%1").arg(QCoreApplication::applicationDirPath() + u8"/TestResource/flash.html")
    << QString("file:///%1").arg(QCoreApplication::applicationDirPath() + u8"/TestResource/tree.html")
    << QString("file:///%1").arg(QCoreApplication::applicationDirPath() + u8"/TestResource/pdf.html")
    << QString("file:///%1").arg(QCoreApplication::applicationDirPath() + u8"/TestResource/clock.html");
}
