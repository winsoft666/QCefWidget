/*
 * Copyright (c) winsoft666.
 * All rights reserved.
 *
 * This source code is licensed under the license found in the
 * LICENSE file in the root directory of this source tree.
 */
 
#include "top_level_window_util.h"

QWidget* GetTopLevelWindow(QWidget* w) {
  if (!w)
    return nullptr;

  QWidget* result = w;
  do {
    QWidget* t = result->parentWidget();
    if (!t)
      break;
    result = t;
  } while (true);



  return result;
}
