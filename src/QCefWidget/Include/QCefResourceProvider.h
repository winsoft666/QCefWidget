#ifndef QCEF_RESOURCE_PROVIDER_H_
#define QCEF_RESOURCE_PROVIDER_H_
#pragma once
#include <QString>
#include <QByteArray>
#include <QMetaType>
#include "QCefExport.h"
#include "QCefUserDefined.h"

class QCEFWIDGET_EXPORT QCefResourceProvider {
 public:
  virtual bool onRequest(const QString& resourceUrl, QByteArray& buffer) = 0;
};

#endif