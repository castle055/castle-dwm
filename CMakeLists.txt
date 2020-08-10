cmake_minimum_required(VERSION 3.10)

project(castle-dwm)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED True)


add_library(LibsModule
	castle-dwm.h
	drw.h
	drw.cpp
	config.h
	transient.cpp
	util.cpp
	util.h
	client.cpp
	monitor.cpp
)
include_directories (/usr/include/freetype2)

add_executable(castle-dwm dwm.cpp)
target_link_libraries(castle-dwm LibsModule)
target_link_libraries(castle-dwm X11 fontconfig Xft Xinerama)

add_definitions(-DXINERAMA)
