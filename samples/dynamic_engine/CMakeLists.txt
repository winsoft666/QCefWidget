cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

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

add_executable(DynamicEngine 
  ${HEADERS}
  ${SOURCES}
  ${RES}
)

set_target_properties(DynamicEngine PROPERTIES FOLDER "Samples")

target_compile_definitions(DynamicEngine PRIVATE
  UNICODE
  _UNICODE
)

target_include_directories(DynamicEngine PRIVATE 
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(DynamicEngine PRIVATE
  Qt5::Core
  Qt5::Gui 
  Qt5::Widgets
  QWebView.WebView2
  QWebView.CEF
)

if(MSVC)
  target_link_options(DynamicEngine PRIVATE "/DELAYLOAD:$<TARGET_FILE_BASE_NAME:QWebView.CEF>.dll")
endif()

if(OS_WINDOWS)
  set_target_properties(DynamicEngine PROPERTIES
    WIN32_EXECUTABLE TRUE
    VS_DEBUGGER_ENVIRONMENT "PATH=${Qt5_DIR}/../../../bin;%PATH%"
  )
endif()

# Embed the manifest file into the target
if (MSVC)
    add_custom_command(TARGET DynamicEngine POST_BUILD
			COMMAND mt.exe -manifest \"${CMAKE_CURRENT_SOURCE_DIR}\\compatibility.manifest\" -inputresource:\"$<TARGET_FILE:DynamicEngine>\" -outputresource:\"$<TARGET_FILE:DynamicEngine>\")
endif(MSVC)

add_custom_command(TARGET DynamicEngine POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/../asserts" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIGURATION>/asserts"
)