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

class QWebViewManagerPrivate;


// Although the WebView2 kernel doesn't necessitate a specific shutdown procedure, 
// to ensure that the usage processes of WebView2 and CEF are identical, 
// they are still managed uniformly through QWebViewManager.
//
class QWEBVIEW_MANAGER_EXPORT QWebViewManager : public QObject {
  Q_OBJECT
 public:
  enum class TopLevelWndCloseState {
    NotStart = 0,
    BrowsersClosing = 1,
    BrowsersClosed = 2,  // All Browsers have closed
    Closed = 3
  };

  static QWebViewManager* Get();

  TopLevelWndCloseState topLevelWinCloseState(QWidget* topLevel) const;

  QWebViewManagerPrivate* privatePointer();

 public slots:
  void prepareToCloseTopLevelWindow(QWidget* topLevel);

 signals:
  void allWebViewsClosed();

 protected:
  bool eventFilter(QObject* obj, QEvent* e) override;

 protected:
  QWebViewManager(QObject* parent = Q_NULLPTR);
  ~QWebViewManager();

  std::unique_ptr<QWebViewManagerPrivate> d_;
};

QWEBVIEW_MANAGER_EXPORT QDebug operator<<(QDebug debug, QWebViewManager::TopLevelWndCloseState state);
#endif  // !QWEBVIEW_MANAGER_H_