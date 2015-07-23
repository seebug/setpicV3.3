#SET CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE,
#FOR SUPPORT IF()ELSE()ENDIF()
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

#SET FOR CMAKE 2.7 AND ABOVE
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif()

#SET CMAKE MIN VERSION,FUNCTION CMAKE_MINIMUM_REQUIRED IS NEEDED.
#FOR XSETPIC ,CMAKE_VERSION MUST BE GREATER THAN 2.8.3 .FOR COMPILE XSETPIC
#MAKE SURE YOUR CMAKE VERSION IS CORRECT.
if(CMAKE_GENERATOR MATCHES Xcode AND XCODE_VERSION VERSION_GREATER 4.3)
    cmake_minimum_required(VERSION 2.8.8)
else()
    cmake_minimum_required(VERSION 2.8.0)
endif()

message(STATUS "Cmake Version:${CMAKE_VERSION}")
SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(CMAKE_SUPPRESS_REGENERATION TRUE)
SET(CMAKE_USE_RELATIVE_PATHS ON)
option(XSETPIC_DEBUG_SWITCHER "CMake Debug Information Output" OFF)
mark_as_advanced(FORCE XSETPIC_DEBUG_SWITCHER)

#SET XSETPIC CMAKE_CONFIGURATION_TYPES
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)
if(DEFINED CMAKE_BUILD_TYPE AND CMAKE_VERSION VERSION_GREATER "2.8")
    set_property( CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES} )
endif()

#SET YOUR PROJECT NAME.AS FOR VISUAL STUDIO,THE SUBFIX OF THE PROJECT IS .sln (SLUTION)
project(XSetPic)

#THE BASE MACRO AND FUNCTION IS DEFINED HERE
include(cmake/xsetpic.utils.cmake)

#CHECK YOUR COMPILER INSTALLATION.IF IT NOT WORK WELL , THE CMAKE WILL STOP UNTILL IT WORKS WELL.
if(NOT CMAKE_SIZEOF_VOID_P GREATER 0)
  message(FATAL_ERROR "CMake fails to deterimine the bitness of target platform.
  Please check your CMake and compiler installation. If you are crosscompiling then ensure that your CMake toolchain file correctly sets the compiler details.")
endif()

#DETECTING YOUR COMPILER.
include(cmake/xsetpic.detectcxxcompiler.cmake)

if(UNIX AND NOT ANDROID)
    if(X86_64 OR CMAKE_SIZEOF_VOID_P EQUAL 8)
        if(EXISTS /lib64)
            list(APPEND CMAKE_LIBRARY_PATH /lib64)
        else()
            list(APPEND CMAKE_LIBRARY_PATH /lib)
        endif()
        if(EXISTS /usr/lib64)
            list(APPEND CMAKE_LIBRARY_PATH /usr/lib64)
        else()
            list(APPEND CMAKE_LIBRARY_PATH /usr/lib)
        endif()
        if(EXISTS /usr/local/lib64)
            list(APPEND CMAKE_LIBRARY_PATH /usr/local/lib64)
        elseif(EXISTS /usr/local/lib)
            list(APPEND CMAKE_LIBRARY_PATH /usr/local/lib)
        else()
        endif()
    elseif(X86 OR CMAKE_SIZEOF_VOID_P EQUAL 4)
        if(EXISTS /lib32)
            list(APPEND CMAKE_LIBRARY_PATH /lib32)
        else()
            list(APPEND CMAKE_LIBRARY_PATH /lib)
        endif()
        if(EXISTS /usr/lib32)
            list(APPEND CMAKE_LIBRARY_PATH /usr/lib32)
        else()
            list(APPEND CMAKE_LIBRARY_PATH /usr/lib)
        endif()
        if(EXISTS /usr/local/lib32)
            list(APPEND CMAKE_LIBRARY_PATH /usr/local/lib32)
        elseif(EXISTS /usr/local/lib)
            list(APPEND CMAKE_LIBRARY_PATH /usr/local/lib)
        else()
        endif()
    endif()
endif()


#--------------------------------------------------------------------------------------------------

#----------------
# Boost
#----------------
#
#Can be set to OFF to use the non-multithreaded boost libraries.  If not specified, defaults to ON
#
XSETPIC_OPTION(Boost_USE_MULTITHREADED "non-multithreaded boost libraries" ON )

#Can be set to ON to force the use of the static boost libraries. Defaults to OFF.
#
XSETPIC_OPTION(Boost_USE_STATIC_LIBS "force the use of the static boost libraries" ON )

#Set to TRUE to suppress searching in system paths (or other locations outside of BOOST_ROOT
#or BOOST_INCLUDEDIR).  Useful when specifying BOOST_ROOT. Defaults to OFF. [Since CMake 2.8.3]
#
if( CMAKE_VERSION VERSION_GREATER "2.8.3")
    XSETPIC_OPTION(Boost_NO_SYSTEM_PATHS  "suppress searching in system" OFF )
endif()

#Do not do a find_package call in config mode before searching for a regular boost install.
#This will avoid finding boost-cmake installs.Defaults to OFF. [Since CMake 2.8.6]
#
if( CMAKE_VERSION VERSION_GREATER "2.8.6")
    XSETPIC_OPTION(Boost_NO_BOOST_CMAKE  "will avoid finding boost-cmake-installs" OFF )
endif()


if( CMAKE_VERSION VERSION_GREATER "2.8.3" )
    #If enabled, searches for boost libraries linked against a static C++ standard library
    #('s' ABI tag). This option should be set to ON or OFF because the default behavior
    #if not specified is platform dependent for backwards compatibility.[Since CMake 2.8.3]
    #
    XSETPIC_OPTION(Boost_USE_STATIC_RUNTIME "searches for boost libraries linked against a static C++ standard library" ON )

    #If enabled, searches for boost libraries compiled against a special debug build of
    #Python ('y' ABI tag). Defaults to OFF. [Since CMake 2.8.3]
    XSETPIC_OPTION(Boost_USE_DEBUG_PYTHON "searches for boost libraries compiled against a special debug build of Python" OFF )

    #If enabled, searches for boost libraries compiled against the STLPort standard
    #library ('p' ABI tag). Defaults to OFF.[Since CMake 2.8.3]
    XSETPIC_OPTION(Boost_USE_STLPORT "searches for boost libraries compiled against the STLPort standard library" OFF)

    #If enabled, searches for boost libraries compiled against the deprecated STLPort
    #"native iostreams" feature ('n' ABI tag).Defaults to OFF. [Since CMake 2.8.3]
    XSETPIC_OPTION(Boost_USE_STLPORT_DEPRECATED_NATIVE_IOSTREAMS "If enabled, searches for boost libraries compiled against the deprecated STLPort 'native iostreams'" OFF )
endif()

#FindBoost doesn't output detailed information about why it failed or how to fix the problem
#unless this is set to TRUE or the REQUIRED keyword is specified in find_package().[Since CMake 2.8.0]
if( CMAKE_VERSION VERSION_GREATER "2.8.0" )
    XSETPIC_OPTION(Boost_DETAILED_FAILURE_MSG "findBoost output switcher" ON)
endif()


#XSETPIC_OPTION(XSETPIC_WITH_LUA_SHARED_LIBRARY  "Compile With Lua Static Library"   OFF)

XSETPIC_OPTION(XSETPIC_WITH_LUA_OWN_COMPILE     "Use XSetPic Default Lua Library"   ON)

SET(XSETPIC_WITH_OPENCV_VERSION "2.4.3" CACHE STRING "The Base Version Of XSetPic Need")

#if switch < ON > ,the file searched <path>/<prefix>calib3d<subfix> will be compiled with,if exists.
#always,flags by -l ,useage: -l<path>/<prefix>calib3d<subfix>
XSETPIC_OPTION(XSETPIC_WITH_OPENCV_calib3d "Compile With Library,Useage: -l<path>/<prefix>calib3d<subfix> " ON )

#if switch < ON > ,the file searched <path>/<prefix>contrib<subfix> will be compiled with,if exists.
#always,flags by -l ,useage: -l<path>/<prefix>contrib<subfix>
XSETPIC_OPTION(XSETPIC_WITH_OPENCV_contrib "Compile With Library,Useage: -l<path>/<prefix>contrib<subfix> " ON )

#if switch < ON > ,the file searched <path>/<prefix>core<subfix> will be compiled with,if exists.
#always,flags by -l ,useage: -l<path>/<prefix>core<subfix>
XSETPIC_OPTION(XSETPIC_WITH_OPENCV_core "Compile With Library,Useage: -l<path>/<prefix>core<subfix> " ON )

#if switch < ON > ,the file searched <path>/<prefix>features2d<subfix> will be compiled with,if exists.
#always,flags by -l ,useage: -l<path>/<prefix>features2d<subfix>
XSETPIC_OPTION(XSETPIC_WITH_OPENCV_features2d "Compile With Library,Useage: -l<path>/<prefix>features2d<subfix> " ON )

#if switch < ON > ,the file searched <path>/<prefix>flann<subfix> will be compiled with,if exists.
#always,flags by -l ,useage: -l<path>/<prefix>flann<subfix>
XSETPIC_OPTION(XSETPIC_WITH_OPENCV_flann "Compile With Library,Useage: -l<path>/<prefix>flann<subfix> " ON )

#if switch < ON > ,the file searched <path>/<prefix>gpu<subfix> will be compiled with,if exists.
#always,flags by -l ,useage: -l<path>/<prefix>gpu<subfix>
XSETPIC_OPTION(XSETPIC_WITH_OPENCV_gpu "Compile With Library,Useage: -l<path>/<prefix>gpu<subfix> " ON )

#if switch < ON > ,the file searched <path>/<prefix>highgui<subfix> will be compiled with,if exists.
#always,flags by -l ,useage: -l<path>/<prefix>highgui<subfix>
XSETPIC_OPTION(XSETPIC_WITH_OPENCV_highgui "Compile With Library,Useage: -l<path>/<prefix>highgui<subfix> " ON )

#if switch < ON > ,the file searched <path>/<prefix>imgproc<subfix> will be compiled with,if exists.
#always,flags by -l ,useage: -l<path>/<prefix>imgproc<subfix>
XSETPIC_OPTION(XSETPIC_WITH_OPENCV_imgproc "Compile With Library,Useage: -l<path>/<prefix>imgproc<subfix> " ON )

#if switch < ON > ,the file searched <path>/<prefix>legacy<subfix> will be compiled with,if exists.
#always,flags by -l ,useage: -l<path>/<prefix>legacy<subfix>
XSETPIC_OPTION(XSETPIC_WITH_OPENCV_legacy "Compile With Library,Useage: -l<path>/<prefix>legacy<subfix> " ON )

#if switch < ON > ,the file searched <path>/<prefix>ml<subfix> will be compiled with,if exists.
#always,flags by -l ,useage: -l<path>/<prefix>ml<subfix>
XSETPIC_OPTION(XSETPIC_WITH_OPENCV_ml "Compile With Library,Useage: -l<path>/<prefix>ml<subfix> " ON )

#if switch < ON > ,the file searched <path>/<prefix>nonfree<subfix> will be compiled with,if exists.
#always,flags by -l ,useage: -l<path>/<prefix>nonfree<subfix>
XSETPIC_OPTION(XSETPIC_WITH_OPENCV_nonfree "Compile With Library,Useage: -l<path>/<prefix>nonfree<subfix> " OFF )

#if switch < ON > ,the file searched <path>/<prefix>objdetect<subfix> will be compiled with,if exists.
#always,flags by -l ,useage: -l<path>/<prefix>objdetect<subfix>
XSETPIC_OPTION(XSETPIC_WITH_OPENCV_objdetect "Compile With Library,Useage: -l<path>/<prefix>objdetect<subfix> " ON )

#if switch < ON > ,the file searched <path>/<prefix>photo<subfix> will be compiled with,if exists.
#always,flags by -l ,useage: -l<path>/<prefix>photo<subfix>
XSETPIC_OPTION(XSETPIC_WITH_OPENCV_photo "Compile With Library,Useage: -l<path>/<prefix>photo<subfix> " ON )

#if switch < ON > ,the file searched <path>/<prefix>stitching<subfix> will be compiled with,if exists.
#always,flags by -l ,useage: -l<path>/<prefix>stitching<subfix>
XSETPIC_OPTION(XSETPIC_WITH_OPENCV_stitching "Compile With Library,Useage: -l<path>/<prefix>stitching<subfix> " ON )

#if switch < ON > ,the file searched <path>/<prefix>ts<subfix> will be compiled with,if exists.
#always,flags by -l ,useage: -l<path>/<prefix>ts<subfix>
XSETPIC_OPTION(XSETPIC_WITH_OPENCV_ts "Compile With Library,Useage: -l<path>/<prefix>ts<subfix> " ON )

#if switch < ON > ,the file searched <path>/<prefix>video<subfix> will be compiled with,if exists.
#always,flags by -l ,useage: -l<path>/<prefix>video<subfix>
XSETPIC_OPTION(XSETPIC_WITH_OPENCV_video "Compile With Library,Useage: -l<path>/<prefix>video<subfix> " ON )

#if switch < ON > ,the file searched <path>/<prefix>videostab<subfix> will be compiled with,if exists.
#always,flags by -l ,useage: -l<path>/<prefix>videostab<subfix>
XSETPIC_OPTION(XSETPIC_WITH_OPENCV_videostab "Compile With Library,Useage: -l<path>/<prefix>videostab<subfix> " ON )

XSETPIC_OPTION(XSETPIC_BUILD_TEST_SETPIC "Compile With Library,Useage: -l<path>/<prefix>videostab<subfix> " OFF)


#--------------------------------------------------------------------------------------------------

# It sets the following variables:
#PKG_CONFIG_FOUND       :if pkg-config executable was found
#PKG_CONFIG_EXECUTABLE  :pathname of the pkg-config program
include(cmake/xsetpic.findpkgconfig.tool.cmake)

#detect boost for xsetpic .
#Boost_FOUND        :System has Boost, this means the include dir was found, as well as all the libraries specified in
#the COMPONENTS list.
#Boost_INCLUDE_DIRS :Boost include directories: not cached
#Boost_INCLUDE_DIR  :This is almost the same as above, but this one is cached and may be modified by advanced users
#Boost_LIBRARIES    :Link to these to use the Boost libraries that you specified: not cached
#Boost_LIBRARY_DIRS :The path to where the Boost library files are.
#Boost_VERSION      :The version number of the boost libraries that have been found, same as in version.hpp from Boost
#Boost_LIB_VERSION  :The version number in filename form as it's appended to the library filenames
#Boost_MAJOR_VERSION :major version number of boost
#Boost_MINOR_VERSION :minor version number of boost
#Boost_SUBMINOR_VERSION :subminor version number of boost
include(cmake/xsetpic.detectboost.library.cmake)

#XSETPIC_PC_OPENCV_FOUND         :the opencv is found ,just check by pkg-config
#XSETPIC_PC_OPENCV_INCLUDE_DIRS  :the opencv include directories , getting from pkg-config
#XSETPIC_PC_OPENCV_LIBRARY_ITEMS :opencv include directories
#XSETPIC_PC_OPENCV_INCLUDE_ITEMS :opencv library
#XSETPIC_PC_OPENCV_VERSION       :opencv version
include(cmake/xsetpic.detectopencv.library.cmake)

#LUA_FOUND          :if false, do not try to link to Lua
#LUA_LIBRARIES      :both lua and lualib
#LUA_INCLUDE_DIR    :where to find lua.h
#LUA_VERSION_STRING :the version of Lua found
#LUA_VERSION_MAJOR  :the major version of Lua
#LUA_VERSION_MINOR  :the minor version of Lua
#LUA_VERSION_PATCH  :the patch version of Lua
include(cmake/xsetpic.detectlua.library.cmake)

include(cmake/xsetpic.detectgtk.library.cmake)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin" CACHE PATH "Output directory for applications" )

set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/libs" CACHE PATH "Output directory for library")

set(3P_LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/3rdparty" CACHE PATH "Output directory for library")

set(TEST_OUTPUT_PATH "${PROJECT_BINARY_DIR}/test/bin" CACHE PATH "Output directory for testing")

set(TOOL_OUTPUT_PATH "${PROJECT_BINARY_DIR}/tools/bin" CACHE PATH "Output directory for tool")

set(XSETPIC_MODULES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/modules")

set(XSETPIC_3RDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty")

set(XSETPIC_TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test")

set(XSETPIC_TOOLS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tools")

set(XSETPIC_BUILD_INFO_FILE "${CMAKE_BINARY_DIR}/.tmpInfo" CACHE FILE "XSetPic Build Information File")


include(CheckIncludeFile)
include(CheckIncludeFileCXX)
#CHECK_INCLUDE_FILE(a,b)
CHECK_INCLUDE_FILE_CXX(stdint.h HAVE_CXX_STDINT_H)
CHECK_INCLUDE_FILE_CXX(unistd.h HAVE_CXX_UNISTD_H)
CHECK_INCLUDE_FILE_CXX(pthread.h HAVE_CXX_PTHREAD_H)
CHECK_INCLUDE_FILE_CXX(signal.h HAVE_CXX_SIGNAL_H)
CHECK_INCLUDE_FILE_CXX(sys/time.h HAVE_CXX_SYS__TIME_H)
CHECK_INCLUDE_FILE_CXX(sys/types.h HAVE_CXX_SYS__TYPES_H)
CHECK_INCLUDE_FILE_CXX(sys/stat.h HAVE_CXX_SYS__STAT_H)
CHECK_INCLUDE_FILE_CXX(fcntl.h HAVE_CXX_FCNTL_H)
CHECK_INCLUDE_FILE_CXX(sys/shm.h HAVE_CXX_SYS__SHM_H)


include(CheckCXXCompilerFlag)
set(XSETPIC_COMPILER_STANDARD11_FLAG "-std=c++11" CACHE STRING "Compile Standard,useage:-std=c++11")
CHECK_CXX_COMPILER_FLAG(${XSETPIC_COMPILER_STANDARD11_FLAG} XSETPIC_COMPILER_STANDARD11_SUPPORT)




if(UNIX)
    include(CheckFunctionExists)
    include(CheckIncludeFile)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_DL_LIB} -ldl -lpthread -lrt")
endif()


#target_link_libraries(lua ${XSETPIC_LINKER_LIBS})


#---------------------------------------------------------------------------
# Sub CMake List
#---------------------------------------------------------------------------

add_subdirectory(3rdparty)

add_subdirectory(modules)

#add_subdirectory(tools)

add_subdirectory(test)

#---------------------------------------------------------------------------
# Information of XSetPic
#---------------------------------------------------------------------------

status("")
status("---------- SetPic Platform information ---------------")
status("")
status("    Host:"             ${CMAKE_HOST_SYSTEM_NAME} ${CMAKE_HOST_SYSTEM_VERSION} ${CMAKE_HOST_SYSTEM_PROCESSOR})
if(CMAKE_CROSSCOMPILING)
    status("    Target:"         ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION} ${CMAKE_SYSTEM_PROCESSOR})
endif()
status("    CMake:"            ${CMAKE_VERSION})
status("    CMake generator:"  ${CMAKE_GENERATOR})
status("    CMake build tool:" ${CMAKE_BUILD_TOOL})
if(MSVC)
    status("    MSVC:"           ${MSVC_VERSION})
endif()
if(CMAKE_GENERATOR MATCHES Xcode)
    status("    Xcode:"          ${XCODE_VERSION})
endif()
if(NOT CMAKE_GENERATOR MATCHES "Xcode|Visual Studio")
    status("    Configuration:"  ${CMAKE_BUILD_TYPE})
endif()
status("")
status("---------- SetPic Compile information -----------------")
status("")
if(Boost_FOUND)
    status("    Boost" "${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}")
endif()
if(XSETPIC_PC_OPENCV_FOUND)
    status("    OpenCV" "${XSETPIC_PC_OPENCV_VERSION}")
endif()
if(LUA_FOUND)
    status("    Lua" "${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}.${LUA_VERSION_PATCH}")
endif()
status("")
status("--------------------------------------------------------")
status("")

