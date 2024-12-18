/*
 * Copyright (c) winsoft666.
 * All rights reserved.
 *
 * This source code is licensed under the license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef ROOT_WINDOW_QT_H_
#define ROOT_WINDOW_QT_H_
#pragma once

#include <string>
#include <QRect>
#include <QWidget>
#include <QPointer>
#include "include/base/cef_scoped_ptr.h"
#include "tests/cefclient/browser/browser_window.h"
#include "tests/cefclient/browser/root_window.h"

namespace client {
class RootWindowQt;
class PopupWidget : public QWidget {
 public:
  PopupWidget(RootWindowQt* w, QWidget* parent = nullptr);

 protected:
  bool eventFilter(QObject* obj, QEvent* e) OVERRIDE;

 private:
  RootWindowQt* root_win_ = nullptr;
};

class RootWindowQt : public RootWindow, public BrowserWindow::Delegate, public QObject {
 public:
  class Delegate {
   public:
    virtual void OnJsNotify(const std::string& message) = 0;
    virtual void OnSetAddress(const std::string& url) = 0;
    virtual void OnSetTitle(const std::string& title) = 0;
    virtual void OnSetLoadingState(bool isLoading, bool canGoBack, bool canGoForward) = 0;
    virtual void OnSetFullscreen(bool fullscreen) = 0;

    virtual void OnBrowserCreated() = 0;
    virtual void OnWindowDestoryed() = 0;
    virtual void OnWindowAndBrowserDestoryed() = 0;
  };

  // Constructor may be called on any thread.
  RootWindowQt();
  ~RootWindowQt();

  // RootWindow methods.
  void Init(RootWindow::Delegate* delegate,
            const RootWindowConfig& config,
            const CefBrowserSettings& settings) OVERRIDE;
  void InitAsPopup(RootWindow::Delegate* delegate,
                   bool with_controls,
                   bool with_osr,
                   const CefPopupFeatures& popupFeatures,
                   CefWindowInfo& windowInfo,
                   CefRefPtr<CefClient>& client,
                   CefBrowserSettings& settings) OVERRIDE;
  void Show(ShowMode mode) OVERRIDE;
  void Hide() OVERRIDE;
  void SetBounds(int x, int y, size_t width, size_t height) OVERRIDE;
  void Close(bool force) OVERRIDE;
  void SetDeviceScaleFactor(float device_scale_factor) OVERRIDE;
  float GetDeviceScaleFactor() const OVERRIDE;
  CefRefPtr<CefBrowser> GetBrowser() const OVERRIDE;
  ClientWindowHandle GetWindowHandle() const OVERRIDE;
  bool WithWindowlessRendering() const OVERRIDE;
  bool WithExtension() const OVERRIDE;

  void SetOneTimeUrl(const std::string& url);
  void ShowDevTools();
  void CloseDevTools();

  void OnReceiveJsNotify(const std::string& message);

  inline void SetDelegate(RootWindowQt::Delegate* delgate) {
    root_win_qt_delegate_ = delgate;
  }

  bool isAllowBrowserClose();

 protected:
  bool eventFilter(QObject* obj, QEvent* e) OVERRIDE;

 private slots:
  void onScreenLogicalDotsPerInchChanged(qreal dpi);

 private:
  // Event handlers.
  void OnFocus();
  void OnActivate(bool active);
  void OnSize(bool minimized);
  void OnMove();
  void OnDpiChanged();
  void OnCreate();
  bool OnClose();

  // BrowserWindow::Delegate methods.
  void OnBrowserCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
  void OnBrowserWindowDestroyed() OVERRIDE;
  void OnSetAddress(const std::string& url) OVERRIDE;
  void OnSetTitle(const std::string& title) OVERRIDE;
  void OnSetFullscreen(bool fullscreen) OVERRIDE;
  void OnAutoResize(const CefSize& new_size) OVERRIDE;
  void OnSetLoadingState(bool isLoading,
                         bool canGoBack,
                         bool canGoForward) OVERRIDE;
  void OnSetDraggableRegions(
      const std::vector<CefDraggableRegion>& regions) OVERRIDE;

  void NotifyDestroyedIfDone();

  void CreateBrowserWindow(const std::string& startup_url);
  void CreateRootWindow(const CefBrowserSettings& settings,
                        bool initially_hidden,
                        QWidget* w);

  float GetWidgetScaleFactor() const;
  QRect GetWidgetScaledRealRect() const;

 private:
  bool with_osr_ = false;
  bool with_extension_ = false;
  bool is_popup_ = false;
  QRect start_rect_;
  scoped_ptr<BrowserWindow> browser_window_;
  CefBrowserSettings browser_settings_;
  bool initialized_ = false;

  QPointer<QWidget> widget_ = nullptr;

  bool window_destroyed_ = false;
  bool browser_destroyed_ = false;

  std::string one_time_url_;   // 一次性URL，记录在Browser创建之前用户设置的URL，延后到Browser创建后设置该URL
  RootWindowQt::Delegate* root_win_qt_delegate_ = nullptr;
  bool force_close_root_win_ = false;

  friend class PopupWidget;
  DISALLOW_COPY_AND_ASSIGN(RootWindowQt);
};
}  // namespace client

#endif  // !ROOT_WINDOW_QT_H_
