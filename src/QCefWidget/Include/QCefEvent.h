#ifndef QCEFEVENT_H
#define QCEFEVENT_H
#pragma once

#include <QtCore/qglobal.h>
#include <QObject>
#include <QString>
#include "QCefUserDefined.h"

#ifndef QCEFWIDGET_EXPORT
#ifdef QCEFWIDGET_LIB
#define QCEFWIDGET_EXPORT Q_DECL_EXPORT
#else
#define QCEFWIDGET_EXPORT Q_DECL_IMPORT
#if (defined Q_OS_WIN32 || defined Q_OS_WIN64)
#pragma comment(lib, QCEF_WIDGET_DLL_LIB_NAME)
#endif
#endif
#endif

class QCEFWIDGET_EXPORT QCefEvent : public QObject {
  Q_OBJECT
 public:
  QCefEvent();

  QCefEvent(const QString& name);

  void setEventName(const QString& name);

  void setIntProperty(const QString& key, int value);

  void setDoubleProperty(const QString& key, double value);

  void setStringProperty(const QString& key, const QString& value);

  void setBoolProperty(const QString& key, bool value);
};

#endif  // QCEFEVENT_H
