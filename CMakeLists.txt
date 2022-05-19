cmake_minimum_required(VERSION 3.17)

project(castle-dwm)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED True)

#set(CMAKE_C_FLAGS "${CMAKE_ENABLE_C99}")

add_library(LibsModule
	castle-dwm.h
	drw.h
	drw.cpp
	config.h
	transient.cpp
	util.cpp
	util.h
)
include_directories (/usr/include/freetype2)

add_executable(castle-dwm dwm.cpp ops/client_ops.cpp ops/client_ops.h state/dwm_types.h ops/monitor_ops.cpp ops/monitor_ops.h state/state.cpp state/state.h layouts/monocle.h layouts/tile.h layouts/centeredmaster.h ops/control_ops.cpp ops/control_ops.h ops/event_ops.cpp ops/event_ops.h ops/x11_ops.cpp ops/x11_ops.h ops/log_ops.cpp ops/log_ops.h ops/file_ops.cpp ops/file_ops.h ops/bar_ops.cpp ops/bar_ops.h ops/keynav_ops.cpp ops/keynav_ops.h)
target_link_libraries(castle-dwm PRIVATE LibsModule)
target_link_libraries(castle-dwm PUBLIC X11 fontconfig Xft Xinerama)
target_link_libraries(castle-dwm PUBLIC program_shell)
target_link_libraries(castle-dwm PUBLIC yaml-cpp)

add_definitions(-DXINERAMA)
