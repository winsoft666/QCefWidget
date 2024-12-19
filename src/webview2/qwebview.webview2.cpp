/*
 * Copyright (c) winsoft666.
 * All rights reserved.
 *
 * This source code is licensed under the license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include "qwebview.webview2.h"
#include "QWebViewWebView2Impl.h"
#include <QDebug>
#include <QApplication>
#include <wrl.h>
#include "wil/com.h"
#include "WebView2.h"
#include "QWebView/Manager.h"
#include "QWebView/Creator.h"

bool QWebViewWebView2::QueryWebView2Version(QString& version) {
  wil::unique_cotaskmem_string versionInfo;
  HRESULT hr = GetAvailableCoreWebView2BrowserVersionString(nullptr, &versionInfo);
  if (hr != S_OK || versionInfo == nullptr) {
    return false;
  }
  version = QString::fromStdWString(versionInfo.get());
  return true;
}

QWebViewWebView2::QWebViewWebView2(QWidget* parent) :
    QWebView(parent),
    impl_(new QWebViewWebView2Impl(this)) {
  qDebug() << ">>>> QWebViewWebView2 Ctor";
  Q_ASSERT(!qApp->quitOnLastWindowClosed());

  engine_ = BrowserEngine::WebView2;
}

QWebViewWebView2::~QWebViewWebView2() {
  qDebug() << ">>>> QWebViewWebView2 Dtor";
}

QWebView::BrowserEngine QWebViewWebView2::browserEngine() const {
  return engine_;
}

void QWebViewWebView2::navigate(const QString& url) {
  if (!impl_->IsInitialized()) {
    HRESULT hr = impl_->InitializeWebView();
    if (FAILED(hr)) {
      ShowFailure(hr, L"Initialize WebView2 failed");
    }
    else {
      QWebViewManager::Get()->add(this);
    }
  }

  impl_->Navigate(url.toStdWString());
}

bool QWebViewWebView2::canGoBack() {
  return impl_->CanGoBack();
}

bool QWebViewWebView2::canGoForward() {
  return impl_->CanGoForward();
}

void QWebViewWebView2::goBack() {
  impl_->GoBack();
}

void QWebViewWebView2::goForward() {
  impl_->GoForward();
}

bool QWebViewWebView2::isLoading() {
  return false;
}

void QWebViewWebView2::reload() {
  impl_->Reload();
}

void QWebViewWebView2::stopLoading() {
  impl_->StopLoading();
}

void QWebViewWebView2::showDevTools() {
  impl_->OpenDevToolsWindow();
}

void QWebViewWebView2::closeDevTools() {
}

bool QWebViewWebView2::isMute() {
  return impl_->IsMute();
}

void QWebViewWebView2::setMute(bool mute) {
  impl_->SetMute(mute);
}

bool QWebViewWebView2::isOSREnabled() {
  qWarning() << "QWebView: not support OSR in WebView2 engine.";
  return false;
}

void QWebViewWebView2::setOSREnabled(bool enabled) {
  qWarning() << "QWebView: not support OSR in WebView2 engine.";
}

bool QWebViewWebView2::postMessage(const QString& message) {
  HRESULT hr = impl_->PostStringWebMessage(message.toStdWString());
  CHECK_FAILURE(hr);

  return SUCCEEDED(hr);
}

QWebView* CreateWebView2(QWidget* parent) {
  return new QWebViewWebView2(parent);
}