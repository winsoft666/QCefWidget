/*
 * Copyright (c) winsoft666.
 * All rights reserved.
 *
 * This source code is licensed under the license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef QWEBVIEW_WEBVIEW2_H_
#define QWEBVIEW_WEBVIEW2_H_
#pragma once

#include "QWebView/Core.h"

#ifndef QWEBVIEW_WEBVIEW2_EXPORT
#ifdef QWEBVIEW_WEBVIEW2_LIB
#define QWEBVIEW_WEBVIEW2_EXPORT Q_DECL_EXPORT
#else
#define QWEBVIEW_WEBVIEW2_EXPORT Q_DECL_IMPORT
#endif
#endif

class QWebViewWebView2Impl;
class QWEBVIEW_WEBVIEW2_EXPORT QWebViewWebView2 : public QWebView {
  Q_OBJECT
 public:
  static bool QueryWebView2Version(QString& version);

  QWebViewWebView2(QWidget* parent = Q_NULLPTR);
  virtual ~QWebViewWebView2();

  virtual BrowserEngine browserEngine() const override;

  virtual void navigate(const QString& url) override;

  virtual bool canGoBack() override;
  virtual bool canGoForward() override;
  virtual void goBack() override;
  virtual void goForward() override;

  virtual bool isLoading() override;
  virtual void reload() override;
  virtual void stopLoading() override;

  virtual void showDevTools() override;
  virtual void closeDevTools() override;

  virtual bool isMute() override;
  virtual void setMute(bool mute) override;

  virtual bool isOSREnabled() override;
  virtual void setOSREnabled(bool enabled) override;

  virtual bool postMessage(const QString& message) override;

 Q_SIGNALS:

  void profileDeleted();

  void acceleratorKeyPressed(unsigned int key);

 private:
  std::unique_ptr<QWebViewWebView2Impl> impl_;
};

#endif  // !QWEBVIEW_WEBVIEW2_H_