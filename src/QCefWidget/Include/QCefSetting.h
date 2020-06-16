#ifndef QCEFSETTINGS_H
#define QCEFSETTINGS_H
#pragma once

#include <QtCore/qglobal.h>
#include <QString>
#include <QMetaType>

#ifndef QCEFWIDGET_EXPORT
#ifdef QCEFWIDGET_LIB
#define QCEFWIDGET_EXPORT Q_DECL_EXPORT
#else
#define QCEFWIDGET_EXPORT Q_DECL_IMPORT
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#pragma comment(lib, "QCefWidget.lib")
#endif
#endif
#endif

namespace QCefSetting {
/// <summary>
///
/// </summary>
void QCEFWIDGET_EXPORT setBrowserSubProcessPath(const QString &path);

/// <summary>
///
/// </summary>
const QCEFWIDGET_EXPORT QString browserSubProcessPath();

/// <summary>
///
/// </summary>
void QCEFWIDGET_EXPORT setResourceDirectoryPath(const QString &path);

/// <summary>
///
/// </summary>
const QCEFWIDGET_EXPORT QString resourceDirectoryPath();

/// <summary>
///
/// </summary>
void QCEFWIDGET_EXPORT setLocalesDirectoryPath(const QString &path);

/// <summary>
///
/// </summary>
const QCEFWIDGET_EXPORT QString localesDirectoryPath();

/// <summary>
///
/// </summary>
void QCEFWIDGET_EXPORT setUserAgent(const QString &agent);

/// <summary>
///
/// </summary>
const QCEFWIDGET_EXPORT QString userAgent();

/// <summary>
///
/// </summary>
void QCEFWIDGET_EXPORT setCachePath(const QString &path);

/// <summary>
///
/// </summary>
const QCEFWIDGET_EXPORT QString cachePath();

/// <summary>
///
/// </summary>
void QCEFWIDGET_EXPORT setUserDataPath(const QString &path);

/// <summary>
///
/// </summary>
const QCEFWIDGET_EXPORT QString userDataPath();

/// <summary>
///
/// </summary>
void QCEFWIDGET_EXPORT setPersistSessionCookies(bool enabled);

/// <summary>
///
/// </summary>
const QCEFWIDGET_EXPORT bool persistSessionCookies();

/// <summary>
///
/// </summary>
void QCEFWIDGET_EXPORT setPersistUserPreferences(bool enabled);

/// <summary>
///
/// </summary>
const QCEFWIDGET_EXPORT bool persistUserPreferences();

/// <summary>
///
/// </summary>
void QCEFWIDGET_EXPORT setLocale(const QString &locale);

/// <summary>
///
/// </summary>
const QCEFWIDGET_EXPORT QString locale();

/// <summary>
///
/// </summary>
void QCEFWIDGET_EXPORT setRemoteDebuggingPort(int port);

/// <summary>
///
/// </summary>
const QCEFWIDGET_EXPORT int remoteDebuggingPort();

/// <summary>
///
/// </summary>
void QCEFWIDGET_EXPORT setAcceptLanguageList(const QString &languages);

/// <summary>
///
/// </summary>
const QCEFWIDGET_EXPORT QString acceptLanguageList();

/// <summary>
///
/// </summary>
const QCEFWIDGET_EXPORT void setGpuEnabled(bool b);

/// <summary>
///
/// </summary>
const QCEFWIDGET_EXPORT bool gpuEnabled();

/// <summary>
///
/// </summary>
QCEFWIDGET_EXPORT void setFlashPlugin(const QString& path, const QString &ver);

/// <summary>
///
/// </summary>
const QCEFWIDGET_EXPORT QString flashPluginPath();

const QCEFWIDGET_EXPORT QString flashPluginVersion();

QCEFWIDGET_EXPORT void setHookTopLevelWindowCloseMessage(bool hook);

const QCEFWIDGET_EXPORT bool isHookTopLevelWindowCloseMessage();


QCEFWIDGET_EXPORT void setIgnoreTopLevelCloseMessage(bool ignore);

const QCEFWIDGET_EXPORT bool isIgnoreTopLevelCloseMessage();

}; // namespace QCefSetting

#endif
