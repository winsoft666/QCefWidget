# Copyright (c) 2014 The Chromium Embedded Framework Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.

#
# Source files.
#

# cefclient browser sources.
set(CEFCLIENT_BROWSER_BROWSER_SRCS
  ${CEF_SDK_DIR}/tests/cefclient/browser/binding_test.cc
  ${CEF_SDK_DIR}/tests/cefclient/browser/binding_test.h
  ${CEF_SDK_DIR}/tests/cefclient/browser/browser_window.cc
  ${CEF_SDK_DIR}/tests/cefclient/browser/browser_window.h
  ${CEF_SDK_DIR}/tests/cefclient/browser/bytes_write_handler.cc
  ${CEF_SDK_DIR}/tests/cefclient/browser/bytes_write_handler.h
  ${CEF_SDK_DIR}/tests/cefclient/browser/client_app_delegates_browser.cc
  ${CEF_SDK_DIR}/tests/cefclient/browser/client_browser.cc
  ${CEF_SDK_DIR}/tests/cefclient/browser/client_browser.h
  ${CEF_SDK_DIR}/tests/cefclient/browser/client_handler.cc
  ${CEF_SDK_DIR}/tests/cefclient/browser/client_handler.h
  ${CEF_SDK_DIR}/tests/cefclient/browser/client_handler_osr.cc
  ${CEF_SDK_DIR}/tests/cefclient/browser/client_handler_osr.h
  ${CEF_SDK_DIR}/tests/cefclient/browser/client_handler_std.cc
  ${CEF_SDK_DIR}/tests/cefclient/browser/client_handler_std.h
  ${CEF_SDK_DIR}/tests/cefclient/browser/client_types.h
  ${CEF_SDK_DIR}/tests/cefclient/browser/dialog_test.cc
  ${CEF_SDK_DIR}/tests/cefclient/browser/dialog_test.h
  ${CEF_SDK_DIR}/tests/cefclient/browser/drm_test.cc
  ${CEF_SDK_DIR}/tests/cefclient/browser/drm_test.h
  ${CEF_SDK_DIR}/tests/cefclient/browser/image_cache.cc
  ${CEF_SDK_DIR}/tests/cefclient/browser/image_cache.h
  ${CEF_SDK_DIR}/tests/cefclient/browser/main_context.cc
  ${CEF_SDK_DIR}/tests/cefclient/browser/main_context.h
  ${CEF_SDK_DIR}/tests/cefclient/browser/main_context_impl.cc
  ${CEF_SDK_DIR}/tests/cefclient/browser/main_context_impl.h
  ${CEF_SDK_DIR}/tests/cefclient/browser/osr_dragdrop_events.h
  ${CEF_SDK_DIR}/tests/cefclient/browser/osr_renderer.cc
  ${CEF_SDK_DIR}/tests/cefclient/browser/osr_renderer.h
  ${CEF_SDK_DIR}/tests/cefclient/browser/osr_renderer_settings.h
  ${CEF_SDK_DIR}/tests/cefclient/browser/preferences_test.cc
  ${CEF_SDK_DIR}/tests/cefclient/browser/preferences_test.h
  ${CEF_SDK_DIR}/tests/cefclient/browser/resource.h
  ${CEF_SDK_DIR}/tests/cefclient/browser/response_filter_test.cc
  ${CEF_SDK_DIR}/tests/cefclient/browser/response_filter_test.h
  ${CEF_SDK_DIR}/tests/cefclient/browser/root_window.cc
  ${CEF_SDK_DIR}/tests/cefclient/browser/root_window.h
  #${CEF_SDK_DIR}/tests/cefclient/browser/root_window_create.cc
  ${CEF_SDK_DIR}/tests/cefclient/browser/root_window_manager.cc
  ${CEF_SDK_DIR}/tests/cefclient/browser/root_window_manager.h
  ${CEF_SDK_DIR}/tests/cefclient/browser/scheme_test.cc
  ${CEF_SDK_DIR}/tests/cefclient/browser/scheme_test.h
  ${CEF_SDK_DIR}/tests/cefclient/browser/server_test.cc
  ${CEF_SDK_DIR}/tests/cefclient/browser/server_test.h
  ${CEF_SDK_DIR}/tests/cefclient/browser/temp_window.h
  ${CEF_SDK_DIR}/tests/cefclient/browser/test_runner.cc
  ${CEF_SDK_DIR}/tests/cefclient/browser/test_runner.h
  ${CEF_SDK_DIR}/tests/cefclient/browser/urlrequest_test.cc
  ${CEF_SDK_DIR}/tests/cefclient/browser/urlrequest_test.h
  ${CEF_SDK_DIR}/tests/cefclient/browser/window_test.cc
  ${CEF_SDK_DIR}/tests/cefclient/browser/window_test.h
  ${CEF_SDK_DIR}/tests/cefclient/browser/window_test_runner.cc
  ${CEF_SDK_DIR}/tests/cefclient/browser/window_test_runner.h
  )
source_group(cefclient\\\\browser FILES ${CEFCLIENT_BROWSER_BROWSER_SRCS})

set(CEFCLIENT_BROWSER_SHARED_BROWSER_SRCS
  ${CEF_SDK_DIR}/tests/shared/browser/client_app_browser.cc
  ${CEF_SDK_DIR}/tests/shared/browser/client_app_browser.h
  ${CEF_SDK_DIR}/tests/shared/browser/extension_util.cc
  ${CEF_SDK_DIR}/tests/shared/browser/extension_util.h
  ${CEF_SDK_DIR}/tests/shared/browser/file_util.cc
  ${CEF_SDK_DIR}/tests/shared/browser/file_util.h
  ${CEF_SDK_DIR}/tests/shared/browser/geometry_util.cc
  ${CEF_SDK_DIR}/tests/shared/browser/geometry_util.h
  ${CEF_SDK_DIR}/tests/shared/browser/main_message_loop.cc
  ${CEF_SDK_DIR}/tests/shared/browser/main_message_loop.h
  ${CEF_SDK_DIR}/tests/shared/browser/main_message_loop_external_pump.cc
  ${CEF_SDK_DIR}/tests/shared/browser/main_message_loop_external_pump.h
  ${CEF_SDK_DIR}/tests/shared/browser/main_message_loop_std.cc
  ${CEF_SDK_DIR}/tests/shared/browser/main_message_loop_std.h
  ${CEF_SDK_DIR}/tests/shared/browser/resource_util.h
  )
source_group(shared\\\\browser FILES ${CEFCLIENT_BROWSER_SHARED_BROWSER_SRCS})

set(CEFCLIENT_BROWSER_SRCS
  ${CEFCLIENT_BROWSER_BROWSER_SRCS}
  ${CEFCLIENT_BROWSER_SHARED_BROWSER_SRCS}
  )

# cefclient common sources.
set(CEFCLIENT_COMMON_COMMON_SRCS
  ${CEF_SDK_DIR}/tests/cefclient/common/client_app_delegates_common.cc
  ${CEF_SDK_DIR}/tests/cefclient/common/scheme_test_common.cc
  ${CEF_SDK_DIR}/tests/cefclient/common/scheme_test_common.h
  )
source_group(cefclient\\\\common FILES ${CEFCLIENT_COMMON_COMMON_SRCS})

set(CEFCLIENT_COMMON_SHARED_COMMON_SRCS
  ${CEF_SDK_DIR}/tests/shared/common/client_app.cc
  ${CEF_SDK_DIR}/tests/shared/common/client_app.h
  ${CEF_SDK_DIR}/tests/shared/common/client_app_other.cc
  ${CEF_SDK_DIR}/tests/shared/common/client_app_other.h
  ${CEF_SDK_DIR}/tests/shared/common/client_switches.cc
  ${CEF_SDK_DIR}/tests/shared/common/client_switches.h
  )
source_group(shared\\\\common FILES ${CEFCLIENT_COMMON_SHARED_COMMON_SRCS})

set(CEFCLIENT_COMMON_SRCS
  ${CEFCLIENT_COMMON_COMMON_SRCS}
  ${CEFCLIENT_COMMON_SHARED_COMMON_SRCS}
  )

# cefclient renderer sources.
set(CEFCLIENT_RENDERER_RENDERER_SRCS
   ${CEF_SDK_DIR}/tests/cefclient/renderer/client_app_delegates_renderer.cc
   ${CEF_SDK_DIR}/tests/cefclient/renderer/client_renderer.cc
   ${CEF_SDK_DIR}/tests/cefclient/renderer/client_renderer.h
   ${CEF_SDK_DIR}/tests/cefclient/renderer/performance_test.cc
   ${CEF_SDK_DIR}/tests/cefclient/renderer/performance_test.h
   ${CEF_SDK_DIR}/tests/cefclient/renderer/performance_test_setup.h
   ${CEF_SDK_DIR}/tests/cefclient/renderer/performance_test_tests.cc
  )
source_group(cefclient\\\\renderer FILES ${CEFCLIENT_RENDERER_RENDERER_SRCS})

set(CEFCLIENT_RENDERER_SHARED_RENDERER_SRCS
  ${CEF_SDK_DIR}/tests/shared/renderer/client_app_renderer.cc
  ${CEF_SDK_DIR}/tests/shared/renderer/client_app_renderer.h
  )
source_group(shared\\\\renderer FILES ${CEFCLIENT_RENDERER_SHARED_RENDERER_SRCS})

set(CEFCLIENT_RENDERER_SRCS
  ${CEFCLIENT_RENDERER_RENDERER_SRCS}
  ${CEFCLIENT_RENDERER_SHARED_RENDERER_SRCS}
  )

#cefclient Linux sources
# set(CEFCLIENT_LINUX_SRCS
#   cefclient_gtk.cc
#   )
# source_group(cefclient FILES ${CEFCLIENT_LINUX_SRCS})

set(CEFCLIENT_LINUX_BROWSER_SRCS
   ${CEF_SDK_DIR}/tests/cefclient/browser/browser_window_osr_gtk.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/browser_window_osr_gtk.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/browser_window_std_gtk.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/browser_window_std_gtk.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/dialog_handler_gtk.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/dialog_handler_gtk.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/main_context_impl_posix.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/main_message_loop_multithreaded_gtk.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/main_message_loop_multithreaded_gtk.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/print_handler_gtk.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/print_handler_gtk.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/resource_util_linux.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/root_window_gtk.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/root_window_gtk.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/root_window_views.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/root_window_views.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/temp_window_x11.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/temp_window_x11.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/util_gtk.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/util_gtk.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/views_menu_bar.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/views_menu_bar.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/views_style.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/views_style.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/views_window.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/views_window.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/window_test_runner_gtk.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/window_test_runner_gtk.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/window_test_runner_views.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/window_test_runner_views.h
  )
source_group(cefclient\\\\browser FILES ${CEFCLIENT_LINUX_BROWSER_SRCS})

set(CEFCLIENT_LINUX_SHARED_BROWSER_SRCS
  ${CEF_SDK_DIR}/tests/shared/browser/main_message_loop_external_pump_linux.cc
  ${CEF_SDK_DIR}/tests/shared/browser/resource_util_posix.cc
  )
source_group(shared\\\\browser FILES ${CEFCLIENT_LINUX_SHARED_BROWSER_SRCS})

set(CEFCLIENT_LINUX_SRCS
  ${CEFCLIENT_LINUX_SRCS}
  ${CEFCLIENT_LINUX_BROWSER_SRCS}
  ${CEFCLIENT_LINUX_SHARED_BROWSER_SRCS}
  )

#cefclient Mac OS X sources
# set(CEFCLIENT_MACOSX_SRCS
#   cefclient_mac.mm
#   )
# source_group(cefclient FILES ${CEFCLIENT_MACOSX_SRCS})

set(CEFCLIENT_MACOSX_BROWSER_SRCS
   ${CEF_SDK_DIR}/tests/cefclient/browser/browser_window_osr_mac.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/browser_window_osr_mac.mm
   ${CEF_SDK_DIR}/tests/cefclient/browser/browser_window_std_mac.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/browser_window_std_mac.mm
   ${CEF_SDK_DIR}/tests/cefclient/browser/main_context_impl_posix.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/osr_accessibility_helper.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/osr_accessibility_helper.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/osr_accessibility_node.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/osr_accessibility_node.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/osr_accessibility_node_mac.mm
   ${CEF_SDK_DIR}/tests/cefclient/browser/root_window_mac.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/root_window_mac.mm
   ${CEF_SDK_DIR}/tests/cefclient/browser/temp_window_mac.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/temp_window_mac.mm
   ${CEF_SDK_DIR}/tests/cefclient/browser/text_input_client_osr_mac.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/text_input_client_osr_mac.mm
   ${CEF_SDK_DIR}/tests/cefclient/browser/window_test_runner_mac.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/window_test_runner_mac.mm
  )
source_group(cefclient\\\\browser FILES ${CEFCLIENT_MACOSX_BROWSER_SRCS})

set(CEFCLIENT_MACOSX_SHARED_BROWSER_SRCS
  ${CEF_SDK_DIR}/tests/shared/browser/main_message_loop_external_pump_mac.mm
  ${CEF_SDK_DIR}/tests/shared/browser/resource_util_mac.mm
  ${CEF_SDK_DIR}/tests/shared/browser/resource_util_posix.cc
  )
source_group(shared\\\\browser FILES ${CEFCLIENT_MACOSX_SHARED_BROWSER_SRCS})

set(CEFCLIENT_MACOSX_SRCS
  ${CEFCLIENT_MACOSX_SRCS}
  ${CEFCLIENT_MACOSX_BROWSER_SRCS}
  ${CEFCLIENT_MACOSX_SHARED_BROWSER_SRCS}
  )

# cefclient Mac OS X helper sources.
set(CEFCLIENT_HELPER_SHARED_SRCS
  ${CEF_SDK_DIR}/tests/shared/process_helper_mac.cc
  )
source_group(shared FILES ${CEFCLIENT_HELPER_SHARED_SRCS})

set(CEFCLIENT_MACOSX_HELPER_SRCS
  ${CEFCLIENT_HELPER_SHARED_SRCS}
  )

#cefclient Windows sources
# set(CEFCLIENT_WINDOWS_SRCS
#   cefclient_win.cc
#   )
# source_group(cefclient FILES ${CEFCLIENT_WINDOWS_SRCS})

set(CEFCLIENT_WINDOWS_BROWSER_SRCS
   ${CEF_SDK_DIR}/tests/cefclient/browser/browser_window_osr_win.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/browser_window_osr_win.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/browser_window_std_win.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/browser_window_std_win.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/main_context_impl_win.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/main_message_loop_multithreaded_win.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/main_message_loop_multithreaded_win.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/osr_accessibility_helper.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/osr_accessibility_helper.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/osr_accessibility_node.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/osr_accessibility_node.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/osr_accessibility_node_win.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/osr_d3d11_win.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/osr_d3d11_win.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/osr_dragdrop_win.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/osr_dragdrop_win.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/osr_ime_handler_win.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/osr_ime_handler_win.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/osr_render_handler_win.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/osr_render_handler_win.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/osr_render_handler_win_d3d11.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/osr_render_handler_win_d3d11.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/osr_render_handler_win_gl.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/osr_render_handler_win_gl.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/osr_window_win.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/osr_window_win.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/resource_util_win_idmap.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/root_window_views.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/root_window_views.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/root_window_win.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/root_window_win.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/temp_window_win.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/temp_window_win.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/views_menu_bar.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/views_menu_bar.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/views_style.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/views_style.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/views_window.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/views_window.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/window_test_runner_views.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/window_test_runner_views.h
   ${CEF_SDK_DIR}/tests/cefclient/browser/window_test_runner_win.cc
   ${CEF_SDK_DIR}/tests/cefclient/browser/window_test_runner_win.h
  )
source_group(cefclient\\\\browser FILES ${CEFCLIENT_WINDOWS_BROWSER_SRCS})

# set(CEFCLIENT_WINDOWS_RESOURCES_WIN_SRCS
#   resources/win/cefclient.exe.manifest
#   resources/win/cefclient.ico
#   resources/win/cefclient.rc
#   resources/win/small.ico
#   )
# source_group(cefclient\\\\resources\\\\win FILES ${CEFCLIENT_WINDOWS_RESOURCES_WIN_SRCS})

set(CEFCLIENT_WINDOWS_SHARED_BROWSER_SRCS
  ${CEF_SDK_DIR}/tests/shared/browser/main_message_loop_external_pump_win.cc
  ${CEF_SDK_DIR}/tests/shared/browser/resource_util_win.cc
  ${CEF_SDK_DIR}/tests/shared/browser/util_win.cc
  ${CEF_SDK_DIR}/tests/shared/browser/util_win.h
  )
source_group(shared\\\\browser FILES ${CEFCLIENT_WINDOWS_SHARED_BROWSER_SRCS})

set(CEFCLIENT_WINDOWS_SRCS
  ${CEFCLIENT_WINDOWS_SRCS}
  ${CEFCLIENT_WINDOWS_BROWSER_SRCS}
  # ${CEFCLIENT_WINDOWS_RESOURCES_WIN_SRCS}
  ${CEFCLIENT_WINDOWS_SHARED_BROWSER_SRCS}
  )

# cefclient resources.
# set(CEFCLIENT_RESOURCES_RESOURCES_SRCS
#   resources/binding.html
#   resources/dialogs.html
#   resources/draggable.html
#   resources/drm.html
#   resources/localstorage.html
#   resources/logo.png
#   resources/menu_icon.1x.png
#   resources/menu_icon.2x.png
#   resources/other_tests.html
#   resources/performance.html
#   resources/performance2.html
#   resources/preferences.html
#   resources/response_filter.html
#   resources/server.html
#   resources/transparency.html
#   resources/urlrequest.html
#   resources/websocket.html
#   resources/window.html
#   resources/xmlhttprequest.html
#   )
# source_group(cefclient\\\\resources FILES ${CEFCLIENT_RESOURCES_RESOURCES_SRCS})

# set(CEFCLIENT_RESOURCES_RESOURCES_EXTENSIONS_SET_PAGE_COLOR_SRCS
#   resources/extensions/set_page_color/README.md
#   resources/extensions/set_page_color/icon.png
#   resources/extensions/set_page_color/manifest.json
#   resources/extensions/set_page_color/popup.html
#   resources/extensions/set_page_color/popup.js
#   )
# source_group(cefclient\\\\resources\\\\extensions\\\\set_page_color FILES ${CEFCLIENT_RESOURCES_RESOURCES_EXTENSIONS_SET_PAGE_COLOR_SRCS})

# set(CEFCLIENT_RESOURCES_RESOURCES_MAC_SRCS_MACOSX
#   resources/mac/Info.plist
#   resources/mac/cefclient.icns
#   )
# APPEND_PLATFORM_SOURCES(CEFCLIENT_RESOURCES_RESOURCES_MAC_SRCS)
# source_group(cefclient\\\\resources\\\\mac FILES ${CEFCLIENT_RESOURCES_RESOURCES_MAC_SRCS})

# set(CEFCLIENT_RESOURCES_RESOURCES_MAC_ENGLISH_LPROJ_SRCS_MACOSX
#   resources/mac/English.lproj/InfoPlist.strings
#   resources/mac/English.lproj/MainMenu.xib
#   )
# APPEND_PLATFORM_SOURCES(CEFCLIENT_RESOURCES_RESOURCES_MAC_ENGLISH_LPROJ_SRCS)
# source_group(cefclient\\\\resources\\\\mac\\\\English.lproj FILES ${CEFCLIENT_RESOURCES_RESOURCES_MAC_ENGLISH_LPROJ_SRCS})

# set(CEFCLIENT_RESOURCES_SHARED_RESOURCES_SRCS
#   ${CEF_SDK_DIR}/tests/shared/resources/osr_test.html
#   ${CEF_SDK_DIR}/tests/shared/resources/pdf.html
#   ${CEF_SDK_DIR}/tests/shared/resources/pdf.pdf
#   ${CEF_SDK_DIR}/tests/shared/resources/window_icon.1x.png
#   ${CEF_SDK_DIR}/tests/shared/resources/window_icon.2x.png
#   )
# source_group(shared\\\\resources FILES ${CEFCLIENT_RESOURCES_SHARED_RESOURCES_SRCS})

# set(CEFCLIENT_RESOURCES_SRCS
#   ${CEFCLIENT_RESOURCES_RESOURCES_SRCS}
#   ${CEFCLIENT_RESOURCES_RESOURCES_EXTENSIONS_SET_PAGE_COLOR_SRCS}
#   ${CEFCLIENT_RESOURCES_RESOURCES_MAC_SRCS}
#   ${CEFCLIENT_RESOURCES_RESOURCES_MAC_ENGLISH_LPROJ_SRCS}
#   ${CEFCLIENT_RESOURCES_SHARED_RESOURCES_SRCS}
#   )


#
# Shared configuration.
#

# Target executable names.
# set(CEF_TARGET "cefclient")
if(OS_MACOSX)
  set(CEF_HELPER_TARGET "cefclient_Helper")
  set(CEF_HELPER_OUTPUT_NAME "cefclient Helper")
else()
  # Logical target used to link the libcef library.
  ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")
endif()

# Set the target output directory to bin/$<CONFIGURATION> folder.
set(CEF_TARGET_OUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>")

#
# Linux configuration.
#

if(OS_LINUX)
  # All sources required by the "cefclient" target. Generates an executable that
  # is used for all processes.
  set(CEFCLIENT_SRCS
    ${CEFCLIENT_BROWSER_SRCS}
    ${CEFCLIENT_COMMON_SRCS}
    ${CEFCLIENT_RENDERER_SRCS}
    ${CEFCLIENT_RESOURCES_SRCS}
    ${CEFCLIENT_LINUX_SRCS}
    )

  # Find required libraries and update compiler/linker variables.
  FIND_LINUX_LIBRARIES("gmodule-2.0 gtk+-2.0 gthread-2.0 gtk+-unix-print-2.0 gtkglext-1.0 xi")

  # Executable target.
  add_executable(${CEF_TARGET} ${CEFCLIENT_SRCS})
  SET_EXECUTABLE_TARGET_PROPERTIES(${CEF_TARGET})
  add_dependencies(${CEF_TARGET} libcef_dll_wrapper)
  target_link_libraries(${CEF_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})

  # Set rpath so that libraries can be placed next to the executable.
  set_target_properties(${CEF_TARGET} PROPERTIES INSTALL_RPATH "$ORIGIN")
  set_target_properties(${CEF_TARGET} PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
  set_target_properties(${CEF_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CEF_TARGET_OUT_DIR})

  # Copy CEF binary and resource files to the target output directory.
  COPY_FILES("${CEF_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
  COPY_FILES("${CEF_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${CEF_TARGET_OUT_DIR}")

  # Copy cefclient resource files to the target output directory.
  COPY_FILES("${CEF_TARGET}" "${CEFCLIENT_RESOURCES_SRCS}" "${CMAKE_CURRENT_SOURCE_DIR}" "${CEF_TARGET_OUT_DIR}/cefclient_files")

  # Set SUID permissions on the chrome-sandbox target.
  SET_LINUX_SUID_PERMISSIONS("${CEF_TARGET}" "${CEF_TARGET_OUT_DIR}/chrome-sandbox")
endif()


#
# Mac OS X configuration.
#

if(OS_MACOSX)
  # All sources required by the "cefclient" target. Generates an app bundle that
  # is used only for the browser process.
  set(CEFCLIENT_SRCS
    ${CEFCLIENT_BROWSER_SRCS}
    ${CEFCLIENT_COMMON_SRCS}
    ${CEFCLIENT_RESOURCES_SRCS}
    ${CEFCLIENT_MACOSX_SRCS}
    )

  # All sources required by the "cefclient Helper" target. Generates an app
  # bundle that is used only for non-browser processes.
  set(CEFCLIENT_HELPER_SRCS
    ${CEFCLIENT_COMMON_SRCS}
    ${CEFCLIENT_RENDERER_SRCS}
    ${CEFCLIENT_MACOSX_HELPER_SRCS}
    )

  # Output paths for the app bundles.
  set(CEF_APP "${CEF_TARGET_OUT_DIR}/${CEF_TARGET}.app")
  set(CEF_HELPER_APP "${CEF_TARGET_OUT_DIR}/${CEF_HELPER_OUTPUT_NAME}.app")

  # Variable referenced from Info.plist files.
  set(PRODUCT_NAME "${CEF_TARGET}")

  # Helper executable target.
  add_executable(${CEF_HELPER_TARGET} MACOSX_BUNDLE ${CEFCLIENT_HELPER_SRCS})
  SET_EXECUTABLE_TARGET_PROPERTIES(${CEF_HELPER_TARGET})
  add_dependencies(${CEF_HELPER_TARGET} libcef_dll_wrapper)
  target_link_libraries(${CEF_HELPER_TARGET} libcef_dll_wrapper ${CEF_STANDARD_LIBS})
  set_target_properties(${CEF_HELPER_TARGET} PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/resources/mac/helper-Info.plist
    OUTPUT_NAME ${CEF_HELPER_OUTPUT_NAME}
    )

  if(USE_SANDBOX)
    # Logical target used to link the cef_sandbox library.
    ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
    target_link_libraries(${CEF_HELPER_TARGET} cef_sandbox_lib)
  endif()

  # Main executable target.
  add_executable(${CEF_TARGET} MACOSX_BUNDLE ${CEFCLIENT_RESOURCES_SRCS} ${CEFCLIENT_SRCS})
  SET_EXECUTABLE_TARGET_PROPERTIES(${CEF_TARGET})
  add_dependencies(${CEF_TARGET} libcef_dll_wrapper "${CEF_HELPER_TARGET}")
  target_link_libraries(${CEF_TARGET} libcef_dll_wrapper ${CEF_STANDARD_LIBS} "-framework OpenGL")
  set_target_properties(${CEF_TARGET} PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/resources/mac/Info.plist
    )

  # Copy files into the main app bundle.
  add_custom_command(
    TARGET ${CEF_TARGET}
    POST_BUILD
    # Copy the helper app bundle into the Frameworks directory.
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CEF_HELPER_APP}"
            "${CEF_APP}/Contents/Frameworks/${CEF_HELPER_OUTPUT_NAME}.app"
    # Copy the CEF framework into the Frameworks directory.
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CEF_BINARY_DIR}/Chromium Embedded Framework.framework"
            "${CEF_APP}/Contents/Frameworks/Chromium Embedded Framework.framework"
    VERBATIM
    )

  # Manually process and copy over resource files.
  # The Xcode generator can support this via the set_target_properties RESOURCE
  # directive but that doesn't properly handle nested resource directories.
  # Remove these prefixes from input file paths.
  set(PREFIXES
    "resources/mac/"
    "resources/"
    "${CEF_SDK_DIR}/tests/shared/resources/"
    )
  COPY_MACOSX_RESOURCES("${CEFCLIENT_RESOURCES_SRCS}" "${PREFIXES}" "${CEF_TARGET}" "${CMAKE_CURRENT_SOURCE_DIR}" "${CEF_APP}")
endif()


#
# Windows configuration.
#

if(OS_WINDOWS)
  # All sources required by the "cefclient" target. Generates an executable that
  # is used for all processes.
  set(CEFCLIENT_SRCS
    ${CEFCLIENT_BROWSER_SRCS}
    ${CEFCLIENT_COMMON_SRCS}
    ${CEFCLIENT_RENDERER_SRCS}
    # ${CEFCLIENT_RESOURCES_SRCS}
    ${CEFCLIENT_WINDOWS_SRCS}
    )
endif()


macro(CONFIG_QWEBVIEW_CEF_TARGET_PROP TARGET_NAME)
  if(OS_WINDOWS)
    # Executable target.
    #add_executable(${CEF_TARGET} WIN32 ${CEFCLIENT_SRCS})
    SET_EXECUTABLE_TARGET_PROPERTIES(${TARGET_NAME})
    add_dependencies(${TARGET_NAME} libcef_dll_wrapper)
    target_link_libraries(${TARGET_NAME} PRIVATE libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS} d3d11.lib glu32.lib imm32.lib opengl32.lib)
  
    if(USE_ATL)
      # Required by VS2013 to link accessibility API functions.
      target_link_libraries(${TARGET_NAME} PRIVATE oleacc.lib)
    endif()
  
    if(USE_SANDBOX)
      # Logical target used to link the cef_sandbox library.
      ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
      target_link_libraries(${TARGET_NAME} PRIVATE cef_sandbox_lib ${CEF_SANDBOX_STANDARD_LIBS})
    endif()
  
    # Add the custom manifest files to the executable.
    #ADD_WINDOWS_MANIFEST("${CMAKE_CURRENT_SOURCE_DIR}/resources/win" "${CEF_TARGET}" "exe")
  
    # Copy CEF binary and resource files to the target output directory.
    COPY_FILES("${TARGET_NAME}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
    COPY_FILES("${TARGET_NAME}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${CEF_TARGET_OUT_DIR}")
  endif()
endmacro()
