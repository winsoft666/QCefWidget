/*
 * Copyright (c) winsoft666.
 * All rights reserved.
 *
 * This source code is licensed under the license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef QWEBVIEW_CEF_H_
#define QWEBVIEW_CEF_H_
#pragma once

#include <QWidget>
#include <QVariantList>
#include <QAbstractNativeEventFilter>
#include "QWebView/Core.h"

#ifndef QWEBVIEW_CEF_EXPORT
#ifdef QWEBVIEW_CEF_LIB
#define QWEBVIEW_CEF_EXPORT Q_DECL_EXPORT
#else
#define QWEBVIEW_CEF_EXPORT Q_DECL_IMPORT
#endif
#endif


// Since CEF uses old Windows system version acquisition API, these API will always return the Windows 8 version (6.2) on Windows 8.1+ systems.
// For more details, please refer to: https://learn.microsoft.com/zh-cn/windows/win32/sysinfo/targeting-your-application-at-windows-8-1
// This will cause CEF to trigger assertions and crash.
// Therefore, it is necessary to embed a .manifest file (see compatibility.manifest) in the executable file to be compatible with systems of Windows 8.1 and above.
//
class QWEBVIEW_CEF_EXPORT QWebViewCEF : public QWebView {
  Q_OBJECT
 public:
  static bool Initialize(bool supportOSR);
  static void UnInitialize();

  QWebViewCEF(QWidget* parent = nullptr);
  virtual ~QWebViewCEF();

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

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

#endif  // QWEBVIEW_CEF_H_