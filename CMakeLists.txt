#
# Copyright (c) winsoft666.
# All rights reserved.
#
# This source code is licensed under the license found in the
# LICENSE file in the root directory of this source tree.
#
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(QWebView
  VERSION 2.0.1
  LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

string(REPLACE "." ";" VERSION_LIST ${PROJECT_VERSION})
list(GET VERSION_LIST 0 QWEBVIEW_VERSION_MAJOR)
list(GET VERSION_LIST 1 QWEBVIEW_VERSION_MINOR)
list(GET VERSION_LIST 2 QWEBVIEW_VERSION_PATCH)

configure_file("${CMAKE_SOURCE_DIR}/src/version.h.in" "${CMAKE_SOURCE_DIR}/src/version.h")

if(MSVC)
  option(QWEBVIEW_STATIC_MSVC_CRT "Set to ON to build with static CRT on Windows (/MT)." OFF)
  option(QWEBVIEW_ENABLE_WEBVIEW2 "Set to ON to enable WebView2 engine on Windows." ON)
endif()

option(QWEBVIEW_BUILD_SAMPLES "Set to ON to build samples." ON)

set(CEF_SDK_DIR "${CMAKE_SOURCE_DIR}/dep/cef_binary_74.1.19+gb62bacf+chromium-74.0.3729.157_windows32" CACHE STRING "The root dir of the CEF SDK.")

# Determine the platform.
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  set(OS_MACOSX 1)
  set(OS_POSIX 1)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  set(OS_LINUX 1)
  set(OS_POSIX 1)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  set(OS_WINDOWS 1)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CXX_STANDARD_REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (MSVC AND QWEBVIEW_STATIC_MSVC_CRT)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

if(CEF_SDK_DIR)
  set(ENABLE_CEF ON)
endif()

if(QWEBVIEW_ENABLE_WEBVIEW2)
  set(ENABLE_WEBVIEW2 ON)
endif()

if(ENABLE_CEF)
  # configure_file (
  # "${CMAKE_SOURCE_DIR}/src/cef/widget/CefVersion.h.in"
  # "${CMAKE_SOURCE_DIR}/src/cef/widget/CefVersion.h"
  # )

  set(CEF_ROOT "${CEF_SDK_DIR}")
  list(APPEND CMAKE_MODULE_PATH "${CEF_SDK_DIR}/cmake")

  set(USE_SANDBOX OFF)

  if(QWEBVIEW_STATIC_MSVC_CRT)
    set(CEF_RUNTIME_LIBRARY_FLAG "/MT" CACHE STRING "")
  else()
    set(CEF_RUNTIME_LIBRARY_FLAG "/MD" CACHE STRING "")
  endif()

  # Find the CEF package
  find_package(CEF REQUIRED)

  # Store the libcef.lib path and cef dll wrapper target name
  if(OS_WINDOWS)
    set(CEF_LIB_FILE "${CEF_BINARY_DIR}/libcef.lib")
  else()
    set(CEF_LIB_FILE "${CEF_BINARY_DIR}/libcef.a")
  endif()

  set(CEF_DLL_WRAPPER "libcef_dll_wrapper")

  add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH})

  PRINT_CEF_CONFIG()
else()
  message(WARNING "Disable CEF engine.")
endif()

if(NOT ENABLE_WEBVIEW2)
  message(WARNING "Disable WebView2 engine.")
endif()

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(src)

if(QWEBVIEW_BUILD_SAMPLES)
  add_subdirectory(samples)
endif()