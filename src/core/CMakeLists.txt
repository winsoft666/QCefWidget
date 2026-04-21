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

add_library(QWebView.Core SHARED 
  ${CMAKE_SOURCE_DIR}/include/QWebView/Core.h
  core.cpp
  ${WIN_RES}
)

if (OS_WINDOWS)
  target_compile_definitions(QWebView.Core PRIVATE
    UNICODE
    _UNICODE
    QWEBVIEW_CORE_LIB
  )
endif()

target_include_directories(QWebView.Core PRIVATE
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(QWebView.Core 
  PUBLIC
    Qt5::Core
    Qt5::Gui 
    Qt5::Widgets
)
