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

//
// CEF 使用了老式的 Windows 系统版本获取函数，这些函数在 Windows 8.1+ 系统上会始终返回 Windows 8 版本 (6.2)。
// 详见：https://learn.microsoft.com/zh-cn/windows/win32/sysinfo/targeting-your-application-at-windows-8-1
//
// 这样会导致 CEF 触发断言而崩溃，因此需要在可执行文件中嵌入.manifest清单（见Sample.manifest），兼容Windows 8.1及以上版本的系统。
//
class QWEBVIEW_CEF_EXPORT QWebViewCEF : public QWebView {
  Q_OBJECT
 public:
  static bool Initialize(bool supportOSR);
  static void UnInitialize();

  QWebViewCEF(QWidget* parent = nullptr);
  virtual ~QWebViewCEF();

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

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

#endif  // QWEBVIEW_CEF_H_