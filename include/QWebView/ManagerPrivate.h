/*
 * Copyright (c) winsoft666.
 * All rights reserved.
 *
 * This source code is licensed under the license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef QWEBVIEW_MANAGER_PRIVATE_H_
#define QWEBVIEW_MANAGER_PRIVATE_H_
#pragma once

#include <QObject>
#include <QSet>
#include <QMap>
#include <QWidget>
#include <QVector>
#include <QDebug>
#include "QWebView/Manager.h"

class QWebView;
class QWEBVIEW_MANAGER_EXPORT QWebViewManagerPrivate : public QObject {
  Q_OBJECT
 public:
  struct WebViewData {
    QWebView* WebView = nullptr;
    bool BrowserClosed = false;
  };

  struct TopLevelWndData {
    QWebViewManager::TopLevelWndCloseState CloseState = QWebViewManager::TopLevelWndCloseState::NotStart;
    QVector<WebViewData> WebviewColl;
  };

  QWebViewManagerPrivate();
 public slots:
  void add(QWebView* webview);
  void remove(QWebView* webview);
  void setCefCanUnInitialize();
  void setWebViewClosed(QWidget* topLevel, QWebView* webview);
  void sendCloseEventToTopLevel(QWebView* webview);

 private:
  void prepareToCloseTopLevelWindow(QWidget* topLevel);
  void checkBrowsersCloseState(QWidget* topLevel);
  void checkAllWebViewsClosed();

 private:
  bool cefCanUnInitialize_ = true;
  QMap<QWidget*, TopLevelWndData> topLevelDataMap_;

  friend class QWebViewManager;
};

#endif  // !QWEBVIEW_MANAGER_PRIVATE_H_