cmake_minimum_required(VERSION 2.8)

project(sdl2-tutorial)

if(NOT CMAKE_CXX_COMPILER)
  set(CMAKE_CXX_COMPILER "clang++")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.7")
    message(FATAL_ERROR "Insufficient g++ version; require g++-4.7")
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "3.2")
    message(FATAL_ERROR "Insufficient clang++ version; require clang++-3.2")
  endif()
else()
  message(WARNING "Unexpected compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

add_definitions(-std=c++11)

set(CMAKE_CXX_FLAGS "-Wall -pedantic-errors ${CMAKE_CXX_FLAGS}")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")
set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g")

include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
if(SDL2_VERSION VERSION_LESS "2.0")
  message(FATAL_ERROR "Insufficient SDL2 version; require SDL-2.0")
else()
  message(STATUS "SDL2: ${SDL2_VERSION} (+libs: ${SDL2_LIBRARIES})")
endif()
pkg_search_module(SDL2_IMAGE REQUIRED SDL2_image)

add_executable(tutorial.01-hello_world tutorial.01-hello_world.cxx)
target_link_libraries(
  tutorial.01-hello_world
  ${SDL2_LIBRARIES}
  ${SDL2_IMAGE_LIBRARIES}
)
add_custom_command(TARGET tutorial.01-hello_world POST_BUILD
  COMMAND cp -uv ${PROJECT_SOURCE_DIR}/sample.png .
)
