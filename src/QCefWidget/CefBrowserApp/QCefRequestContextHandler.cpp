#include "QCefRequestContextHandler.h"

RequestContextHandler::RequestContextHandler() {}

RequestContextHandler::~RequestContextHandler() {}

bool RequestContextHandler::OnBeforePluginLoad(
  const CefString& mime_type,
  const CefString& plugin_url,
  bool is_main_frame,
  const CefString& top_origin_url,
  CefRefPtr<CefWebPluginInfo> plugin_info,
  PluginPolicy* plugin_policy) {
  if (mime_type == "application/pdf") {
    *plugin_policy = PLUGIN_POLICY_ALLOW;
    return true;
  }
  else if (mime_type == "application/x-shockwave-flash") {
    *plugin_policy = PLUGIN_POLICY_ALLOW;
    return true;
  }

  return false;
}
