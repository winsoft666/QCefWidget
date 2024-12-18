if(ENABLE_CEF)
    add_subdirectory(single_cef)
    add_subdirectory(double_cef)
endif()

if(ENABLE_WEBVIEW2)
    add_subdirectory(single_webview2)
    add_subdirectory(double_webview2)
endif()

add_subdirectory(comprehensive)