cmake_minimum_required(VERSION 3.20)
project(velvet-installer)

find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-4.0)

link_directories(
    ${GTKMM_LIBRARY_DIRS}  )

include_directories(
    ${GTKMM_INCLUDE_DIRS}  )

add_executable(velvet-installer
    src/main.cpp
)

include_directories(include/)

target_link_libraries(velvet-installer
    ${GTKMM_LIBRARIES}  )

add_custom_command(TARGET ${PROJECT_NAME}
POST_BUILD
COMMAND cp -r ${PROJECT_SOURCE_DIR}/assets ${PROJECT_SOURCE_DIR}/main.ui ${CMAKE_BINARY_DIR}
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)