#include "QCefGlobalSetting.h"
#include <QCoreApplication>
#include <QDir>
#include <QCefProtocol.h>


CefString QCefGlobalSetting::browser_sub_process_path;
CefString QCefGlobalSetting::resource_directory_path;
CefString QCefGlobalSetting::locales_directory_path;
CefString QCefGlobalSetting::flush_plugin_path;
CefString QCefGlobalSetting::flush_plugin_ver;
CefString QCefGlobalSetting::user_agent;
CefString QCefGlobalSetting::cache_path;
CefString QCefGlobalSetting::user_data_path;
CefString QCefGlobalSetting::locale;
CefString QCefGlobalSetting::accept_language_list;
int QCefGlobalSetting::persist_session_cookies = 1;
int QCefGlobalSetting::persist_user_preferences = 1;
int QCefGlobalSetting::remote_debugging_port = 0;
bool QCefGlobalSetting::gpu_enabled = true;
bool QCefGlobalSetting::hook_top_level_window_close_msg = true;
bool QCefGlobalSetting::ignore_top_level_window_close_msg = false;

void QCefGlobalSetting::initializeInstance() { static QCefGlobalSetting s_instance; }

QCefGlobalSetting::QCefGlobalSetting() {
  QDir ExeDir = QCoreApplication::applicationDirPath();

  QString strExePath = ExeDir.filePath(RENDER_PROCESS_NAME);
  browser_sub_process_path.FromString(QDir::toNativeSeparators(strExePath).toStdString());

  QString strResPath = ExeDir.filePath(RESOURCE_DIRECTORY_NAME);
  resource_directory_path.FromString(QDir::toNativeSeparators(strResPath).toStdString());

  QDir ResPath(strResPath);
  locales_directory_path.FromString(
      QDir::toNativeSeparators(ResPath.filePath(LOCALES_DIRECTORY_NAME)).toStdString());

  user_agent.FromString(QCEF_USER_AGENT);

  accept_language_list.FromWString(L"zh-CN,en-US,en,en-GB");
  locale.FromWString(L"zh-CN");

  gpu_enabled = true;
}

QCefGlobalSetting::~QCefGlobalSetting() {}
