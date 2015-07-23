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

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/global")

add_definitions("-D_SETPIC_IPC_FLAG_=0x205")

add_subdirectory(script)

add_subdirectory(algorithm)

add_subdirectory(camera)

add_subdirectory(vcamera)

add_subdirectory(setpic)

add_subdirectory(vcamgetpic)