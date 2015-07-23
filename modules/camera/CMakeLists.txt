aux_source_directory(src CAMERA_SOURCE_LIST_target)

include_directories(${Boost_INCLUDE_DIRS})

if(XSETPIC_WITH_LUA_OWN_COMPILE)
    include_directories("${XSETPIC_3RDPARTY_DIR}/lua/src")
else()
    if(LUA_FOUND)
        include_directories(${LUA_INCLUDE_DIR})
    endif()
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

include_directories("${XSETPIC_MODULES_DIR}/script/include")

include_directories("/usr/local/ebus_sdk/include")

link_directories("/usr/local/ebus_sdk/lib/")

link_directories("/usr/local/ebus_sdk/lib/genicam/bin/Linux32_i86/")

set(CAMERA_TARGET  "camera")

add_library(${CAMERA_TARGET} STATIC ${CAMERA_SOURCE_LIST_target})

target_link_libraries(${CAMERA_TARGET} ${Boost_LIBRARIES})

target_link_libraries(${CAMERA_TARGET} script)

target_link_libraries(${CAMERA_TARGET} lua)

target_link_libraries(${CAMERA_TARGET} dl)

target_link_libraries(${CAMERA_TARGET} rt)

target_link_libraries(${CAMERA_TARGET} pthread)

target_link_libraries(${CAMERA_TARGET} PvBase)

target_link_libraries(${CAMERA_TARGET} PvDevice)

target_link_libraries(${CAMERA_TARGET} PvBuffer)

target_link_libraries(${CAMERA_TARGET} PvGUI)

target_link_libraries(${CAMERA_TARGET} PvPersistence)

target_link_libraries(${CAMERA_TARGET} PvGenICam)

target_link_libraries(${CAMERA_TARGET} PvStreamRaw)

target_link_libraries(${CAMERA_TARGET} PvStream)

target_link_libraries(${CAMERA_TARGET} PvTransmitterRaw)

target_link_libraries(${CAMERA_TARGET} PvVirtualDevice)
