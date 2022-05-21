
add_executable(castle-dwm
        ${PROJECT_SOURCE_DIR}/castle-dwm.h
        ${PROJECT_SOURCE_DIR}/dwm.cpp
        ${PROJECT_SOURCE_DIR}/drw.h
        ${PROJECT_SOURCE_DIR}/drw.cpp
        ${PROJECT_SOURCE_DIR}/util.h
        ${PROJECT_SOURCE_DIR}/util.cpp
        )
include(${PROJECT_SOURCE_DIR}/../build/state/CMakeLists.txt)
include(${PROJECT_SOURCE_DIR}/../build/ops/CMakeLists.txt)
include(${PROJECT_SOURCE_DIR}/../build/layouts/CMakeLists.txt)
