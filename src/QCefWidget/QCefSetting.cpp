#include "Include/QCefSetting.h"
#include "QCefGlobalSetting.h"
#include <QDir>

void QCefSetting::setBrowserSubProcessPath(const QString& path) {
  QCefGlobalSetting::initializeInstance();
  QCefGlobalSetting::browser_sub_process_path.FromWString( QDir::toNativeSeparators(path).toStdWString());
}

const QString QCefSetting::browserSubProcessPath() {
  QCefGlobalSetting::initializeInstance();
  return QString::fromStdWString( QCefGlobalSetting::browser_sub_process_path.ToWString());
}

void QCefSetting::setResourceDirectoryPath(const QString& path) {
  QCefGlobalSetting::initializeInstance();
  QCefGlobalSetting::resource_directory_path.FromWString( QDir::toNativeSeparators(path).toStdWString());
}

const QString QCefSetting::resourceDirectoryPath() {
  QCefGlobalSetting::initializeInstance();
  return QString::fromStdWString( QCefGlobalSetting::resource_directory_path.ToWString());
}

void QCefSetting::setLocalesDirectoryPath(const QString& path) {
  QCefGlobalSetting::initializeInstance();
  QCefGlobalSetting::locales_directory_path.FromWString( QDir::toNativeSeparators(path).toStdWString());
}

const QString QCefSetting::localesDirectoryPath() {
  QCefGlobalSetting::initializeInstance();
  return QString::fromStdWString( QCefGlobalSetting::locales_directory_path.ToWString());
}

void QCefSetting::setUserAgent(const QString& agent) {
  QCefGlobalSetting::initializeInstance();
  QCefGlobalSetting::user_agent.FromWString(agent.toStdWString());
}

const QString QCefSetting::userAgent() {
  QCefGlobalSetting::initializeInstance();
  return QString::fromStdWString(QCefGlobalSetting::user_agent.ToWString());
}

void QCefSetting::setCachePath(const QString& path) {
  QCefGlobalSetting::initializeInstance();
  QCefGlobalSetting::cache_path.FromWString( QDir::toNativeSeparators(path).toStdWString());
}

const QString QCefSetting::cachePath() {
  QCefGlobalSetting::initializeInstance();
  return QString::fromStdWString(QCefGlobalSetting::cache_path.ToWString());
}

void QCefSetting::setUserDataPath(const QString& path) {
  QCefGlobalSetting::initializeInstance();
  QCefGlobalSetting::user_data_path.FromWString(path.toStdWString());
}

const QString QCefSetting::userDataPath() {
  QCefGlobalSetting::initializeInstance();
  return QString::fromStdWString(QCefGlobalSetting::user_data_path.ToWString());
}

void QCefSetting::setPersistSessionCookies(bool enabled) {
  QCefGlobalSetting::initializeInstance();
  QCefGlobalSetting::persist_session_cookies = enabled;
}

const bool QCefSetting::persistSessionCookies() {
  QCefGlobalSetting::initializeInstance();
  return QCefGlobalSetting::persist_session_cookies;
}

void QCefSetting::setPersistUserPreferences(bool enabled) {
  QCefGlobalSetting::initializeInstance();
  QCefGlobalSetting::persist_user_preferences = enabled;
}

const bool QCefSetting::persistUserPreferences() {
  QCefGlobalSetting::initializeInstance();
  return QCefGlobalSetting::persist_user_preferences;
}

void QCefSetting::setLocale(const QString& locale) {
  QCefGlobalSetting::initializeInstance();
  QCefGlobalSetting::locale.FromString(locale.toStdString());
}

const QString QCefSetting::locale() {
  QCefGlobalSetting::initializeInstance();
  return QString::fromStdString(QCefGlobalSetting::locale.ToString());
}

void QCefSetting::setRemoteDebuggingPort(int port) {
  QCefGlobalSetting::initializeInstance();
  QCefGlobalSetting::remote_debugging_port = port;
}

const int QCefSetting::remoteDebuggingPort() {
  QCefGlobalSetting::initializeInstance();
  return QCefGlobalSetting::remote_debugging_port;
}

void QCefSetting::setAcceptLanguageList(const QString& languages) {
  QCefGlobalSetting::initializeInstance();
  QCefGlobalSetting::accept_language_list.FromWString(languages.toStdWString());
}

const QString QCefSetting::acceptLanguageList() {
  QCefGlobalSetting::initializeInstance();
  return QString::fromStdWString(
      QCefGlobalSetting::accept_language_list.ToWString());
}

const void QCefSetting::setGpuEnabled(bool b) {
  QCefGlobalSetting::initializeInstance();
  QCefGlobalSetting::gpu_enabled = b;
}

const bool QCefSetting::gpuEnabled() {
  QCefGlobalSetting::initializeInstance();
  return QCefGlobalSetting::gpu_enabled;
}

void QCefSetting::setFlashPlugin(const QString& path, const QString& ver) {
  QCefGlobalSetting::initializeInstance();

  QCefGlobalSetting::flush_plugin_path.FromWString(path.toStdWString());
  QCefGlobalSetting::flush_plugin_ver.FromWString(ver.toStdWString());
}

const QString QCefSetting::flashPluginPath() {
  QCefGlobalSetting::initializeInstance();
  return QString::fromStdWString(
      QCefGlobalSetting::flush_plugin_path.ToWString());
}

const QString QCefSetting::flashPluginVersion() {
  QCefGlobalSetting::initializeInstance();
  return QString::fromStdWString(
      QCefGlobalSetting::flush_plugin_ver.ToWString());
}

const void QCefSetting::setOsrEnabled(bool b) {
  QCefGlobalSetting::initializeInstance();
  QCefGlobalSetting::osr_enabled = b;
}

const bool QCefSetting::osrEnabled() {
  QCefGlobalSetting::initializeInstance();
  return QCefGlobalSetting::osr_enabled;
}

void QCefSetting::setDebugLogPath(const QString& path) {
  QCefGlobalSetting::initializeInstance();
  QCefGlobalSetting::debug_log_path.FromWString( QDir::toNativeSeparators(path).toStdWString());
}

const QString QCefSetting::debugLogPath() {
  QCefGlobalSetting::initializeInstance();
  return QString::fromStdWString(QCefGlobalSetting::debug_log_path.ToWString());
}

void QCefSetting::setResourceMap(
    const QMap<QString, QPair<int, QString>>& resourceMap) {
  QCefGlobalSetting::initializeInstance();
  QCefGlobalSetting::resource_map = resourceMap;
}

const QMap<QString, QPair<int, QString>> QCefSetting::resourceMap() {
  QCefGlobalSetting::initializeInstance();
  return QCefGlobalSetting::resource_map;
}
