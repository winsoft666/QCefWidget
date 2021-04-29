#include "Include/QCefEvent.h"
#include <QVariant>
#include <include/cef_app.h>
#include <include/wrapper/cef_message_router.h>

QCefEvent::QCefEvent() :
    QObject(nullptr) {}

QCefEvent::QCefEvent(const QString& name) :
    QObject() {
  setObjectName(name);
}

void QCefEvent::setEventName(const QString& name) {
  setObjectName(name);
}

void QCefEvent::setIntProperty(const QString& key, int value) {
  setProperty(key.toUtf8(), QVariant::fromValue(value));
}

void QCefEvent::setDoubleProperty(const QString& key, double value) {
  setProperty(key.toUtf8(), QVariant::fromValue(value));
}

void QCefEvent::setStringProperty(const QString& key, const QString& value) {
  setProperty(key.toUtf8(), QVariant::fromValue(value));
}

void QCefEvent::setBoolProperty(const QString& key, bool value) {
  setProperty(key.toUtf8(), QVariant::fromValue(value));
}
