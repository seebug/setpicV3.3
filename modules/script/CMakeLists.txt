aux_source_directory(src SCRIPT_SOURCE_LIST_target)

#include directory ,useage : -I
include_directories(${Boost_INCLUDE_DIRS})

if(XSETPIC_WITH_LUA_OWN_COMPILE)
    include_directories("${XSETPIC_3RDPARTY_DIR}/lua/src")
else()
    if(LUA_FOUND)
        include_directories(${LUA_INCLUDE_DIR})
    endif()
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

set(SCRIPT_TARGET  "script")

file(COPY ./script/genicam.lua
    DESTINATION ${EXECUTABLE_OUTPUT_PATH}
    )

add_library(${SCRIPT_TARGET} STATIC ${SCRIPT_SOURCE_LIST_target})


#boost
#target_link_libraries(${SETPIC_TARGET} ${Boost_LIBRARIES})

target_link_libraries(${SETPIC_TARGET} lua)

#target_link_libraries(${SETPIC_TARGET} dl)

#target_link_libraries(${SETPIC_TARGET} rt)

#target_link_libraries(${SETPIC_TARGET} pthread)
