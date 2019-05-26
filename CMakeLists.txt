cmake_minimum_required(VERSION 2.8)
project(ffDoom)

set(FFDOOMCLONE_VERSION_MAJOR 0)
set(FFDOOMCLONE_VERSION_MINOR 0)
set(FFDOOMCLONE_VERSION_PATCH 6)

find_package(SDL2 REQUIRED)

set(TEMPLATE_PATH "${CMAKE_SOURCE_DIR}/TEMPLATES")

configure_file("${TEMPLATE_PATH}/version.hxx.in" "${CMAKE_SOURCE_DIR}/version.hxx")

include_directories("${CMAKE_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR}/include ${SDL2_INCLUDE_DIRS}")

set(CMAKE_CXX_FLAGS "-Wall")

file(GLOB SOURCE_FILES *.cpp)

add_executable(ffDoom ${SOURCE_FILES})

target_link_libraries(ffDoom ${SDL2_LIBRARIES})