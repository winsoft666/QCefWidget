#ifndef QCEFCOMMON_H
#define QCEFCOMMON_H
#if defined(_MSVC_) && _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define QCEF_SCHEMA "qcef"

#define QCEF_QUERY_NAME "QCefQuery"

#define QCEF_QUERY_CANCEL_NAME "QCefQueryCancel"

#define QCEF_OBJECT_NAME "QCef"

// QCef.invoke("method_name", ...)
#define QCEF_INVOKEMETHOD "invoke"

// QCef.addEventListener(type, listener)
#define QCEF_ADDEVENTLISTENER "addEventListener"

// QCef.removeEventListener(type, listener)
#define QCEF_REMOVEEVENTLISTENER "removeEventListener"


// this message is sent from render process to browser process
// and is processed in the Qt UI thread
//
// format
// msg.name
//   msg.arg[0]: frame id
//   msg.arg[1]: function name
//   msg.arg[2~...]: function parameters
#define INVOKEMETHOD_NOTIFY_MESSAGE "QCef891201ZJ0228#InvokeMethodNotify"


// this message is sent from browser process to render process
// and is processed in the CefRenderer_Main thread
//
// format:
//   msg.name:
//	  msg.arg[0]: frame id
//	  msg.arg[1]: function name
//	  msg.arg[2~...]: function parameters
#define TRIGGEREVENT_NOTIFY_MESSAGE "QCef891201ZJ0228#TriggerEventNotify"


#define RENDER_PROCESS_NAME "QCefWing.exe"

#define RESOURCE_DIRECTORY_NAME "resources"

#define LOCALES_DIRECTORY_NAME "locales"

#define QCEF_USER_AGENT "QCef/1.1 (Windows; en-us)"

#endif
