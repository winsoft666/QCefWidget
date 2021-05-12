#ifndef QCEFSETTINGS_H
#define QCEFSETTINGS_H
#pragma once

#include <QtCore/qglobal.h>
#include <QString>
#include <QMap>
#include <QMetaType>
#include "QCefUserDefined.h"

#ifndef QCEFWIDGET_EXPORT
#ifdef QCEFWIDGET_LIB
#define QCEFWIDGET_EXPORT Q_DECL_EXPORT
#else
#define QCEFWIDGET_EXPORT Q_DECL_IMPORT
#if (defined Q_OS_WIN32 || defined Q_OS_WIN64)
#pragma comment(lib, QCEF_WIDGET_DLL_LIB_NAME)
#endif
#endif
#endif

namespace QCefSetting {
/// <summary>
///
/// </summary>
void QCEFWIDGET_EXPORT setBrowserSubProcessPath(const QString& path);

/// <summary>
///
/// </summary>
const QCEFWIDGET_EXPORT QString browserSubProcessPath();

/// <summary>
///
/// </summary>
void QCEFWIDGET_EXPORT setResourceDirectoryPath(const QString& path);

/// <summary>
///
/// </summary>
const QCEFWIDGET_EXPORT QString resourceDirectoryPath();

/// <summary>
///
/// </summary>
void QCEFWIDGET_EXPORT setLocalesDirectoryPath(const QString& path);

/// <summary>
///
/// </summary>
const QCEFWIDGET_EXPORT QString localesDirectoryPath();

/// <summary>
///
/// </summary>
void QCEFWIDGET_EXPORT setUserAgent(const QString& agent);

/// <summary>
///
/// </summary>
const QCEFWIDGET_EXPORT QString userAgent();

/// <summary>
///
/// </summary>
void QCEFWIDGET_EXPORT setCachePath(const QString& path);

/// <summary>
///
/// </summary>
const QCEFWIDGET_EXPORT QString cachePath();

/// <summary>
///
/// </summary>
void QCEFWIDGET_EXPORT setUserDataPath(const QString& path);

/// <summary>
///
/// </summary>
const QCEFWIDGET_EXPORT QString userDataPath();

/// <summary>
/// To persist session cookies (cookies without an expiry date or validity interval) by default when using the global cookie manager set this value to true.
/// Session cookies are generally intended to be transient and most Web browsers do not persist them.
/// A cachePath value must also be specified to enable this feature.
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
void QCEFWIDGET_EXPORT setLocale(const QString& locale);

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
void QCEFWIDGET_EXPORT setAcceptLanguageList(const QString& languages);

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
QCEFWIDGET_EXPORT void setFlashPlugin(const QString& path, const QString& ver);

const QCEFWIDGET_EXPORT QString flashPluginPath();

/// <summary>
///
/// </summary>
const QCEFWIDGET_EXPORT QString flashPluginVersion();

/// <summary>
///
/// </summary>
const QCEFWIDGET_EXPORT void setOsrEnabled(bool b);

/// <summary>
///
/// </summary>
const QCEFWIDGET_EXPORT bool osrEnabled();

/// <summary>
///
/// </summary>
void QCEFWIDGET_EXPORT setDebugLogPath(const QString& path);

/// <summary>
///
/// </summary>
const QCEFWIDGET_EXPORT QString debugLogPath();

/// <summary>
///
/// </summary>
void QCEFWIDGET_EXPORT setResourceMap(const QMap<QString, QPair<int, QString>>& resourceMap);

/// <summary>
///
/// </summary>
const QCEFWIDGET_EXPORT QMap<QString, QPair<int, QString>> resourceMap();

};  // namespace QCefSetting

#endif
