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

//
// 1.  User clicks the window close button which sends a close notification to
//     the application's top-level window.
// 2.  Application's top-level window receives the close notification and:
//     A. Calls CefBrowserHost::CloseBrowser(false).
//     B. Cancels the window close.
// 3.  JavaScript 'onbeforeunload' handler executes and shows the close
//     confirmation dialog (which can be overridden via
//     CefJSDialogHandler::OnBeforeUnloadDialog()).
// 4.  User approves the close.
// 5.  JavaScript 'onunload' handler executes.
// 6.  Application's DoClose() handler is called. Application will:
//     A. Set a flag to indicate that the next close attempt will be allowed.
//     B. Return false.
// 7.  CEF sends an close notification to the application's top-level window.
// 8.  Application's top-level window receives the close notification and
//     allows the window to close based on the flag from #6B.
// 9.  Application's top-level window is destroyed.
// 10. Application's OnBeforeClose() handler is called and the browser object
//     is destroyed.
// 11. Application exits by calling CefQuitMessageLoop() if no other browsers
//     exist.
///

class QCefWidgetImpl;
class QCefDevToolsWnd;

class QCEFWIDGET_EXPORT QCefWidget : public QWidget {
  Q_OBJECT
public:
  QCefWidget(QWidget *parent = nullptr);
  virtual ~QCefWidget();

  virtual void navigateToUrl(const QString &url);
  virtual bool canGoBack();
  virtual bool canGoForward();
  virtual void goBack();
  virtual void goForward();
  virtual bool isLoadingBrowser();
  virtual void reloadBrowser();
  virtual void stopLoadBrowser();

  virtual bool triggerEvent(const QString &name, const QCefEvent &event);
  virtual bool triggerEvent(const QString &name, const QCefEvent &event, int frameId);
  virtual bool broadcastEvent(const QString &name, const QCefEvent &event);
  virtual bool responseQCefQuery(const QCefQuery &query);
  virtual void executeJavascript(const QString &javascript);

  virtual void setFPS(int fps);
  virtual int fps() const;

  virtual void setBrowserBackgroundColor(const QColor &color);
  virtual QColor browserBackgroundColor() const;

  virtual void showDevTools();
  virtual void closeDevTools();
signals:
  void loadingStateChanged(bool isLoading, bool canGoBack, bool canGoForward);
  void titleChanged(QString title);
  void loadStart();
  void loadEnd(int httpStatusCode);
  void loadError(int errorCode, const QString &errorMsg, const QString &failedUrl);
  void renderProcessTerminated(int status);
  void cefUrlRequest(const QString &url);
  void cefQueryRequest(const QCefQuery &query);
  void invokeMethodNotify(int browserId, int frameId, const QString &method,
                          const QVariantList &arguments);
protected:
  bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
  void paintEvent(QPaintEvent *event) override;
  void setVisible(bool visible) override;

  std::shared_ptr<QCefWidgetImpl> pImpl_;
  friend QCefDevToolsWnd;
};

#endif // QCEF_WIDGET_H_