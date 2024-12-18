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

set(HEADERS
  qwebview.webview2.h
  AudioComponent.h
  CheckFailure.h
  ComponentBase.h
  ControlComponent.h
  CookieManagementComponent.h
  DCompTargetImpl.h
  DpiUtil.h
  DropTarget.h
  HostObjectComponent.h
  HostObjectSimpleImpl.h
  NonClientRegionSupportComponent.h
  ProcessComponent.h
  QWebViewWebView2Impl.h
  QWebView2Options.h
  ScriptComponent.h
  SettingsComponent.h
  stdafx.h
  ViewComponent.h
)

set(SOURCES
  qwebview.webview2.cpp
  AudioComponent.cpp
  CheckFailure.cpp
  ControlComponent.cpp
  CookieManagementComponent.cpp
  DCompTargetImpl.cpp
  DpiUtil.cpp
  DropTarget.cpp
  HostObjectComponent.cpp
  HostObjectSimpleImpl.cpp
  NonClientRegionSupportComponent.cpp
  ProcessComponent.cpp
  QWebViewWebView2Impl.cpp
  ScriptComponent.cpp
  SettingsComponent.cpp
  stdafx.cpp
  ViewComponent.cpp
)

add_library(QWebView.WebView2 SHARED 
  ${HEADERS}
  ${SOURCES}
  resource.rc
)

target_precompile_headers(QWebView.WebView2 PRIVATE stdafx.h)

target_compile_features(QWebView.WebView2 PRIVATE cxx_std_17)

target_compile_definitions(QWebView.WebView2 PRIVATE
  UNICODE
  _UNICODE
  QWEBVIEW_WEBVIEW2_LIB
  QWEBVIEW_CREATOR_LIB
)

target_include_directories(QWebView.WebView2 PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/src/qwebview
  ../3rd/Microsoft.Web.WebView2.1.0.2651.64/build/native/include
  ../3rd/Microsoft.Windows.ImplementationLibrary.1.0.240803.1/include/
)

target_link_libraries(QWebView.WebView2 
    PRIVATE
      Shlwapi.lib
      D2d1.lib
      Urlmon.lib
      ${CMAKE_SOURCE_DIR}/src/3rd/Microsoft.Web.WebView2.1.0.2651.64/build/native/x86/WebView2LoaderStatic.lib
    PUBLIC
      Qt5::Core
      Qt5::Gui 
      Qt5::Widgets
      QWebView.Core
      QWebView.Manager
)

set(MIDL_OUTPUT
    ${CMAKE_CURRENT_SOURCE_DIR}/HostObjectSimple_h.h
    ${CMAKE_CURRENT_SOURCE_DIR}/HostObjectSimple_i.c
)

set(MIDL_FILE
    ${CMAKE_CURRENT_SOURCE_DIR}/HostObjectSimple.idl
)

add_custom_command(
    OUTPUT ${MIDL_OUTPUT}
    COMMAND midl /h HostObjectSimple_h.h /tlb ${CMAKE_CURRENT_BINARY_DIR}/QWebView.WebView2.tlb ${MIDL_FILE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
)

add_custom_target(midl-cmplr DEPENDS ${MIDL_OUTPUT})

add_dependencies(QWebView.WebView2 midl-cmplr)