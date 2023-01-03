cmake_minimum_required(VERSION 3.17)

project(castle-dwm)

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

#enable_cxx_compiler_flag_if_supported("-Wno-error=unused")

enable_cxx_compiler_flag_if_supported("-Wno-unknown-pragmas")

include(build/CMakeLists.txt)

include_directories (/usr/include/freetype2)

#add_library(b_config config.h)
target_sources(castle-dwm PRIVATE config.h)
target_link_libraries(castle-dwm PUBLIC X11 fontconfig Xft Xinerama)
#target_link_libraries(castle-dwm PUBLIC program_shell)
target_link_libraries(castle-dwm PUBLIC yaml-cpp)

add_definitions(-DXINERAMA)
