aux_source_directory(src VCAMGETPIC_SOURCE_LIST_target)

#include directory ,useage : -I
include_directories(${Boost_INCLUDE_DIRS})

include_directories(${XSETPIC_PC_OPENCV_INCLUDE_ITEMS})


if(XSETPIC_WITH_LUA_OWN_COMPILE)
    include_directories("${XSETPIC_3RDPARTY_DIR}/lua/src")
else()
    if(LUA_FOUND)
        include_directories(${LUA_INCLUDE_DIR})
    endif()
endif()

include_directories("${XSETPIC_MODULES_DIR}/script/include")

include_directories("${XSETPIC_MODULES_DIR}/vcamgetpic/include")

#include_directories("${XSETPIC_MODULES_DIR}/setpic/include")

link_directories(${EXECUTABLE_OUTPUT_PATH})

set(VCAMGETPIC_TARGET  "vcamgetpic")

add_library(${VCAMGETPIC_TARGET} STATIC ${VCAMGETPIC_SOURCE_LIST_target})

#boost
target_link_libraries(${VCAMGETPIC_TARGET} ${Boost_LIBRARIES})

target_link_libraries(${VCAMGETPIC_TARGET} lua)

target_link_libraries(${VCAMGETPIC_TARGET} ${XSETPIC_PC_OPENCV_LIBRARY_ITEMS})

target_link_libraries(${VCAMGETPIC_TARGET} dl)

target_link_libraries(${VCAMGETPIC_TARGET} rt)

target_link_libraries(${VCAMGETPIC_TARGET} pthread)
