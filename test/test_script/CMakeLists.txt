aux_source_directory(src TEST_SCRIPT_SOURCE_LIST_target)

if(XSETPIC_WITH_LUA_OWN_COMPILE)
    include_directories("${XSETPIC_3RDPARTY_DIR}/lua/src")
else()
    if(LUA_FOUND)
        include_directories(${LUA_INCLUDE_DIR})
    endif()
endif()

#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

include_directories("${XSETPIC_MODULES_DIR}/script/include")

set(TEST_SCRIPT_TARGET  "test_script")

add_executable(${TEST_SCRIPT_TARGET} ${TEST_SCRIPT_SOURCE_LIST_target})

target_link_libraries(${TEST_SCRIPT_TARGET} script)

target_link_libraries(${TEST_SCRIPT_TARGET} lua)

target_link_libraries(${TEST_SCRIPT_TARGET} dl)

target_link_libraries(${TEST_SCRIPT_TARGET} rt)

target_link_libraries(${TEST_SCRIPT_TARGET} pthread)

