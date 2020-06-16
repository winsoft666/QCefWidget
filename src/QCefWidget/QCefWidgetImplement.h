#ifndef QCEF_WIDGET_IMPLEMENT_H_
#define QCEF_WIDGET_IMPLEMENT_H_
#pragma once
#include "CefBrowserApp/CefBrowserHandler.h"
#include "Include/QCefEvent.h"
#include "Include/QCefQuery.h"
#include "UIEventHandlerWin.h"
#include <QString>
#include <QWidget>


enum WidgetType { Widget = 0, OpenGLWidget };

class QCefWidgetImplement {
public:
  explicit QCefWidgetImplement(WidgetType vt, QWidget* pWidget);
  ~QCefWidgetImplement();

public:
  void navigateToUrl(const QString &url);
  bool browserCanGoBack();
  bool browserCanGoForward();
  void browserGoBack();
  void browserGoForward();
  bool browserIsLoading();
  void browserReload();
  void browserStopLoad();
  bool triggerEvent(const QString &name, const QCefEvent &event,
                    int frameId = CefBrowserHandler::MAIN_FRAME);
  bool responseQCefQuery(const QCefQuery &query);
  void executeJavascript(const QString &javascript);

  bool createBrowser();

  void browserCreatedNotify(CefRefPtr<CefBrowser> browser);
  void browserDestoryedNotify(CefRefPtr<CefBrowser> browser);

  void OnImeCompositionRangeChanged(CefRefPtr<CefBrowser> browser, const CefRange &selection_range,
                                    const CefRenderHandler::RectList &character_bounds);

  QWidget* getWidget();
  WidgetType getWidgetType();

  bool nativeEvent(const QByteArray &eventType, void *message, long *result);
  void paintEvent(QPaintEvent *event);
  void openGLPaintEvent(QPaintEvent *event);
  float deviceScaleFactor();

  void setFPS(int fps);
  int fps() const;

  void setBackgroundColor(const QColor &color);
  QColor backgroundColor() const;

  void updateCefView();

  void setBrowserClosing(bool b);
protected:
  bool sendEventNotifyMessage(int frameId, const QString &name, const QCefEvent &event);
  CefRefPtr<CefBrowserHost> getCefBrowserHost();

private:
  WidgetType vt_;
  QWidget *pWidget_;
  QWidget* pTopWidget_;

  CefRefPtr<CefBrowserHandler> pQCefViewHandler_;
  std::shared_ptr<UIEventHandlerWin> pCefUIEventWin_;

  // Cached url will be navigated when browser created
  CefString cachedNavigateUrl_;

  bool browserClosing_;
  bool browserCreated_;

  int fps_;
  QColor background_color_;
};
#endif