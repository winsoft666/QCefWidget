#
# Copyright (c) winsoft666.
# All rights reserved.
#
# This source code is licensed under the license found in the
# LICENSE file in the root directory of this source tree.
#
add_subdirectory(core)

if(ENABLE_CEF)
    add_subdirectory(cef)
endif()

if(ENABLE_WEBVIEW2)
    add_subdirectory(webview2)
endif()

add_subdirectory(manager)
