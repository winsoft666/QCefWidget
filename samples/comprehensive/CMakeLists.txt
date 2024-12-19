set_property(GLOBAL PROPERTY AUTOGEN_SOURCE_GROUP "Generated Files")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core GUI Widgets REQUIRED)

file(GLOB HEADERS LIST_DIRECTORIES false *.h *.hpp)
file(GLOB SOURCES LIST_DIRECTORIES false *.cpp)

if (OS_WINDOWS)
  file(GLOB RES "*.rc")
endif()

add_executable(Comprehensive
  ${HEADERS}
  ${SOURCES}
  ${RES}
  Sample.qrc
)

set_target_properties(Comprehensive PROPERTIES FOLDER "Samples")

if (OS_WINDOWS)
  target_compile_definitions(Comprehensive PRIVATE
    UNICODE
    _UNICODE
  )
endif()

target_include_directories(Comprehensive PRIVATE 
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(Comprehensive PRIVATE
  Qt5::Core
  Qt5::Gui 
  Qt5::Widgets
  QWebView.CEF
  QWebView.WebView2
)

if(ENABLE_CEF)
  target_link_libraries(Comprehensive PRIVATE QWebView.CEF)
endif()

if(ENABLE_WEBVIEW2)
  target_link_libraries(Comprehensive PRIVATE QWebView.WebView2)
endif()

if(OS_WINDOWS)
  set_target_properties(Comprehensive PROPERTIES
    WIN32_EXECUTABLE TRUE
    VS_DEBUGGER_ENVIRONMENT "PATH=${Qt5_DIR}/../../../bin;%PATH%"
  )
endif()

# Embed the manifest file into the target
if (MSVC)
    add_custom_command(TARGET Comprehensive POST_BUILD
			COMMAND mt.exe -manifest \"${CMAKE_CURRENT_SOURCE_DIR}\\compatibility.manifest\" -inputresource:\"$<TARGET_FILE:Comprehensive>\" -outputresource:\"$<TARGET_FILE:Comprehensive>\")
endif(MSVC)

add_custom_command(TARGET Comprehensive POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/../asserts" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/asserts"
)