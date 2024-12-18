/*
 * Copyright (c) winsoft666.
 * All rights reserved.
 *
 * This source code is licensed under the license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef QWEBVIEW_TOP_LEVEL_WINDOW_UTIL_H_
#define QWEBVIEW_TOP_LEVEL_WINDOW_UTIL_H_
#pragma once

#include <QObject>
#include <QSet>
#include <QMap>
#include <QWidget>
#include <QAbstractNativeEventFilter>

QWidget* GetTopLevelWindow(QWidget* w);

#endif // !QWEBVIEW_TOP_LEVEL_WINDOW_UTIL_H_