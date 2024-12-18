#ifndef QWEBVIEW2_OPTIONS_H_
#define QWEBVIEW2_OPTIONS_H_
#if defined(_MSVC_) && _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers.
#include <windows.h>
#include <string>
#include <functional>
#include <QColor>
#include <QVariant>

enum class CreationMode {
    Windowed = 0,
    VisualDComp = 1,
    TargetDComp = 2,
    VisualWinComp = 3
};

// Call_ParamsTypes_ReturnType
// V = Void
// S = String
// I = INT
// D = double
// C = Callback
//
enum class ScriptCallMode {
    Unknown = 0,
    Call_V_V,   // No parameter and no return value.
    Call_S_V,   // Parameter type is string and no return value.
    Call_S_D,   // Parameter type is string and return value type is double.
    Call_S_I,   // Parameter type is string and return value type is int.
    Call_S_S,   // Parameter type is string and return value type is also string.
    Call_I_V,   // Parameter type is int and no return value.
    Call_I_D,   // Parameter type is int and return value type is double.
    Call_I_I,   // Parameter type is int and return value type is also int.
    Call_I_S,   // Parameter type is int and return value type is string.
    Call_D_V,   // Parameter type is double and no return value.
    Call_D_D,   // Parameter type is double and return value type is also double.
    Call_D_I,   // Parameter type is double and return value type is int.
    Call_D_S,   // Parameter type is double and return value type is string.
    Call_IC_V,  // TODO
    Call_SC_V,  // TODO
    Call_DC_V,  // TODO
};

enum class CookieSameSiteKind {
    None = 0,
    Lax = 1,
    Strict = 2
};

struct WebView2Cookie {
    bool isHttpsOnly = false;
    bool isSecure = false;
    bool isSession = false;
    double expires = 0.0;
    QString name;
    QString value;
    QString domain;
    QString path;
    CookieSameSiteKind sameSiteKind = CookieSameSiteKind::None;
};

struct WebView2Options {
    bool mute = false;
    bool AADSSOEnabled = false;  // AAD single sign on
    bool useOSRegion = false;
    bool exclusiveUserDataFolderAccess = false;
    bool customCrashReportingEnabled = false;
    bool trackingPreventionEnabled = false;
    bool contextMenuEnabled = false;
    bool nonClientRegionSupportEnabled = true;
    bool cleanupUserDataFolderEnabled = false;  // TODO
    bool autoReinitializeOnProcessCrashed = true;
    bool autoReloadOnRendererCrashed = true;
    bool builtInErrorPageEnabled = true;

    CreationMode creationMode = CreationMode::Windowed;
    QString initialUrl;
    QString language;
    QString region;
    QString userDataFolder;  // Set it before QWebView2 instantiation.
    QString profile;
    QString downloadPath;
    QString scriptLocale;
    QColor backgroundColor;
};

typedef std::function<void(QVariant)> CallbackToScript;
typedef std::function<QVariant(ScriptCallMode,
                               int,                       // Command type
                               const QVariant& value,     // value
                               CallbackToScript cbScript  // Javascript callback function
                               )>
    ScriptCallCallback;

typedef std::function<void(HRESULT, QString result, QString exception)> ScriptResultCallback;

typedef std::function<void(HRESULT, const QVector<WebView2Cookie>&)> QueryCookieCallback;
#endif  // !QWEBVIEW2_OPTIONS_H_
