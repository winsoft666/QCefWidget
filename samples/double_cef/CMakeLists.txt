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

add_executable(DoubleCEF 
  ${HEADERS}
  ${SOURCES}
  ${RES}
)

set_target_properties(DoubleCEF PROPERTIES FOLDER "Samples")

if (OS_WINDOWS)
  target_compile_definitions(DoubleCEF PRIVATE
    UNICODE
    _UNICODE
  )
endif()

target_include_directories(DoubleCEF PRIVATE 
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(DoubleCEF PRIVATE
  Qt5::Core
  Qt5::Gui 
  Qt5::Widgets
  QWebView.CEF
)

if(OS_WINDOWS)
  set_target_properties(DoubleCEF PROPERTIES
    WIN32_EXECUTABLE TRUE
    VS_DEBUGGER_ENVIRONMENT "PATH=${Qt5_DIR}/../../../bin;%PATH%"
  )
endif()

# Embed the manifest file into the target
if (MSVC)
    add_custom_command(TARGET DoubleCEF POST_BUILD
			COMMAND mt.exe -manifest \"${CMAKE_CURRENT_SOURCE_DIR}\\compatibility.manifest\" -inputresource:\"$<TARGET_FILE:DoubleCEF>\" -outputresource:\"$<TARGET_FILE:DoubleCEF>\")
endif(MSVC)