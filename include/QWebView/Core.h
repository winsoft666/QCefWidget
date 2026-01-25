/*
 * Copyright (c) winsoft666.
 * All rights reserved.
 *
 * This source code is licensed under the license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef QWEBVIEW_CORE_H_
#define QWEBVIEW_CORE_H_
#pragma once

#include <QtCore/qglobal.h>
#include <QWidget>
#include <QString>
#include <QFlags>
#include <QDebug>

#ifndef QWEBVIEW_CORE_EXPORT
#ifdef QWEBVIEW_CORE_LIB
#define QWEBVIEW_CORE_EXPORT Q_DECL_EXPORT
#else
#define QWEBVIEW_CORE_EXPORT Q_DECL_IMPORT
#endif
#endif

class QWEBVIEW_CORE_EXPORT QWebView : public QWidget {
  Q_OBJECT
 public:
  enum BrowserEngine {
    None = 0,
    WebView2 = 1 << 0,  // Only support on windows.
    CEF = 1 << 1,
  };
  Q_DECLARE_FLAGS(BrowserEngines, BrowserEngine)

  QWebView(QWidget* parent = Q_NULLPTR);

  virtual BrowserEngine browserEngine() const;

  virtual void navigate(const QString& url) = 0;

  virtual bool canGoBack() = 0;
  virtual bool canGoForward() = 0;
  virtual void goBack() = 0;
  virtual void goForward() = 0;

  virtual bool isLoading() = 0;
  virtual void reload() = 0;
  virtual void stopLoading() = 0;

  virtual void showDevTools() = 0;
  virtual void closeDevTools() = 0;

  virtual bool isMute() = 0;
  virtual void setMute(bool mute) = 0;

  virtual bool isOSREnabled() = 0;
  virtual void setOSREnabled(bool enabled) = 0;

  virtual bool postMessage(const QString& message) = 0;

 Q_SIGNALS:
  void titleChanged(QString title);
  void urlChanged(const QString& url);
  void loadStateChanged(bool isLoading);
  void fullscreenChanged(bool fullscreen);
  void newPopupWindow(const QString& url);
  void messageReceived(const QString& message);

 protected:
  BrowserEngine engine_;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QWebView::BrowserEngines)

QWEBVIEW_CORE_EXPORT QDebug operator<<(QDebug debug, QWebView::BrowserEngine engine);

#endif  // !QWEBVIEW_CORE_H_