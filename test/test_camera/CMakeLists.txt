aux_source_directory(src Source_File)

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

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

include_directories("${XSETPIC_MODULES_DIR}/script/include")

include_directories("${XSETPIC_MODULES_DIR}/camera/include")

link_directories("/usr/local/ebus_sdk/lib/")

link_directories("/usr/local/ebus_sdk/lib/genicam/bin/Linux32_i86/")

add_definitions("-DSET_VERSION_COMPILE_HOST=\"${CMAKE_HOST_SYSTEM_NAME}\"")
add_definitions("-DSET_VERSION_COMPILE_OS=\"${CMAKE_HOST_SYSTEM_PROCESSOR}\"")
add_definitions("-DSET_VERSION_COMPLIE_KERNEL=\"${CMAKE_HOST_SYSTEM_VERSION}\"")
add_definitions("-D_V_CAM_MAJOR_VERSION_=3")
add_definitions("-D_V_CAM_MINOR_VERSION_=3")
add_definitions("-D_V_CAM_REVISION_VERSION=1")
add_definitions("-D_V_CAM_BUILD_VERSION=1")

set(SETPIC_TARGET  "test_camera")

add_executable(${SETPIC_TARGET} ${Source_File})


#boost
target_link_libraries(${SETPIC_TARGET} ${Boost_LIBRARIES})

target_link_libraries(${SETPIC_TARGET} ${XSETPIC_PC_OPENCV_LIBRARY_ITEMS})

target_link_libraries(${SETPIC_TARGET} lua)

target_link_libraries(${SETPIC_TARGET} script)

target_link_libraries(${SETPIC_TARGET} camera)

target_link_libraries(${SETPIC_TARGET} dl)

target_link_libraries(${SETPIC_TARGET} rt)

target_link_libraries(${SETPIC_TARGET} pthread)

target_link_libraries(${SETPIC_TARGET} PvBase)

target_link_libraries(${SETPIC_TARGET} PvDevice)

target_link_libraries(${SETPIC_TARGET} PvBuffer)

target_link_libraries(${SETPIC_TARGET} PvGUI)

target_link_libraries(${SETPIC_TARGET} PvPersistence)

target_link_libraries(${SETPIC_TARGET} PvGenICam)

target_link_libraries(${SETPIC_TARGET} PvStreamRaw)

target_link_libraries(${SETPIC_TARGET} PvStream)

target_link_libraries(${SETPIC_TARGET} PvTransmitterRaw)

target_link_libraries(${SETPIC_TARGET} PvVirtualDevice)
