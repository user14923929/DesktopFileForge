cmake_minimum_required(VERSION 3.16)
project(DesktopFileForge VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(desktopforge
    src/main.cpp
    src/core/DesktopEntry.cpp
    src/core/DesktopEntryWriter.cpp
    src/core/DesktopEntryParser.cpp
    src/core/DesktopEntryValidator.cpp
    src/ui/MainWindow.cpp
)

target_include_directories(desktopforge PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(desktopforge PRIVATE Qt6::Widgets)

install(TARGETS desktopforge RUNTIME DESTINATION bin)
install(FILES resources/desktopforge.desktop DESTINATION share/applications)
install(FILES resources/desktopforge.svg DESTINATION share/icons/hicolor/scalable/apps)
