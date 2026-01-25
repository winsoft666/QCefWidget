/*
 * Copyright (c) winsoft666.
 * All rights reserved.
 *
 * This source code is licensed under the license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef QWEBVIEW_CREATOR_H_
#define QWEBVIEW_CREATOR_H_
#pragma once

#include <QWidget>
#include <QString>
#include <QFlags>
#include <QtCore/qglobal.h>
#include "QWebView/Core.h"

#ifndef QWEBVIEW_CREATOR_EXPORT
#ifdef QWEBVIEW_CREATOR_LIB
#define QWEBVIEW_CREATOR_EXPORT Q_DECL_EXPORT
#else
#define QWEBVIEW_CREATOR_EXPORT Q_DECL_IMPORT
#endif
#endif

QWEBVIEW_CREATOR_EXPORT bool InitializeCEF(bool supportOSR);
QWEBVIEW_CREATOR_EXPORT void UnInitializeCEF();

QWEBVIEW_CREATOR_EXPORT QWebView* CreateCEF(QWidget* parent = Q_NULLPTR);

QWEBVIEW_CREATOR_EXPORT QWebView* CreateWebView2(QWidget* parent = Q_NULLPTR);

#endif  // !QWEBVIEW_CREATOR_H_