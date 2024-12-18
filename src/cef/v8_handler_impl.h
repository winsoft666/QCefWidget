/*
 * Copyright (c) winsoft666.
 * All rights reserved.
 *
 * This source code is licensed under the license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef QWEBVIEW_CEF_V8_HANDLER_IMPL_H
#define QWEBVIEW_CEF_V8_HANDLER_IMPL_H
#pragma once

#include "include/cef_v8.h"
#include "tests/shared/renderer/client_app_renderer.h"

namespace client {
namespace renderer {
// 渲染进程：在Window对象上注册JS通知函数
void RegisterJSNotifyFunction(CefRefPtr<CefV8Context> context,
                              CefRefPtr<ClientAppRenderer::Delegate> delegate);
}  // namespace renderer

// 浏览器进程：处理发送到浏览器进程的消息
bool HandleProcessMessage(CefRefPtr<CefBrowser> browser, CefRefPtr<CefProcessMessage> message);
}  // namespace client

#endif  // !QWEBVIEW_CEF_V8_HANDLER_IMPL_H