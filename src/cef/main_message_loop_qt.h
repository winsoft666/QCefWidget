/*
 * Copyright (c) winsoft666.
 * All rights reserved.
 *
 * This source code is licensed under the license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef QWEBVIEW_CEF_MAIN_MESSAGE_LOOP_QT_H_
#define QWEBVIEW_CEF_MAIN_MESSAGE_LOOP_QT_H_
#pragma once

#include <QObject>
#include <QEvent>
#include "tests/shared/browser/main_message_loop.h"

using namespace client;

class TaskEvent final : public QEvent {
 public:
  static const int kType = QEvent::User + 1;

  explicit TaskEvent(CefRefPtr<CefTask> task) :
      QEvent(QEvent::Type(kType)),
      cefTask(task) {
    // Nothing
  }

  CefRefPtr<CefTask> cefTask;

 private:
  DISALLOW_COPY_AND_ASSIGN(TaskEvent);
};

class MainMessageLoopQt : public QObject, public MainMessageLoop {
 public:
  MainMessageLoopQt();

  // MainMessageLoop methods.
  int Run() OVERRIDE;
  void Quit() OVERRIDE;
  void PostTask(CefRefPtr<CefTask> task) OVERRIDE;
  bool RunsTasksOnCurrentThread() const OVERRIDE;

#if defined(OS_WIN)
  void SetCurrentModelessDialog(HWND hWndDialog) OVERRIDE;
#endif

 protected:
  void customEvent(QEvent* event) final;

 private:
  base::PlatformThreadId thread_id_;

  DISALLOW_COPY_AND_ASSIGN(MainMessageLoopQt);
};
#endif  // !QWEBVIEW_CEF_MAIN_MESSAGE_LOOP_QT_H_
