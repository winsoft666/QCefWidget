# 
# The main config file for QCefWidget
# NOTE:
#   Usually there is no need to change this file.
#   Change the config.cmake instead.
#
cmake_minimum_required(VERSION 3.4.1)
project(QCefWidget)

# Include the local config files
###############################################################
include(config.cmake)
###############################################################

configure_file (
  "${PROJECT_SOURCE_DIR}/src/QCefWidget/Include/QCefVersion.h.in"
  "${PROJECT_SOURCE_DIR}/src/QCefWidget/Include/QCefVersion.h"
  )
configure_file (
  "${PROJECT_SOURCE_DIR}/src/QCefWidget/Include/QCefUserDefined.h.in"
  "${PROJECT_SOURCE_DIR}/src/QCefWidget/Include/QCefUserDefined.h"
  )

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

# Determine the project architecture.
if(NOT DEFINED PROJECT_ARCH)
  if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(PROJECT_ARCH "x86_64")
  else()
    set(PROJECT_ARCH "x86")
  endif()

  if(OS_MACOSX)
    # PROJECT_ARCH should be specified on Mac OS X.
    message(WARNING "No PROJECT_ARCH value specified, using ${PROJECT_ARCH}")
  endif()
endif()

# Set common configurations
###############################################################
# Use solution folder
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 11)
set(CXX_STANDARD_REQUIRED)
set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(QCEF_WIDGET_SDK_OUT ${QCEF_WIDGET_SDK_OUT_DIR}/${QCEF_WIDGET_DLL})
set(QCEF_WIDGET_SDK_BIN_OUT ${QCEF_WIDGET_SDK_OUT_DIR}/${QCEF_WIDGET_DLL}/bin)
set(QCEF_WIDGET_SDK_LIB_OUT ${QCEF_WIDGET_SDK_OUT_DIR}/${QCEF_WIDGET_DLL}/lib)
set(QCEF_WIDGET_SDK_INC_OUT ${QCEF_WIDGET_SDK_OUT_DIR}/${QCEF_WIDGET_DLL}/include)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${QCEF_WIDGET_SDK_BIN_OUT})
###############################################################

# Append the QT dir to CMAKE_PREFIX_PATH
###############################################################
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${QT_SDK_DIR})
###############################################################


# Set CEF root dir and append it to CMAKE_MODULE_PATH
###############################################################
set(CEF_ROOT "${CEF_SDK_DIR}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")
###############################################################


# Config the CEF
###############################################################
cmake_policy(SET CMP0074 NEW)
cmake_policy(SET CMP0077 NEW)

# Set the runtime library type
set(CEF_RUNTIME_LIBRARY_FLAG "/MD" CACHE STRING "Use dynamic runtime")

# Disable the SANDBOX
set(USE_SANDBOX OFF CACHE BOOL "Disable the Sandbox")

# Disable the ATL
set(USE_ATL OFF CACHE STRING "Disable the ATL")

# Disable message hook
# TODO
set(DISABLE_HOOK OFF CACHE BOOL "Disable hook")

# Find the CEF package
find_package(CEF REQUIRED)

# Store the libcef.lib path and cef dll wrapper target name
set(CEF_LIB_FILE "${CEF_BINARY_DIR}/libcef.lib")
set(CEF_DLL_WRAPPER "libcef_dll_wrapper")

# Add build target 
add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH})
PRINT_CEF_CONFIG()
###############################################################


# Add all targets
###############################################################

include_directories(
  src/QCefProto
  ${CEF_ROOT}
)

add_subdirectory(src/QCefWing)

add_subdirectory(src/QCefWidget)
###############################################################


# Add demo
###############################################################
add_subdirectory(test/QCefWidgetTest)
###############################################################