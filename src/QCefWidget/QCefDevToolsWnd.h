#ifndef QCEF_DEVTOOLS_WND_H_
#define QCEF_DEVTOOLS_WND_H_
#pragma once
#include <QMainWindow>
#include <include/cef_app.h>
#include "QCefWidgetImpl.h"

class QCefWidget;
class QCefDevToolsWnd : public QMainWindow {
  Q_OBJECT
 public:
  QCefDevToolsWnd(CefRefPtr<CefBrowser> targetBrowser,
                  QWidget* parent = nullptr);
  ~QCefDevToolsWnd();

 protected:
  void closeEvent(QCloseEvent* event) override;
  void setupUi();

  QCefWidget* cefWidget_;
  CefRefPtr<CefBrowser> targetBrowser_;
};
#endif  // !QCEF_DEVTOOLS_WND_H_