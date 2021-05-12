#ifndef QCEFQUERY_H
#define QCEFQUERY_H
#pragma once

#include <cstdint>
#include <QtCore/qglobal.h>
#include <QString>
#include <QPointer>
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

class QCEFWIDGET_EXPORT QCefQuery {
 public:
  QCefQuery();

  QCefQuery(QString req, int64_t query);

  QCefQuery(const QCefQuery& other);

  QCefQuery& operator=(const QCefQuery& other);

  ~QCefQuery();

  const QString reqeust() const;

  const int64_t id() const;

  const QString response() const;

  const bool result() const;

  const int error() const;

  void setResponseResult(bool success, const QString& response, int error = 0) const;

 private:
  int64_t id_;

  QString reqeust_;

  mutable QString response_;

  mutable bool restult_;

  mutable int error_;

  static int TYPEID;
};
Q_DECLARE_METATYPE(QCefQuery);
#endif  // QCEFQUERY_H
