#ifndef QCEF_WIDGET_H_
#define QCEF_WIDGET_H_
#pragma once

#include <memory>
#include <QtCore/qglobal.h>
#include <QWidget>
#include <QVariantList>
#include <QAbstractNativeEventFilter>
#include "QCefQuery.h"
#include "QCefEvent.h"
#include "QCefResourceProvider.h"

#ifndef QCEFWIDGET_EXPORT
#ifdef QCEFWIDGET_LIB
#define QCEFWIDGET_EXPORT Q_DECL_EXPORT
#else
#define QCEFWIDGET_EXPORT Q_DECL_IMPORT
#if (defined Q_OS_WIN32 || defined Q_OS_WIN64)
#pragma comment(lib, "QCefWidget.lib")
#endif
#endif
#endif

class QCefWidgetImpl;
class QCefDevToolsWnd;
class QCEFWIDGET_EXPORT QCefWidget : public QWidget {
  Q_OBJECT
public:
  QCefWidget(const QString &url = "", QWidget *parent = nullptr);
  virtual ~QCefWidget();

  virtual void navigateToUrl(const QString &url);

  virtual bool canGoBack();
  virtual bool canGoForward();
  virtual void goBack();
  virtual void goForward();

  virtual bool isLoadingBrowser();
  virtual void reloadBrowser(bool bIgnoreCache);
  virtual void stopLoadBrowser();

  virtual bool triggerEvent(const QString &name, const QCefEvent &event);

  // When received cefQueryRequest signal notify, can call this function to response.
  //
  virtual bool responseCefQuery(const QCefQuery &query);

  virtual void executeJavascript(const QString &javascript);

  // Must be set before browser created, otherwise return false.
  // OSR is enabled by default.
  //
  virtual bool setOsrEnabled(bool b);
  virtual bool osrEnabled() const;

  // Context menu is enabled by default.
  //
  virtual void setContextMenuEnabled(bool b);
  virtual bool contextMenuEnabled() const;

  virtual void setAutoShowDevToolsContextMenu(bool b);
  virtual bool autoShowDevToolsContextMenu() const;

  virtual void setAllowExecuteUnknownProtocolViaOS(bool b);
  virtual bool allowExecuteUnknownProtocolViaOS() const;

  virtual void setAutoDestoryCefWhenCloseEvent(bool b);
  virtual bool autoDestoryCefWhenCloseEvent();

  virtual void setFPS(int fps);
  virtual int fps() const;

  virtual void setBrowserBackgroundColor(const QColor &color);
  virtual QColor browserBackgroundColor() const;

  virtual void showDevTools();
  virtual void closeDevTools();

  virtual bool addResourceProvider(QCefResourceProvider* provider, const QString &identifier);
  virtual bool removeResourceProvider(const QString &identifier);
  virtual bool removeAllResourceProvider();
signals:
  void loadingStateChanged(bool isLoading, bool canGoBack, bool canGoForward);
  void titleChanged(QString title);
  void urlChanged(bool isMainFrame, const QString &url);
  void loadStart(bool isMainFrame);
  void loadEnd(bool isMainFrame, int httpStatusCode);
  void loadError(bool isMainFrame, int errorCode, const QString &errorMsg, const QString &failedUrl);
  void renderProcessTerminated(int status);
  void cefQueryRequest(const QCefQuery &query);
  void invokeMethodNotify(int browserId, int frameId, const QString &method, const QVariantList &arguments);
protected:
  bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
  bool event(QEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  void setVisible(bool visible) override;

  std::shared_ptr<QCefWidgetImpl> pImpl_;
  friend QCefDevToolsWnd;
};

#endif // QCEF_WIDGET_H_