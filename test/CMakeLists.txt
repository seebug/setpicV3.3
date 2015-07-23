if(HAVE_CXX_STDINT_H)
    add_definitions(-DHAVE_CXX_STDINT_H)
endif()

if(HAVE_CXX_SIGNAL_H)
    add_definitions(-DHAVE_CXX_SIGNAL_H)
endif()

#GET CHECK_CXX_COMPILER_FLAG RESULT
# -std=c++11

if(XSETPIC_COMPILER_STANDARD11_SUPPORT)
    add_definitions("-DHAVE_CXX11_SUPPORT")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${XSETPIC_COMPILER_STANDARD11_FLAG}")
endif()

add_definitions("-D_SETPIC_IPC_FLAG_=0x205")

set(EXECUTABLE_OUTPUT_PATH "${TEST_OUTPUT_PATH}")

include_directories("${XSETPIC_MODULES_DIR}/global")

add_subdirectory(test_script)

if(GTK2_FOUND)
if(XSETPIC_BUILD_TEST_SETPIC)
add_subdirectory(test_setpic)
endif(XSETPIC_BUILD_TEST_SETPIC)
endif(GTK2_FOUND)

#add_subdirectory(test_camera)
