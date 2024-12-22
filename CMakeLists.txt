# Copyright (c) 2024, Víctor Castillo Agüero.
# SPDX-License-Identifier: GPL-3.0-or-later


################################################################################
#[[                            PROJECT OVERVIEW                              ]]#
################################################################################
#                                                                    CYD-WM    #
#                                                                              #
#[[  AVAILABLE OPTIONS:                                                      ]]#
#      - CYD_UI_BUILD_DOCS .............................. DEV_MODE only, ON    #
#            whether or not the documentation should be built                  #
#                                                                              #
#[[  CMAKE STRUCTURE:                                                        ]]#
#      - Project setup                                                         #
#      - Configure dependencies:                                               #
#          - CYD Fabric (dev)                                                  #
#          - CYD UI (dev)                                                      #
#      - Configure CYD WM                                                      #
#      - Configure Doxygen documentation                                       #
#      - Configure install                                                     #
#                                                                              #
################################################################################


################################################################################
#[[                              PROJECT SETUP                               ]]#
################################################################################
#[[                          PREVENT IN-TREE BUILD                           ]]#
if (CMAKE_SOURCE_DIR STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree build. Create a `build/` directory and call `cmake ..` from there.")
endif ()

#[[                          CHECK DEVELOPER MODE                            ]]#
set(CYD_WM_DEV_MODE OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(CYD_WM_DEV_MODE ON)
endif ()

#[[                           CONFIGURE OPTIONS                              ]]#

#[[                         DEVELOPER MODE OPTIONS                           ]]#
if(CYD_WM_DEV_MODE)
    option(CYD_WM_BUILD_DOCS "whether or not the documentation should be built" ON)
endif ()

# Select 'Release' build type by default.
# Has to be done before the call to `project()`.
# Use `-DCMAKE_BUILD_TYPE=` to override this.
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "")

cmake_minimum_required(VERSION 3.30 FATAL_ERROR)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(cyd_wm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CYD_WM_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CYD_WM_INCLUDE_DIR ${CYD_WM_DIR}/include)
set(CYD_WM_SOURCE_DIR ${CYD_WM_DIR}/src)

include(cmake/compiler_flags.cmake)
compiler_flags(
        "-stdlib=libc++"
        "-Wall"
        "-Wextra"
        "-pedantic"
        "-Werror"

        "-Wliteral-suffix"

        "-Wno-dollar-in-identifier-extension"
        "-Wno-macro-redefine"
        "-Wno-unused-function"
        "-Wno-unused-lambda-capture"
        "-Wno-unused-parameter"
        "-Wno-unused-variable"
        "-Wno-unused-value"
        "-Wno-unused-but-set-variable"
        "-Wno-missing-field-initializers"
        #"-Wno-error=unused"

        "-Wno-unknown-pragmas"
        "-Wno-extra-semi"

        # Hmm...
        "-Wno-error=format="
)

################################################################################
#[[                          CONFIGURE DEPENDENCIES                          ]]#
################################################################################
include(FetchContent)

#[[                           CONFIGURE CYD FABRIC                           ]]#
FetchContent_Declare(cyd_fabric
        GIT_REPOSITORY https://github.com/castle055/cyd-fabric.git
        GIT_TAG dev
        FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(cyd_fabric)

#[[                           CONFIGURE CYD UI                               ]]#
FetchContent_Declare(cyd_ui
        GIT_REPOSITORY https://github.com/castle055/cyd-ui.git
        GIT_TAG ft/module-conversion
        FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(cyd_ui)


################################################################################
#[[                             CONFIGURE CYD WM                             ]]#
################################################################################
add_executable(cyd_wm)


FILE(GLOB_RECURSE SRC_LIST CONFIGURE_DEPENDS
        ${CYD_WM_INCLUDE_DIR}/*.h
        ${CYD_WM_INCLUDE_DIR}/*.hh
        ${CYD_WM_INCLUDE_DIR}/*.hpp
        ${CYD_WM_SOURCE_DIR}/*.h
        ${CYD_WM_SOURCE_DIR}/*.hh
        ${CYD_WM_SOURCE_DIR}/*.hpp
        ${CYD_WM_SOURCE_DIR}/*.c
        ${CYD_WM_SOURCE_DIR}/*.cc
        ${CYD_WM_SOURCE_DIR}/*.cpp
)
FILE(GLOB_RECURSE SRC_MOD_LIST CONFIGURE_DEPENDS
        ${CYD_WM_INCLUDE_DIR}/*.cppm
        ${CYD_WM_SOURCE_DIR}/*.cppm
)

target_include_directories(cyd_wm PUBLIC ${CYD_WM_INCLUDE_DIR})
target_include_directories(cyd_wm PUBLIC ${cyd_fabric_SOURCE_DIR}/include)
target_include_directories(cyd_wm PUBLIC ${cyd_ui_SOURCE_DIR}/include)

target_compile_features(cyd_wm PUBLIC cxx_std_23)
target_compile_definitions(cyd_wm PUBLIC -DXINERAMA)

target_sources(cyd_wm
        PUBLIC ${SRC_LIST}
        PUBLIC
        FILE_SET cxx_modules
        #        FILE_SET "${APP_NAME}_modfiles"
        TYPE CXX_MODULES
        BASE_DIRS ${CYD_UI_SOURCE_DIR} ${CYD_UI_INCLUDE_DIR}
        FILES ${SRC_MOD_LIST}
)

target_link_libraries(cyd_wm
        PUBLIC cyd_fabric cyd_ui
        PUBLIC std sensors
)

#LIST(APPEND EXT_LIBS
#        cydstd cyd_ui
#        X11 Xft Xrender Xinerama xcb
#        yaml-cpp fontconfig jpeg ${CAIROMM-1.16_LIBRARIES} sensors)

################################################################################
#[[                     CONFIGURE DOXYGEN DOCUMENTATION                      ]]#
################################################################################
if (CYD_WM_DEV_MODE AND CYD_WM_BUILD_DOCS)
    target_configure_doxygen(cyd_ui
            DOXYFILE ${CYD_WM_DIR}/Doxyfile
            BASE_DIR ${CYD_WM_DIR}
    )
endif ()



################################################################################
#[[                            CONFIGURE INSTALL                             ]]#
################################################################################
if (CYD_WM_DEV_MODE AND CMAKE_BUILD_TYPE MATCHES "Release")
    add_custom_command(TARGET cyd_wm POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory dist
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    )
    add_custom_command(TARGET cyd_wm POST_BUILD
            COMMAND cp -v
            $<TARGET_FILE:cyd_wm>
            ${CMAKE_CURRENT_LIST_DIR}/dist/
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
            COMMENT "Copying binary into distribution directory..."
    )
    add_custom_command(TARGET cyd_wm POST_BUILD
            COMMAND cp -rv
            ${CYD_WM_INCLUDE_DIR}
            ${CMAKE_CURRENT_LIST_DIR}/dist/
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
            COMMENT "Copying header files into distribution directory..."
    )
endif ()

