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
// Rendering process: Register the value of IsCEFEngine on the Window object.
void RegisterCEFEngineFlag(CefRefPtr<CefV8Context> context, CefRefPtr<ClientAppRenderer::Delegate> delegate);

// Rendering process: Register a JavaScript notification function on the Window object.
void RegisterJSNotifyFunction(CefRefPtr<CefV8Context> context,
                              CefRefPtr<ClientAppRenderer::Delegate> delegate);
}  // namespace renderer

// Browser process: Handle messages sent to the browser process.
bool HandleProcessMessage(CefRefPtr<CefBrowser> browser, CefRefPtr<CefProcessMessage> message);
}  // namespace client

#endif  // !QWEBVIEW_CEF_V8_HANDLER_IMPL_H