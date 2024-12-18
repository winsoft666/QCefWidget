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

QWebViewManager* QWebViewManager::Get() {
  static QWebViewManager manager;
  return &manager;
}

QWebViewManager::TopLevelWndCloseState QWebViewManager::topLevelWinCloseState(QWidget* topLevel) const {
  if (topLevelDataMap_.contains(topLevel))
    return topLevelDataMap_[topLevel].CloseState;

  return TopLevelWndCloseState::NotStart;
}

void QWebViewManager::setCefCanUnInitialize() {
  cefCanUnInitialize_ = true;

  checkAllWebViewsClosed();
}

void QWebViewManager::add(QWebView* webview) {
  QWidget* topLevel = GetTopLevelWindow(webview);
  Q_ASSERT(topLevel);
  Q_ASSERT(!topLevel->testAttribute(Qt::WA_DeleteOnClose));

  if (webview->browserEngine() == QWebView::BrowserEngine::CEF) {
    cefCanUnInitialize_ = false;
  }

  WebViewData wvd;
  wvd.WebView = webview;

  if (topLevelDataMap_.contains(topLevel)) {
    topLevelDataMap_[topLevel].WebviewColl.push_back(wvd);
  }
  else {
    TopLevelWndData data;
    data.WebviewColl.push_back(wvd);

    topLevelDataMap_[topLevel] = data;
  }
}

void QWebViewManager::remove(QWebView* webview) {
  QWidget* topLevel = GetTopLevelWindow(webview);
  Q_ASSERT(topLevel);
  Q_ASSERT(topLevelDataMap_.contains(topLevel));

  if (topLevelDataMap_.contains(topLevel)) {
    auto& coll = topLevelDataMap_[topLevel].WebviewColl;
    for (QVector<WebViewData>::iterator it = coll.begin(); it != coll.end(); ++it) {
      if (it->WebView == webview) {
        coll.erase(it);
        break;
      }
    }
  }

  checkAllWebViewsClosed();
}

void QWebViewManager::prepareToCloseTopLevelWindow(QWidget* topLevel) {
  qDebug() << ">>>> QWebViewManager::prepareToCloseTopLevelWindow" << topLevel;

  if (topLevelDataMap_.contains(topLevel)) {
    if (topLevelDataMap_[topLevel].CloseState == TopLevelWndCloseState::NotStart) {
      topLevelDataMap_[topLevel].CloseState = TopLevelWndCloseState::BrowsersClosing;

      auto& coll = topLevelDataMap_[topLevel].WebviewColl;
      for (QVector<WebViewData>::iterator it = coll.begin(); it != coll.end(); ++it) {
        if (!it->BrowserClosed) {
          Q_ASSERT(it->WebView);
          if (it->WebView->browserEngine() == QWebView::BrowserEngine::CEF) {
            qDebug() << ">>>>     Call QWidget:close()" << it->WebView;
            QWidget* w = it->WebView;
            QTimer::singleShot(1, [w]() {
              w->close();
            });
          }
          else if (it->WebView->browserEngine() == QWebView::BrowserEngine::WebView2) {
            it->BrowserClosed = true;
            QWebView* webview = it->WebView;

            // WebView2 不会给顶级窗口发送 WM_CLOSE 消息，为了保持和 CEF 流程一直，模拟发送 CloseEvent 到顶级窗口
            QTimer::singleShot(1, [topLevel, webview, this]() {
              topLevel->close();
              setWebViewClosed(topLevel, webview);
              remove(webview);
            });
          }
        }
      }
    }

    checkBrowsersCloseState(topLevel);
  }
}

QWebViewManager::QWebViewManager(QObject* parent /*= Q_NULLPTR*/) :
    QObject(parent) {
  qDebug() << ">>>> QWebViewManager Ctor";
}

QWebViewManager::~QWebViewManager() {
  qDebug() << ">>>> QWebViewManager Dtor";
}

void QWebViewManager::setWebViewClosed(QWidget* topLevel, QWebView* webview) {
  qDebug() << ">>>> QWebViewManager::setCefWebViewClosed" << webview;

  if (!topLevel)
    topLevel = GetTopLevelWindow(webview);
  Q_ASSERT(topLevel);
  Q_ASSERT(topLevelDataMap_.contains(topLevel));

  if (topLevelDataMap_.contains(topLevel)) {
    auto& coll = topLevelDataMap_[topLevel].WebviewColl;
    for (QVector<WebViewData>::iterator it = coll.begin(); it != coll.end(); ++it) {
      if (it->WebView == webview) {
        it->BrowserClosed = true;
        break;
      }
    }

    checkBrowsersCloseState(topLevel);
  }
}

void QWebViewManager::sendCloseEventToTopLevel(QWebView* webview) {
  QWidget* topLevel = GetTopLevelWindow(webview);
  QTimer::singleShot(1, [topLevel]() {
    if (topLevel)
      topLevel->close();
  });
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

void QWebViewManager::checkBrowsersCloseState(QWidget* topLevel) {
  qDebug() << ">>>> QWebViewManager::checkBrowsersCloseState" << topLevel;

  if (topLevelDataMap_.contains(topLevel)) {
    auto& coll = topLevelDataMap_[topLevel].WebviewColl;

    bool isAllClosed = true;
    for (QVector<WebViewData>::iterator it = coll.begin(); it != coll.end(); ++it) {
      if (!it->BrowserClosed) {
        isAllClosed = false;
        break;
      }
    }

    if (isAllClosed) {
      qDebug() << ">>>>    All Browsers Closed";
      topLevelDataMap_[topLevel].CloseState = TopLevelWndCloseState::BrowsersClosed;
    }
  }
}

void QWebViewManager::checkAllWebViewsClosed() {
  if (!cefCanUnInitialize_)
    return;

  bool allEmpty = true;
  for (auto it = topLevelDataMap_.cbegin(); it != topLevelDataMap_.cend(); ++it) {
    if (!it.value().WebviewColl.isEmpty()) {
      allEmpty = false;
      break;
    }
  }

  if (!allEmpty)
    return;

  emit allWebViewsClosed();
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
