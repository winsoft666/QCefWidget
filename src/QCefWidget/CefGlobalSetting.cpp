#include "CefGlobalSetting.h"
#include <QCoreApplication>
#include <QDir>
#include <QCefProtocol.h>


CefString CefGlobalSetting::browser_sub_process_path;
CefString CefGlobalSetting::resource_directory_path;
CefString CefGlobalSetting::locales_directory_path;
CefString CefGlobalSetting::flush_plugin_path;
CefString CefGlobalSetting::flush_plugin_ver;
CefString CefGlobalSetting::user_agent;
CefString CefGlobalSetting::cache_path;
CefString CefGlobalSetting::user_data_path;
CefString CefGlobalSetting::locale;
CefString CefGlobalSetting::accept_language_list;
int CefGlobalSetting::persist_session_cookies = 0;
int CefGlobalSetting::persist_user_preferences = 0;
int CefGlobalSetting::remote_debugging_port = 0;
bool CefGlobalSetting::gpu_enabled = false;
bool CefGlobalSetting::hook_top_level_window_close_msg = true;
bool CefGlobalSetting::ignore_top_level_window_close_msg = false;

void CefGlobalSetting::initializeInstance() { static CefGlobalSetting s_instance; }

CefGlobalSetting::CefGlobalSetting() {
  QDir ExeDir = QCoreApplication::applicationDirPath();

  QString strExePath = ExeDir.filePath(RENDER_PROCESS_NAME);
  browser_sub_process_path.FromString(QDir::toNativeSeparators(strExePath).toStdString());

  QString strResPath = ExeDir.filePath(RESOURCE_DIRECTORY_NAME);
  resource_directory_path.FromString(QDir::toNativeSeparators(strResPath).toStdString());

  QDir ResPath(strResPath);
  locales_directory_path.FromString(
      QDir::toNativeSeparators(ResPath.filePath(LOCALES_DIRECTORY_NAME)).toStdString());

  user_agent.FromString(QCEF_USER_AGENT);

  accept_language_list.FromWString(L"zh-CN,en-US,en");
  locale.FromWString(L"zh-CN");

  gpu_enabled = false;
}

CefGlobalSetting::~CefGlobalSetting() {}
