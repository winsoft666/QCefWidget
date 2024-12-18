#pragma once
#include <QtWidgets>
#include <QDebug>
#include "WebViewWndBase.hpp"
#include "QWebView/Core.h"

#pragma execution_character_set("utf-8")

class QWebView;
class WebViewWnd : public WebViewWndBase<QWidget> {
  Q_OBJECT
 public:
  WebViewWnd(bool frameless,
             bool translucentWindowBackground,
             QWebView::BrowserEngine engine,
             QWidget* parent = nullptr);
  virtual ~WebViewWnd();

  void setupUi();

  void setAllowExecuteUnknownProtocolViaOS(bool b);
  bool allowExecuteUnknownProtocolViaOS();

  bool framelessWindow();

  bool translucentWindowBackground();

  QWebView::BrowserEngine browserEngine() const;

  bool osrEnabled();
  void setOsrEnabled(bool b);

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
  void webViewWndVisibleChanged(bool bVisible);
  void webViewWndClosed();
 protected:
  QSize sizeHint() const override;
  void showEvent(QShowEvent* event) override;
  void hideEvent(QHideEvent* event) override;
  void closeEvent(QCloseEvent* e) override;
 public slots:
  virtual void onNavigateToUrl(QString url);
  virtual void onReload();
  virtual void onShowDevTools();
  virtual void onCloseDevTools();
  virtual void onTriggerEvent();
  virtual void onPopupWindow(const QString& url);

 private slots:
  void UpdateWindowTitle();

 protected:
  bool framelessWindow_;
  bool translucentWindowBackground_;
  bool allowExecuteUnknownProtocolViaOS_;
  bool usingHideInsteadClose_;
  bool forceClose_;
  QSize initSize_;
  QString initUrl_;
  QColor windowBkColor_;
  QColor browserBkColor_;
  bool osrEnabled_;
  int maximumFps_;

  QWebView::BrowserEngine engine_;
  QWebView* webview_;

  QString curTitle_;
  QString curUrl_;
  bool curLoading_ = false;
};