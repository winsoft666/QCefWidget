#ifndef QCEFEVENT_H
#define QCEFEVENT_H

#include <QObject>
#include <QString>
#include "QCefExport.h"
#include "QCefUserDefined.h"

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
