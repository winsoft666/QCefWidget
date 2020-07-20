#ifndef QCEF_WIDGET_IMPLEMENT_H_
#define QCEF_WIDGET_IMPLEMENT_H_
#pragma once
#include "CefBrowserApp/QCefBrowserHandler.h"
#include "Include/QCefEvent.h"
#include "Include/QCefQuery.h"
#include "QCefWidgetUIEventHandlerWin.h"
#include <QString>
#include <QWidget>
#include "QCefBrowserSetting.h"

enum WidgetType { WT_Widget = 0, WT_OpenGLWidget };

class QCefWidgetImpl {
public:
  explicit QCefWidgetImpl(WidgetType vt, QWidget *pWidget, const QString &url);
  ~QCefWidgetImpl();

public:
  void navigateToUrl(const QString &url);

  bool canGoBack();
  bool canGoForward();
  void goBack();
  void goForward();
  bool isLoadingBrowser();
  void reloadBrowser();
  void stopLoadBrowser();

  bool triggerEvent(const QString &name, const QCefEvent &event);
  bool responseCefQuery(const QCefQuery &query);
  void executeJavascript(const QString &javascript);

  bool createBrowser(const QString &url);
  bool createDevTools(CefRefPtr<CefBrowser> targetBrowser);

  void browserClosingNotify(CefRefPtr<CefBrowser> browser);
  void browserCreatedNotify(CefRefPtr<CefBrowser> browser);
  void browserDestoryedNotify(CefRefPtr<CefBrowser> browser);

  void OnImeCompositionRangeChanged(CefRefPtr<CefBrowser> browser, const CefRange &selection_range, const CefRenderHandler::RectList &character_bounds);

  QWidget *getWidget();
  WidgetType getWidgetType();
  QRect rect();

  bool nativeEvent(const QByteArray &eventType, void *message, long *result);
  bool event(QEvent *event);
  bool paintEventHandle(QPaintEvent *event);
#ifndef QT_NO_OPENGL
  bool openGLPaintEventHandle(QPaintEvent *event);
#endif
  void setVisible(bool visible);

  void setBrowserBackgroundColor(const QColor &color);
  bool setOsrEnabled(bool b);
  void setContextMenuEnabled(bool b);
  void setAllowExecuteUnknownProtocolViaOS(bool b);
  float deviceScaleFactor();
  void setFPS(int fps);
  QCefBrowserSetting browserSetting() const;

  void updateCefWidget();

  void setBrowserClosing(bool b);

  CefRefPtr<CefBrowser> browser() const;

protected:
  bool sendEventNotifyMessage(const QString &name, const QCefEvent &event);
  CefRefPtr<CefBrowserHost> getCefBrowserHost();

private:
  WidgetType vt_;
  QWidget *pWidget_;
  QWidget *pTopWidget_;
  QString initUrl_;

  WId widgetWId_;

  CefRefPtr<QCefBrowserHandler> pQCefViewHandler_;
  std::shared_ptr<QCefWidgetUIEventHandlerWin> pCefUIEventWin_;

  bool browserClosing_;
  bool browserCreated_;

  QCefBrowserSetting browserSetting_;
};
#endif