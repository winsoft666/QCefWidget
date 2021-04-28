#
# Build environment configuration file for QCefWidget
#

#
# QCefWidget Version
#
SET(QCEF_VERSION_MAJOR 1)
SET(QCEF_VERSION_MINOR 0)
SET(QCEF_VERSION_PATCH 3)

#
# The Qt SDK path
#
set(QT_SDK_DIR
  # Change this value to the Qt SDK path of your build environment
  "C:\\Qt\\Qt5.12.10\\5.12.10\\msvc2017\\"
  #"C:\\Qt\\Qt5.14.2\\5.14.2\\msvc2017\\"
)

#
# The root dir of the CEF SDK
#
set(CEF_SDK_DIR
  # Change this value to the CEF binary distribution path of your build environment
  "${CMAKE_CURRENT_SOURCE_DIR}/dep/cef_binary_89.0.18+gb36241d+chromium-89.0.4389.114_windows32"
)
SET(CEF_VERSION_MAJOR 89)
SET(CEF_VERSION_MINOR 0)
SET(CEF_VERSION_PATCH 18)