#--------------------------------------------------------------------------------------------------
#
# FILENAME  :xsetpic_detectcxxcompiler.cmake
#
# PURPOSE   :INIT/FIND CPLUSPLUS COMPILER
#
# NOTE      :
#
#               <perturbed@sina.com>
#--------------------------------------------------------------------------------------------------

if(CMAKE_CL_64)
    set(MSVC64  1)
endif()

#-------------------------------------------------------
#
#DETECT GUN C/CXX COMPILER 
#
#-------------------------------------------------------
#message(STATUS "================================${CMAKE_CXX_COMPILER_ID}")
#message(STATUS "================================${CMAKE_C_COMPILER_ID}")
if(NOT APPLE)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_COMPILER_IS_GNUCXX 1)
    set(ENABLE_PRECOMPILED_HEADERS OFF CACHE BOOL "" FORCE)
  endif()
  if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_COMPILER_IS_GNUCC 1)
    set(ENABLE_PRECOMPILED_HEADERS OFF CACHE BOOL "" FORCE)
  endif()
endif()

#-------------------------------------------------------
#
# XSETPIC SET FOR COMPILER GUNCXX
#
#--------------------------------------------------------

if(CMAKE_COMPILER_IS_GNUCXX)
  set(XSETPIC_COMPILER_IS_GNU TRUE)
else()
  set(XSETPIC_COMPILER_IS_GNU FALSE)
endif()

#--------------------------------------------------------
#
# execute_process(COMMAND <cmd1>[args1...]]
#                 [COMMAND <cmd2>[args2...] [...]]
#                  [WORKING_DIRECTORY<directory>]
#                  [TIMEOUT<seconds>]
#                  [RESULT_VARIABLE<variable>]
#                  [OUTPUT_VARIABLE <variable>]
#                  [ERROR_VARIABLE<variable>]
#                  [INPUT_FILE<file>]
#                  [OUTPUT_FILE<file>]
#                  [ERROR_FILE<file>]
#                  [OUTPUT_QUIET]
#                  [ERROR_QUIET]
#                 [OUTPUT_STRIP_TRAILING_WHITESPACE]
#                 [ERROR_STRIP_TRAILING_WHITESPACE])
#-------------------------------------------------------

if(CMAKE_COMPILER_IS_GNUCXX)
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_ARG1} --version
                  OUTPUT_VARIABLE CMAKE_XSETPIC_GCC_VERSION_FULL
                  OUTPUT_STRIP_TRAILING_WHITESPACE)

    execute_process(COMMAND ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_ARG1} -v
                  ERROR_VARIABLE CMAKE_XSETPIC_GCC_INFO_FULL
                  OUTPUT_STRIP_TRAILING_WHITESPACE)

    # Typical output in CMAKE_XSETPIC_GCC_VERSION_FULL: "c+//0 (whatever) 4.2.3 (...)"
    # Look for the version number
    string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" CMAKE_GCC_REGEX_VERSION "${CMAKE_XSETPIC_GCC_VERSION_FULL}")
    if(NOT CMAKE_GCC_REGEX_VERSION)
      string(REGEX MATCH "[0-9]+\\.[0-9]+" CMAKE_GCC_REGEX_VERSION "${CMAKE_XSETPIC_GCC_VERSION_FULL}")
    endif()

    # Split the three parts:
    string(REGEX MATCHALL "[0-9]+" CMAKE_XSETPIC_GCC_VERSIONS "${CMAKE_GCC_REGEX_VERSION}")

    list(GET CMAKE_XSETPIC_GCC_VERSIONS 0 CMAKE_XSETPIC_GCC_VERSION_MAJOR)
    list(GET CMAKE_XSETPIC_GCC_VERSIONS 1 CMAKE_XSETPIC_GCC_VERSION_MINOR)

    set(CMAKE_XSETPIC_GCC_VERSION ${CMAKE_XSETPIC_GCC_VERSION_MAJOR}${CMAKE_XSETPIC_GCC_VERSION_MINOR})
    math(EXPR CMAKE_XSETPIC_GCC_VERSION_NUM "${CMAKE_XSETPIC_GCC_VERSION_MAJOR}*100 + ${CMAKE_XSETPIC_GCC_VERSION_MINOR}")
    message(STATUS "Detected version of GNU GCC: ${CMAKE_XSETPIC_GCC_VERSION} (${CMAKE_XSETPIC_GCC_VERSION_NUM})")

    if(WIN32)
        execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpmachine
                  OUTPUT_VARIABLE CMAKE_XSETPIC_GCC_TARGET_MACHINE
                  OUTPUT_STRIP_TRAILING_WHITESPACE)
        if(CMAKE_XSETPIC_GCC_TARGET_MACHINE MATCHES "64")
            set(MINGW64 1)
        endif()
    endif()
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "amd64.*|x86_64.*|AMD64.*" OR CMAKE_GENERATOR MATCHES "Visual Studio.*Win64")
    set(X86_64 1)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "i686.*|i386.*|x86.*|amd64.*|AMD64.*")
    set(X86 1)
endif()


