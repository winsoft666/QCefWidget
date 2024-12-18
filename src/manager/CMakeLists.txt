#
# Copyright (c) winsoft666.
# All rights reserved.
#
# This source code is licensed under the license found in the
# LICENSE file in the root directory of this source tree.
#

set_property(GLOBAL PROPERTY AUTOGEN_SOURCE_GROUP "Generated Files")

set(CMAKE_AUTOMOC ON)

set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core GUI Widgets REQUIRED)

if (OS_WINDOWS)
  set(WIN_RES "resource.rc")
endif()

add_library(QWebView.Manager SHARED 
  ${CMAKE_SOURCE_DIR}/include/QWebView/Manager.h
  manager.cpp
  top_level_window_util.h
  top_level_window_util.cpp
  ${WIN_RES}
)

target_compile_definitions(QWebView.Manager PRIVATE
  UNICODE
  _UNICODE
  QWEBVIEW_MANAGER_LIB
)

target_include_directories(QWebView.Manager PRIVATE
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(QWebView.Manager 
  PUBLIC
    Qt5::Core
    Qt5::Gui 
    Qt5::Widgets
    QWebView.Core
)
