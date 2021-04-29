#ifndef QCEF_GLOBAL_SETTING_H__
#define QCEF_GLOBAL_SETTING_H__
#pragma once

#include <include\cef_base.h>
#include <QMap>

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
  static CefString debug_log_path;
  static bool persist_session_cookies;
  static bool persist_user_preferences;
  static int remote_debugging_port;
  static bool gpu_enabled;
  static bool osr_enabled;
  static QMap<QString, QPair<int, QString>> resource_map;

 protected:
  QCefGlobalSetting();
  ~QCefGlobalSetting();
};
#endif