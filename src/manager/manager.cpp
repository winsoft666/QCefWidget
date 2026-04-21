/*
 * Copyright (c) winsoft666.
 * All rights reserved.
 *
 * This source code is licensed under the license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include "QWebView/Manager.h"
#include <QApplication>
#include <QEvent>
#include <QDebug>
#include <QTimer>
#include "top_level_window_util.h"
#include "QWebView/ManagerPrivate.h"

QWebViewManager* QWebViewManager::Get() {
  static QWebViewManager manager;
  return &manager;
}

QWebViewManager::TopLevelWndCloseState QWebViewManager::topLevelWinCloseState(QWidget* topLevel) const {
  if (d_->topLevelDataMap_.contains(topLevel))
    return d_->topLevelDataMap_[topLevel].CloseState;

  return TopLevelWndCloseState::NotStart;
}

QWebViewManagerPrivate* QWebViewManager::privatePointer() {
  return d_.get();
}

QWebViewManager::QWebViewManager(QObject* parent /*= Q_NULLPTR*/) :
    QObject(parent),
    d_(new QWebViewManagerPrivate()) {
#ifdef QT_DEBUG
  qDebug() << ">>>> QWebViewManager Ctor";
#endif
}

QWebViewManager::~QWebViewManager() {
#ifdef QT_DEBUG
  qDebug() << ">>>> QWebViewManager Dtor";
#endif
}

void QWebViewManager::prepareToCloseTopLevelWindow(QWidget* topLevel) {
  d_->prepareToCloseTopLevelWindow(topLevel);
}

bool QWebViewManager::eventFilter(QObject* obj, QEvent* e) {
  QWidget* widget = static_cast<QWidget*>(obj);
  Q_ASSERT(widget);
  if (!widget) {
    return false;
  }

  //QEvent::Type event_type = e->type();
  //if (event_type == QEvent::ParentChange) {
  //  if (widget->property("QWebView").toBool()) {  // QWebView�ĸ����ڱ仯
  //    QWebView* webview = (QWebView*)widget;

  //    QWidget* newTopLevel = GetCefTopLevelWindow(webview);
  //    newTopLevel->installEventFilter(this);

  //    filterWebViews_[webview] = newTopLevel;
  //  }
  //  else {  // �������ڵĸ����ڱ仯
  //    widget->removeEventFilter(this);

  //    for (QMap<QWebView*, QWidget*>::iterator it = filterWebViews_.begin(); it != filterWebViews_.end(); it++) {
  //      if (it.value() == widget) {
  //        QWidget* newTopLevel = GetCefTopLevelWindow(it.key());
  //        //newTopLevel->installEventFilter(this);

  //        filterWebViews_[it.key()] = newTopLevel;
  //        break;
  //      }
  //    }
  //  }
  //}
  return false;
}

QDebug operator<<(QDebug debug, QWebViewManager::TopLevelWndCloseState state) {
  switch (state) {
    case QWebViewManager::TopLevelWndCloseState::NotStart:
      debug << "QWebViewManager::TopLevelWndCloseState::NotStart";
      break;
    case QWebViewManager::TopLevelWndCloseState::BrowsersClosing:
      debug << "QWebViewManager::TopLevelWndCloseState::BrowsersClosing";
      break;
    case QWebViewManager::TopLevelWndCloseState::BrowsersClosed:
      debug << "QWebViewManager::TopLevelWndCloseState::BrowsersClosed";
      break;
    case QWebViewManager::TopLevelWndCloseState::Closed:
      debug << "QWebViewManager::TopLevelWndCloseState::Closed";
      break;
  }
  return debug;
}
