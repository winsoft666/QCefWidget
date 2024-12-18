/*
 * Copyright (c) winsoft666.
 * All rights reserved.
 *
 * This source code is licensed under the license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef QWEBVIEW_MANAGER_H_
#define QWEBVIEW_MANAGER_H_
#pragma once

#include <QObject>
#include <QSet>
#include <QMap>
#include <QVector>
#include <QDebug>
#include "QWebView/Core.h"
#include <QtCore/qglobal.h>

#ifndef QWEBVIEW_MANAGER_EXPORT
#ifdef QWEBVIEW_MANAGER_LIB
#define QWEBVIEW_MANAGER_EXPORT Q_DECL_EXPORT
#else
#define QWEBVIEW_MANAGER_EXPORT Q_DECL_IMPORT
#endif
#endif

// 虽然WebView2内核在关闭时无需特殊的处理流程，但为了使WebView2和CEF有相同的使用流程，仍然统一使用QWebViewManager进行管理
//
class QWEBVIEW_MANAGER_EXPORT QWebViewManager : public QObject {
  Q_OBJECT
 public:
  enum class TopLevelWndCloseState {
    NotStart = 0,
    BrowsersClosing = 1,
    BrowsersClosed = 2,  // 所有浏览器已关闭
    Closed = 3
  };

  struct WebViewData {
    QWebView* WebView = nullptr;
    bool BrowserClosed = false;
  };

  struct TopLevelWndData {
    TopLevelWndCloseState CloseState = TopLevelWndCloseState::NotStart;
    QVector<WebViewData> WebviewColl;
  };

  static QWebViewManager* Get();

  TopLevelWndCloseState topLevelWinCloseState(QWidget* topLevel) const;

  void setCefCanUnInitialize();
  void setWebViewClosed(QWidget* topLevel, QWebView* webview);
  void sendCloseEventToTopLevel(QWebView* webview);
 signals:
  void allWebViewsClosed();

 public slots:
  void prepareToCloseTopLevelWindow(QWidget* topLevel);
  void add(QWebView* webview);
  void remove(QWebView* webview);

 protected:
  bool eventFilter(QObject* obj, QEvent* e) override;
  void checkBrowsersCloseState(QWidget* topLevel);
  void checkAllWebViewsClosed();

 protected:
  QWebViewManager(QObject* parent = Q_NULLPTR);
  ~QWebViewManager();

  bool cefCanUnInitialize_ = true;
  QMap<QWidget*, TopLevelWndData> topLevelDataMap_;
};

QWEBVIEW_MANAGER_EXPORT QDebug operator<<(QDebug debug, QWebViewManager::TopLevelWndCloseState state);
#endif // !QWEBVIEW_MANAGER_H_