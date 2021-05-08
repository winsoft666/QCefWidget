#ifndef QCEF_RESOURCE_PROVIDER_H_
#define QCEF_RESOURCE_PROVIDER_H_
#pragma once
#include <QString>
#include <QByteArray>
#include <QtCore/qglobal.h>
#include <QMetaType>
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

class QCEFWIDGET_EXPORT QCefResourceProvider {
 public:
  virtual bool onRequest(const QString& resourceUrl, QByteArray& buffer) = 0;
};

#endif