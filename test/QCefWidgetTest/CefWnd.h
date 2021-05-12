#pragma once
#include <QWidget>
#include <QString>
#include <QSize>
#include <QDebug>
#include "QCefQuery.h"

class QCefWidget;
class QCefOpenGLWidget;

class CefWnd : public QWidget {
  Q_OBJECT
 public:
  CefWnd(QWidget* parent = nullptr);
  virtual ~CefWnd();

  void setupUi();

  bool usingGLWidget();
  void setUsingGLWidget(bool b);

  void setAllowExecuteUnknownProtocolViaOS(bool b);
  bool allowExecuteUnknownProtocolViaOS();

  bool framelessWindow();
  void setFramelessWindow(bool b);

  bool translucentWindowBackground();
  void setTranslucentWindowBackground(bool b);

  bool osrEnabled();
  void setOsrEnabled(bool b);

  bool contextMenuEnabled();
  void setContextMenuEnabled(bool b);

  bool autoAddDevToolsContextMenu();
  void setAutoAddDevToolsContextMenu(bool b);

  bool usingHideInsteadClose();
  void setUsingHideInsteadClose(bool b);

  QSize initSize();
  void setInitSize(QSize s);

  QString initUrl();
  void setInitUrl(QString u);

  int maximumFps();
  void setMaximumFps(int f);

  QColor windowBkColor();
  void setWindowBkColor(QColor c);

  QColor browserBkColor();
  void setBrowserBkColor(QColor c);

  void forceClose();

 signals:
  void cefWndVisibleChange(bool bVisible);

 protected:
  QSize sizeHint() const override;
  void showEvent(QShowEvent* event) override;
  void hideEvent(QHideEvent* event) override;
  void closeEvent(QCloseEvent *event) override;
 public slots:
  virtual void onNavigateToUrl(QString url);
  virtual void onReload();
  virtual void onShowDevTools();
  virtual void onCloseDevTools();
  virtual void onInvokeMethodNotify(int browserId,
                                    int frameId,
                                    const QString& method,
                                    const QVariantList& arguments);
  virtual void onTriggerEvent();
  virtual void onCefQueryRequest(const QCefQuery& query);

 protected:
  bool usingGLWidget_;
  bool framelessWindow_;
  bool translucentWindowBackground_;
  bool allowExecuteUnknownProtocolViaOS_;
  bool usingHideInsteadClose_;
  bool forceClose_;
  QSize initSize_;
  QString initUrl_;
  QColor windowBkColor_;
  QColor browserBkColor_;
  bool contextMenuEnabled_;
  bool autoAddDevToolsContextMenu_;
  bool osrEnabled_;
  int maximumFps_;

  QCefWidget* pCefWidget_;
  QCefOpenGLWidget* pCefGLWidget_;
};