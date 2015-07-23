
aux_source_directory(src TEST_SETPIC_SOURCE_LIST_target)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

include_directories("${XSETPIC_MODULES_DIR}/global")

include_directories("${XSETPIC_MODULES_DIR}/vcamgetpic/include")

include_directories("${XSETPIC_MODULES_DIR}/setpic/include")

include_directories("${XSETPIC_MODULES_DIR}/script/include")

include_directories("${XSETPIC_MODULES_DIR}/camera/include")

include_directories("${GTK2_INCLUDE_DIRS}")

set(TEST_SETPIC_TARGET  "test_setpic")

add_executable(${TEST_SETPIC_TARGET} ${TEST_SETPIC_SOURCE_LIST_target})

target_link_libraries(${TEST_SETPIC_TARGET} vcamgetpic)

target_link_libraries(${TEST_SETPIC_TARGET} script)

#target_link_libraries(${TEST_SETPIC_TARGET} lua)

target_link_libraries(${TEST_SETPIC_TARGET} ${GTK2_LIBRARIES})

target_link_libraries(${TEST_SETPIC_TARGET} dl)

target_link_libraries(${TEST_SETPIC_TARGET} rt)

target_link_libraries(${TEST_SETPIC_TARGET} pthread)

