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
CefString QCefGlobalSetting::debug_log_path;
bool QCefGlobalSetting::persist_session_cookies = false;
bool QCefGlobalSetting::persist_user_preferences = true;
int QCefGlobalSetting::remote_debugging_port = 0;
bool QCefGlobalSetting::gpu_enabled = true;
bool QCefGlobalSetting::osr_enabled = true;
QMap<QString, QPair<int, QString>> QCefGlobalSetting::resource_map;

void QCefGlobalSetting::initializeInstance() {
  static QCefGlobalSetting s_instance;
}

QCefGlobalSetting::QCefGlobalSetting() {
  QDir exeDir = QCoreApplication::applicationDirPath();

  QString strCachePath = exeDir.filePath(WEB_CACHE_DIRECTORY_NAME);
  cache_path.FromWString(QDir::toNativeSeparators(strCachePath).toStdWString());

  QString strExePath = exeDir.filePath(RENDER_PROCESS_NAME);
  browser_sub_process_path.FromWString( QDir::toNativeSeparators(strExePath).toStdWString());

  QString strResPath = exeDir.filePath(RESOURCE_DIRECTORY_NAME);
  resource_directory_path.FromWString( QDir::toNativeSeparators(strResPath).toStdWString());

  QDir ResPath(strResPath);
  locales_directory_path.FromWString( QDir::toNativeSeparators(ResPath.filePath(LOCALES_DIRECTORY_NAME)) .toStdWString());

  user_agent.FromString(QCEF_USER_AGENT);

  accept_language_list.FromWString(L"zh-CN,en-US,en,en-GB");
  locale.FromWString(L"zh-CN");

  QString debugLogPath = exeDir.filePath(DEBUG_LOG_NAME);
  debug_log_path.FromWString( QDir::toNativeSeparators(debugLogPath).toStdWString());
}

QCefGlobalSetting::~QCefGlobalSetting() {}
