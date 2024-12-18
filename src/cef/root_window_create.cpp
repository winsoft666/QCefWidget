// Copyright (c) 2016 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "tests/cefclient/browser/root_window.h"

#if defined(OS_WIN) || defined(OS_LINUX)
#include "tests/cefclient/browser/root_window_views.h"
#endif

#include "root_window_qt.h"

namespace client {
// static
// winsoft666: 实现browser/root_window.h中的RootWindow::Create方法，创建RootWindowQt窗口
//
scoped_refptr<RootWindow> RootWindow::Create(bool use_views) {
  if (use_views) {
#if defined(OS_WIN) || defined(OS_LINUX)
    return new RootWindowViews();
#else
    LOG(FATAL) << "Views framework is not supported on this platform.";
#endif
  }

  return new RootWindowQt();
}
}  // namespace client
