#include "main_message_loop_qt.h"
#include "include/cef_app.h"
#include <QApplication>
#include "QWebView/Manager.h"

MainMessageLoopQt::MainMessageLoopQt() :
    thread_id_(base::PlatformThread::CurrentId()) {
}

int MainMessageLoopQt::Run() {
  return 0;
}

void MainMessageLoopQt::Quit() {
  //if (!CURRENTLY_ON_MAIN_THREAD()) {
  //  MAIN_POST_CLOSURE(base::Bind(&MainMessageLoopQt::Quit, base::Unretained(this)));
  //  return;
  //}

  QWebViewManager::Get()->setCefCanUnInitialize();
}

void MainMessageLoopQt::PostTask(CefRefPtr<CefTask> task) {
  QApplication::postEvent(this, new TaskEvent(task));
}

bool MainMessageLoopQt::RunsTasksOnCurrentThread() const {
  return (thread_id_ == base::PlatformThread::CurrentId());
}

#if defined(OS_WIN)
void MainMessageLoopQt::SetCurrentModelessDialog(HWND hWndDialog) {
  // Nothing to do here. The Chromium message loop implementation will
  // internally route dialog messages.
}
#endif

void MainMessageLoopQt::customEvent(QEvent* event) {
  if (event->type() == TaskEvent::kType) {
    TaskEvent* taskEvent = reinterpret_cast<TaskEvent*>(event);
    if (taskEvent) {
      CefRefPtr<CefTask> t = taskEvent->cefTask;
      if (t) {
        t->Execute();
      }
    }
  }
}
