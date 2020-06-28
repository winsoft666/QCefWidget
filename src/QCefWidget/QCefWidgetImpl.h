#ifndef QCEF_WIDGET_IMPLEMENT_H_
#define QCEF_WIDGET_IMPLEMENT_H_
#pragma once
#include "CefBrowserApp/QCefBrowserHandler.h"
#include "Include/QCefEvent.h"
#include "Include/QCefQuery.h"
#include "QCefWidgetUIEventHandlerWin.h"
#include <QString>
#include <QWidget>


enum WidgetType { WT_Widget = 0, WT_OpenGLWidget };

class QCefWidgetImpl {
public:
  explicit QCefWidgetImpl(WidgetType vt, QWidget* pWidget);
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

  bool triggerEvent(const QString &name, const QCefEvent &event,
                    int frameId = QCefBrowserHandler::MAIN_FRAME);
  bool responseQCefQuery(const QCefQuery &query);
  void executeJavascript(const QString &javascript);

  bool createBrowser();
  bool createDevTools(CefRefPtr<CefBrowser> targetBrowser);

  void browserCreatedNotify(CefRefPtr<CefBrowser> browser);
  void browserDestoryedNotify(CefRefPtr<CefBrowser> browser);

  void OnImeCompositionRangeChanged(CefRefPtr<CefBrowser> browser, const CefRange &selection_range,
                                    const CefRenderHandler::RectList &character_bounds);

  QWidget* getWidget();
  WidgetType getWidgetType();

  bool nativeEvent(const QByteArray &eventType, void *message, long *result);

  bool paintEventHandle(QPaintEvent *event);
#ifndef QT_NO_OPENGL
  bool openGLPaintEventHandle(QPaintEvent *event);
#endif
  void setVisible(bool visible);

  float deviceScaleFactor();

  void setFPS(int fps);
  int fps() const;

  void setBrowserBackgroundColor(const QColor &color);
  QColor browserBackgroundColor() const;

  void updateCefWidget();

  void setBrowserClosing(bool b);

  CefRefPtr<CefBrowser> browser();
protected:
  bool sendEventNotifyMessage(int frameId, const QString &name, const QCefEvent &event);
  CefRefPtr<CefBrowserHost> getCefBrowserHost();

private:
  WidgetType vt_;
  QWidget *pWidget_;
  QWidget* pTopWidget_;

  CefRefPtr<QCefBrowserHandler> pQCefViewHandler_;
  std::shared_ptr<QCefWidgetUIEventHandlerWin> pCefUIEventWin_;

  // Cached url will be navigated when browser created
  CefString cachedNavigateUrl_;

  bool browserClosing_;
  bool browserCreated_;

  int fps_;
  QColor background_color_;
};
#endif