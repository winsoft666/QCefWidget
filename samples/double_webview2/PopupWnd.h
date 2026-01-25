#pragma once
#include <QtWidgets>
#include <QDebug>
#include "QWebView/Manager.h"

#pragma execution_character_set("utf-8")

class QWebView;
class PopupWnd : public QWidget {
  Q_OBJECT
 public:
  PopupWnd(QString url, QWidget* parent = nullptr);

  void closeEvent(QCloseEvent* e);

 private slots:
  void onNewPopupWindow(QString url);

 protected:
  QWebView* webview_ = nullptr;
};