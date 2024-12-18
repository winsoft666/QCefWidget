set_property(GLOBAL PROPERTY AUTOGEN_SOURCE_GROUP "Generated Files")
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core GUI Widgets REQUIRED)

file(GLOB HEADERS LIST_DIRECTORIES false *.h)
file(GLOB SOURCES LIST_DIRECTORIES false *.cpp)
file(GLOB RES "*.rc")

add_executable(DoubleWebView2 
  ${HEADERS}
  ${SOURCES}
  ${RES}
)

set_target_properties(DoubleWebView2 PROPERTIES FOLDER "Samples")

target_compile_definitions(DoubleWebView2 PRIVATE
  UNICODE
  _UNICODE
)

target_include_directories(DoubleWebView2 PRIVATE 
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(DoubleWebView2 PRIVATE
  Qt5::Core
  Qt5::Gui 
  Qt5::Widgets
  QWebView.WebView2
)

set_target_properties(DoubleWebView2 PROPERTIES
  WIN32_EXECUTABLE TRUE
  VS_DEBUGGER_ENVIRONMENT "PATH=${Qt5_DIR}/../../../bin;%PATH%"
)
