#include "Include/QCefSetting.h"
#include "CefGlobalSetting.h"

void QCefSetting::setBrowserSubProcessPath(const QString &path) {
  CefGlobalSetting::initializeInstance();
  CefGlobalSetting::browser_sub_process_path.FromWString(path.toStdWString());
}

const QString QCefSetting::browserSubProcessPath() {
  CefGlobalSetting::initializeInstance();
  return QString::fromStdWString(CefGlobalSetting::browser_sub_process_path.ToWString());
}

void QCefSetting::setResourceDirectoryPath(const QString &path) {
  CefGlobalSetting::initializeInstance();
  CefGlobalSetting::resource_directory_path.FromWString(path.toStdWString());
}

const QString QCefSetting::resourceDirectoryPath() {
  CefGlobalSetting::initializeInstance();
  return QString::fromStdWString(CefGlobalSetting::resource_directory_path.ToWString());
}

void QCefSetting::setLocalesDirectoryPath(const QString &path) {
  CefGlobalSetting::initializeInstance();
  CefGlobalSetting::locales_directory_path.FromWString(path.toStdWString());
}

const QString QCefSetting::localesDirectoryPath() {
  CefGlobalSetting::initializeInstance();
  return QString::fromStdWString(CefGlobalSetting::locales_directory_path.ToWString());
}

void QCefSetting::setUserAgent(const QString &agent) {
  CefGlobalSetting::initializeInstance();
  CefGlobalSetting::user_agent.FromWString(agent.toStdWString());
}

const QString QCefSetting::userAgent() {
  CefGlobalSetting::initializeInstance();
  return QString::fromStdWString(CefGlobalSetting::user_agent.ToWString());
}

void QCefSetting::setCachePath(const QString &path) {
  CefGlobalSetting::initializeInstance();
  CefGlobalSetting::cache_path.FromWString(path.toStdWString());
}

const QString QCefSetting::cachePath() {
  CefGlobalSetting::initializeInstance();
  return QString::fromStdWString(CefGlobalSetting::cache_path.ToWString());
}

void QCefSetting::setUserDataPath(const QString &path) {
  CefGlobalSetting::initializeInstance();
  CefGlobalSetting::user_data_path.FromWString(path.toStdWString());
}

const QString QCefSetting::userDataPath() {
  CefGlobalSetting::initializeInstance();
  return QString::fromStdWString(CefGlobalSetting::user_data_path.ToWString());
}

void QCefSetting::setPersistSessionCookies(bool enabled) {
  CefGlobalSetting::initializeInstance();
  CefGlobalSetting::persist_session_cookies = enabled ? TRUE : FALSE;
}

const bool QCefSetting::persistSessionCookies() {
  CefGlobalSetting::initializeInstance();
  return CefGlobalSetting::persist_session_cookies ? true : false;
}

void QCefSetting::setPersistUserPreferences(bool enabled) {
  CefGlobalSetting::initializeInstance();
  CefGlobalSetting::persist_user_preferences = enabled ? TRUE : FALSE;
}

const bool QCefSetting::persistUserPreferences() {
  CefGlobalSetting::initializeInstance();
  return CefGlobalSetting::persist_user_preferences ? true : false;
}

void QCefSetting::setLocale(const QString &locale) {
  CefGlobalSetting::initializeInstance();
  CefGlobalSetting::locale.FromString(locale.toStdString());
}

const QString QCefSetting::locale() {
  CefGlobalSetting::initializeInstance();
  return QString::fromStdString(CefGlobalSetting::locale.ToString());
}

void QCefSetting::setRemoteDebuggingPort(int port) {
  CefGlobalSetting::initializeInstance();
  CefGlobalSetting::remote_debugging_port = port;
}

const int QCefSetting::remoteDebuggingPort() {
  CefGlobalSetting::initializeInstance();
  return CefGlobalSetting::remote_debugging_port;
}

void QCefSetting::setAcceptLanguageList(const QString &languages) {
  CefGlobalSetting::initializeInstance();
  CefGlobalSetting::accept_language_list.FromWString(languages.toStdWString());
}

const QString QCefSetting::acceptLanguageList() {
  CefGlobalSetting::initializeInstance();
  return QString::fromStdWString(CefGlobalSetting::accept_language_list.ToWString());
}

const void QCefSetting::setGpuEnabled(bool b) {
  CefGlobalSetting::initializeInstance();
  CefGlobalSetting::gpu_enabled = b;
}

const bool QCefSetting::gpuEnabled() {
  CefGlobalSetting::initializeInstance();
  return CefGlobalSetting::gpu_enabled;
}

void QCefSetting::setFlashPlugin(const QString &path, const QString &ver) {
  CefGlobalSetting::initializeInstance();
  CefGlobalSetting::flush_plugin_path.FromWString(path.toStdWString());
  CefGlobalSetting::flush_plugin_ver.FromWString(ver.toStdWString());
}

const QString QCefSetting::flashPluginPath() {
  CefGlobalSetting::initializeInstance();
  return QString::fromStdWString(CefGlobalSetting::flush_plugin_path.ToWString());
}

const QString QCefSetting::flashPluginVersion() {
  CefGlobalSetting::initializeInstance();
  return QString::fromStdWString(CefGlobalSetting::flush_plugin_ver.ToWString());
}

void QCefSetting::setHookTopLevelWindowCloseMessage(bool hook) {
  CefGlobalSetting::initializeInstance();
  CefGlobalSetting::hook_top_level_window_close_msg = hook;
}

const bool QCefSetting::isHookTopLevelWindowCloseMessage() {
  CefGlobalSetting::initializeInstance();
  return CefGlobalSetting::hook_top_level_window_close_msg;
}

void QCefSetting::setIgnoreTopLevelCloseMessage(bool ignore) {
  CefGlobalSetting::initializeInstance();
  CefGlobalSetting::ignore_top_level_window_close_msg = ignore;
}

const bool QCefSetting::isIgnoreTopLevelCloseMessage() {
  CefGlobalSetting::initializeInstance();
  return CefGlobalSetting::ignore_top_level_window_close_msg;
}
