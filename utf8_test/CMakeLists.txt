cmake_minimum_required(VERSION 3.14)

project(utf8_test LANGUAGES CXX)

include_directories(./include)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



add_compile_options(/utf-8)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_executable(utf8_test
  main.cpp
)
target_link_libraries(utf8_test Qt${QT_VERSION_MAJOR}::Core)

include(GNUInstallDirs)
install(TARGETS utf8_test
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
