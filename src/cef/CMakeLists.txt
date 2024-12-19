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

set(SRC_ALL_PLATFORM
  QWebView.CEF.h
  qwebview.cef.cpp
  main_message_loop_qt.h
  main_message_loop_qt.cpp
  root_window_qt.h
  root_window_qt.cpp
  root_window_create.cpp
  v8_handler_impl.h
  v8_handler_impl.cpp
)

set(SRC_WIN
  qwebview.cef.win.cpp
)

set(RES_WIN
)

if(OS_WINDOWS)
  set(QWEBVIEW_CEF_SRC
    ${SRC_ALL_PLATFORM}
    ${SRC_WIN}
  )
elseif(OS_MACOSX)
elseif(OS_LINUX)
endif()

include(cef_74.1.19)

if (OS_WINDOWS)
  set(WIN_RES "resource.rc")
endif()

add_library(QWebView.CEF SHARED 
  ${CMAKE_SOURCE_DIR}/include/QWebView/
  ${CEFCLIENT_SRCS}
  ${QWEBVIEW_CEF_SRC}
  ${WIN_RES}
)

target_compile_definitions(QWebView.CEF PRIVATE
  UNICODE
  _UNICODE
  QWEBVIEW_CEF_LIB
  QWEBVIEW_CREATOR_LIB
)

target_include_directories(QWebView.CEF PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CEF_ROOT}
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/src/qwebview
)

target_link_libraries(QWebView.CEF 
  PRIVATE
    ${CEF_DLL_WRAPPER}
    ${CEF_LIB_FILE}
  PUBLIC
    Qt5::Core
    Qt5::Gui 
    Qt5::Widgets
    QWebView.Core
    QWebView.Manager
)

CONFIG_QWEBVIEW_CEF_TARGET_PROP(QWebView.CEF)

# add_custom_command(TARGET QWebView.CEF POST_BUILD
#   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CEF_RESOURCE_DIR} $<TARGET_FILE_DIR:QWebView.CEF/resources
#   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CEF_BINARY_DIR} $<TARGET_FILE_DIR:QWebView.CEF>
# )
