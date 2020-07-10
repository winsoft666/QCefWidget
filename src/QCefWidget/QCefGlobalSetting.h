#ifndef QCEF_GLOBAL_SETTING_H__
#define QCEF_GLOBAL_SETTING_H__
#pragma once

#include <include\cef_base.h>

class QCefGlobalSetting {
public:
  static void initializeInstance();

public:
  static CefString browser_sub_process_path;
  static CefString resource_directory_path;
  static CefString locales_directory_path;
  static CefString user_agent;
  static CefString cache_path;
  static CefString user_data_path;
  static CefString locale;
  static CefString accept_language_list;
  static CefString flush_plugin_path;
  static CefString flush_plugin_ver;
  static int persist_session_cookies;
  static int persist_user_preferences;
  static int remote_debugging_port;
  static bool gpu_enabled;
protected:
  QCefGlobalSetting();
  ~QCefGlobalSetting();
};
#endif