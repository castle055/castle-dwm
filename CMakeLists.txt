cmake_minimum_required(VERSION 3.17)

project(castle-dwm)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
#set(CMAKE_CXX_STANDARD_REQUIRED True)

#set(CMAKE_C_FLAGS "${CMAKE_ENABLE_C99}")

include(build/CMakeLists.txt)

include_directories (/usr/include/freetype2)

#add_library(b_config config.h)
target_sources(castle-dwm PRIVATE config.h)
target_link_libraries(castle-dwm PUBLIC X11 fontconfig Xft Xinerama)
#target_link_libraries(castle-dwm PUBLIC program_shell)
target_link_libraries(castle-dwm PUBLIC yaml-cpp)

add_definitions(-DXINERAMA)
