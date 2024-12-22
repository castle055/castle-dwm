include(CheckCXXCompilerFlag)

set(CMAKE_REQUIRED_QUIET true)
function(enable_cxx_compiler_flag_if_supported flag)
    string(FIND "${CMAKE_CXX_FLAGS}" "${flag}" flag_already_set)
    if (flag_already_set EQUAL -1)
        check_cxx_compiler_flag("${flag}" flag_supported)
        if (flag_supported)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
            message("   [ OK  ] Compiler flag: ${flag}")
        else ()
            message("   [ERROR] Compiler flag: ${flag} <--(Not supported)")
        endif ()
        unset(flag_supported CACHE)
    else ()
        message("   [WARN ] Compiler flag: ${flag} <--(Already set)")
    endif ()
endfunction()

macro(compiler_flags)
    message("Setting Compiler Flags:")
    foreach (flag ${ARGN})
        enable_cxx_compiler_flag_if_supported("${flag}")
    endforeach ()
endmacro()
