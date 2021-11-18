#ifndef QCEF_EXPORT_H_
#define QCEF_EXPORT_H_

#include <QtCore/qglobal.h>
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

#endif
