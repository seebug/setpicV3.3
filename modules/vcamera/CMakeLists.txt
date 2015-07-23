aux_source_directory(src VCAMERA_SOURCE_LIST_target)

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

include_directories("${XSETPIC_MODULES_DIR}/vcamera/include")

include_directories("${XSETPIC_MODULES_DIR}/camera/include")

include_directories("${XSETPIC_MODULES_DIR}/setpic/include")

link_directories(${EXECUTABLE_OUTPUT_PATH})

set(VCAMERA_TARGET  "vcamera")

add_library(${VCAMERA_TARGET} STATIC ${VCAMERA_SOURCE_LIST_target})

#boost
target_link_libraries(${VCAMERA_TARGET} ${Boost_LIBRARIES})

target_link_libraries(${VCAMERA_TARGET} ${XSETPIC_PC_OPENCV_LIBRARY_ITEMS})

target_link_libraries(${VCAMERA_TARGET} dl)

target_link_libraries(${VCAMERA_TARGET} rt)

target_link_libraries(${VCAMERA_TARGET} pthread)
