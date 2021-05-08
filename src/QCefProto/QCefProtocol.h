#ifndef QCEFPROTOCOL_H_
#define QCEFPROTOCOL_H_
#if defined(_MSVC_) && _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\QCefWidget\Include\QCefUserDefined.h"

#define QCEF_SCHEMA "qcefwidget"

#define QCEF_QUERY_NAME "QCefWidgetQuery"

#define QCEF_QUERY_CANCEL_NAME "QCefWidgetQueryCancel"

#define QCEF_OBJECT_NAME "QCefWidget"

// QCefWidget.invoke("method_name", ...)
#define QCEF_INVOKEMETHOD "invoke"

// QCefWidget.addEventListener(type, listener)
#define QCEF_ADDEVENTLISTENER "addEventListener"

// QCefWidget.removeEventListener(type, listener)
#define QCEF_REMOVEEVENTLISTENER "removeEventListener"


// this message is sent from render process to browser process
// and is processed in the Qt UI thread
//
// format
// msg.name
//   msg.arg[0]: frame id
//   msg.arg[1]: function name
//   msg.arg[2~...]: function parameters
#define INVOKEMETHOD_NOTIFY_MESSAGE "QCefWidget891201ZJ0228#InvokeMethodNotify"


// this message is sent from browser process to render process
// and is processed in the CefRenderer_Main thread
//
// format:
//   msg.name:
//	  msg.arg[0]: frame id
//	  msg.arg[1]: function name
//	  msg.arg[2~...]: function parameters
#define TRIGGEREVENT_NOTIFY_MESSAGE "QCefWidget891201ZJ0228#TriggerEventNotify"


#define RENDER_PROCESS_NAME QCEF_WING_EXE_NAME

#define RESOURCE_DIRECTORY_NAME "resources"

#define LOCALES_DIRECTORY_NAME "locales"

#define WEB_CACHE_DIRECTORY_NAME "WebCache"

#define DEBUG_LOG_NAME "QCefWidget.log"

#define QCEF_USER_AGENT "QCefWidget/1.1 (Windows; en-us)"

#endif // !QCEFPROTOCOL_H_
