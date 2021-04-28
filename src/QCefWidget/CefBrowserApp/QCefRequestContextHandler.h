#ifndef QCEF_REQUEST_CONTEXT_HANDLER_H_
#define QCEF_REQUEST_CONTEXT_HANDLER_H_
#pragma once

#include "include/cef_request_context.h"
#include "include/cef_request_context_handler.h"
#include "Include/QCefVersion.h"

class RequestContextHandler : public CefRequestContextHandler {
public:
  RequestContextHandler();
  ~RequestContextHandler();
  bool OnBeforePluginLoad(const CefString& mime_type,
                          const CefString& plugin_url,
                          bool is_main_frame,
                          const CefString& top_origin_url,
                          CefRefPtr<CefWebPluginInfo> plugin_info,
                          PluginPolicy* plugin_policy) override;
private:
  IMPLEMENT_REFCOUNTING(RequestContextHandler);
  DISALLOW_COPY_AND_ASSIGN(RequestContextHandler);
};
#endif // !QCEF_REQUEST_CONTEXT_HANDLER_H_