cmake_minimum_required(VERSION 3.26)

project(cyd-wm)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
#set(CMAKE_CXX_STANDARD_REQUIRED True)

#set(CMAKE_C_FLAGS "${CMAKE_ENABLE_C99}")

include(CheckCXXCompilerFlag)

function(enable_cxx_compiler_flag_if_supported flag)
    string(FIND "${CMAKE_CXX_FLAGS}" "${flag}" flag_already_set)
    if (flag_already_set EQUAL -1)
        check_cxx_compiler_flag("${flag}" flag_supported)
        if (flag_supported)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
        endif ()
        unset(flag_supported CACHE)
    endif ()
endfunction()

enable_cxx_compiler_flag_if_supported("-Wall")
enable_cxx_compiler_flag_if_supported("-Wextra")
enable_cxx_compiler_flag_if_supported("-pedantic")
enable_cxx_compiler_flag_if_supported("-Werror")

enable_cxx_compiler_flag_if_supported("-Wno-unused-function")
enable_cxx_compiler_flag_if_supported("-Wno-unused-parameter")
enable_cxx_compiler_flag_if_supported("-Wno-unused-variable")
enable_cxx_compiler_flag_if_supported("-Wno-unused-value")
enable_cxx_compiler_flag_if_supported("-Wno-unused-but-set-variable")
enable_cxx_compiler_flag_if_supported("-Wno-missing-field-initializers")

enable_cxx_compiler_flag_if_supported("-Wno-write-strings")
enable_cxx_compiler_flag_if_supported("-Wno-sign-compare")
enable_cxx_compiler_flag_if_supported("-Wno-deprecated-declarations")
enable_cxx_compiler_flag_if_supported("-Wno-cast-function-type")

enable_cxx_compiler_flag_if_supported("-Wmacro-redefine")

#enable_cxx_compiler_flag_if_supported("-Wno-error=unused")

enable_cxx_compiler_flag_if_supported("-Wno-unknown-pragmas")

# Hmm...
enable_cxx_compiler_flag_if_supported("-Wno-error=format=")

include_directories (/usr/include/freetype2)

FILE(GLOB_RECURSE SRC_LIST
        ${CMAKE_CURRENT_LIST_DIR}/include/*.c
        ${CMAKE_CURRENT_LIST_DIR}/include/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/include/*.h
        ${CMAKE_CURRENT_LIST_DIR}/include/*.hh
        ${CMAKE_CURRENT_LIST_DIR}/include/*.hpp
        ${CMAKE_CURRENT_LIST_DIR}/src/*.c
        ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/*.h
        ${CMAKE_CURRENT_LIST_DIR}/src/*.hpp
        )

find_package(PkgConfig REQUIRED)
pkg_check_modules(CAIROMM-1.16 REQUIRED cairomm-1.16)
include_directories(${CAIROMM-1.16_INCLUDE_DIRS})

LIST(APPEND EXT_LIBS
        cydstd cyd_ui
        X11 Xft Xrender Xinerama xcb
        yaml-cpp fontconfig jpeg ${CAIROMM-1.16_LIBRARIES} sensors)

add_executable(cyd-wm ${SRC_LIST})
target_link_libraries(cyd-wm PUBLIC ${EXT_LIBS})

FILE(GLOB_RECURSE TEST_LIST
        ${CMAKE_CURRENT_LIST_DIR}/tests/*.test.cpp)
FOREACH(ENTRYPOINT IN LISTS TEST_LIST)
    get_filename_component(ENTRYPOINT_NAME ${ENTRYPOINT} NAME_WE)
    set(ENTRYPOINT_NAME TEST--${ENTRYPOINT_NAME})
    add_executable(${ENTRYPOINT_NAME} ${SRC_LIST} ${ENTRYPOINT} )
    target_link_libraries(${ENTRYPOINT_NAME} PUBLIC ${EXT_LIBS})
ENDFOREACH()
add_definitions(-DXINERAMA)
