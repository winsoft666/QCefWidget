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
#include "QWebView/ManagerPrivate.h"
#include "QWebView/Creator.h"
#include <QHBoxLayout>

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
#ifdef QT_DEBUG
  qDebug() << ">>>> QWebViewWebView2 Ctor";
#endif
  Q_ASSERT(!qApp->quitOnLastWindowClosed());

  engine_ = BrowserEngine::WebView2;

  impl_->RegisterSciptCallNotify([this](const QVariant& value, CallbackToScript cbScript) {
    emit messageReceived(value.toString());
    return QVariant();
  });
}

QWebViewWebView2::~QWebViewWebView2() {
#ifdef QT_DEBUG
  qDebug() << ">>>> QWebViewWebView2 Dtor";
#endif
}

void QWebViewWebView2::navigate(const QString& url) {
  if (!impl_->IsInitialized()) {
    HRESULT hr = impl_->InitializeWebView();
    if (FAILED(hr)) {
      ShowFailure(hr, L"Initialize WebView2 failed");
    }
    else {
      QWebViewManager::Get()->privatePointer()->add(this);
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

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
bool QWebViewWebView2::nativeEvent(const QByteArray& eventType, void* message, qintptr* result) {
#else
bool QWebViewWebView2::nativeEvent(const QByteArray& eventType, void* message, long* result) {
#endif
  if (eventType == "windows_generic_MSG") {
    MSG* pMsg = (MSG*)message;
    if (!pMsg) {
      return false;
    }

    HWND hWnd = pMsg->hwnd;
    UINT message = pMsg->message;
    DWORD wParam = pMsg->wParam;
    DWORD lParam = pMsg->lParam;

    // Give all components a chance to handle the message first.
    LRESULT lret = 0;
    if (impl_->ComponentsHandleWindowMessage(hWnd, message, wParam, lParam, &lret)) {
      return false;
    }

    switch (message) {
      case WM_SIZE: {
        // Don't resize the app or webview when the app is minimized, let WM_SYSCOMMAND to handle it
        if (lParam != 0) {
          impl_->ResizeEverything(LOWORD(lParam), HIWORD(lParam));
        }
        break;
      }
      case WM_DPICHANGED: {
        if (auto view = impl_->GetComponent<ViewComponent>()) {
          view->UpdateDpiAndTextScale();
        }

        RECT* const newWindowSize = reinterpret_cast<RECT*>(lParam);
        SetWindowPos(
            hWnd, nullptr, newWindowSize->left, newWindowSize->top,
            newWindowSize->right - newWindowSize->left,
            newWindowSize->bottom - newWindowSize->top, SWP_NOZORDER | SWP_NOACTIVATE);
        return true;
      }
      case WUM_ASYNC_TASK:
        auto* task = reinterpret_cast<std::function<void()>*>(wParam);
        (*task)();
        delete task;
        return true;
    }
    return false;
  }
  return false;
}

void QWebViewWebView2::closeEvent(QCloseEvent* e) {
  impl_->UninitializeWebView();
  QWebView::closeEvent(e);
}

void QWebViewWebView2::onNewWindow(QString url) {
  emit newPopupWindow(url);
}

QWebView* CreateWebView2(QWidget* parent) {
  return new QWebViewWebView2(parent);
}