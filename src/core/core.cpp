/*
 * Copyright (c) winsoft666.
 * All rights reserved.
 *
 * This source code is licensed under the license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "QWebView/Core.h"

QWebView::QWebView(QWidget* parent /*= Q_NULLPTR*/) :
    QWidget(parent),
    engine_(QWebView::BrowserEngine::None) {
}

QWebView::BrowserEngine QWebView::browserEngine() const {
  return engine_;
}

QDebug operator<<(QDebug debug, QWebView::BrowserEngine engine) {
  switch (engine) {
    case QWebView::BrowserEngine::None:
      debug << "QWebView::BrowserEngine::None";
      break;
    case QWebView::BrowserEngine::WebView2:
      debug << "QWebView::BrowserEngine::WebView2";
      break;
    case QWebView::BrowserEngine::CEF:
      debug << "QWebView::BrowserEngine::CEF";
      break;
  }
  return debug;
}
