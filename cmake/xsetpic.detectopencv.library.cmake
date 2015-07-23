#--------------------------------------------------------------------------------------------------
#
# FILENAME  :xsetpic.detectopencv.library.cmake
#
# PURPOSE   :Find OpenCV Library by PkgConfig
#
# NOTE      :
#
#                               <perturbed@sina.com>
#--------------------------------------------------------------------------------------------------

###################################################################################################
#
#
# After Executed:
#
#   XSETPIC_PC_OPENCV_FOUND         :the opencv is found ,just check by pkg-config
#   XSETPIC_PC_OPENCV_INCLUDE_DIRS  :the opencv include directories , getting from pkg-config
#
#   XSETPIC_PC_OPENCV_LIBRARY_ITEMS :opencv include directories
#   XSETPIC_PC_OPENCV_INCLUDE_ITEMS :opencv library 
#
#   XSETPIC_PC_OPENCV_VERSION       :opencv version
#
#   ....... 
#
#
#
#    for more information. 
#       read the document of cmake . 
#       read < FindPkgConfig.cmake > of cmake.
#                               <perturbed@sina.com>
#
#
#
###################################################################################################

include(cmake/xsetpic.utils.cmake)


##pkg_check_modules(PC_OPENCV opencv)
## CMake developers made the UsePkgConfig system deprecated in the same release (2.6)
## where they added pkg_check_modules. Consequently I need to support both in my scripts
## to avoid those deprecated warnings. Here's a helper that does just that.
## Works identically to pkg_check_modules, except that no checks are needed prior to use.
#macro (libfind_pkg_check_modules PREFIX PKGNAME)
#  if (${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4)
#    include(UsePkgConfig)
#    pkgconfig(${PKGNAME} ${PREFIX}_INCLUDE_DIRS ${PREFIX}_LIBRARY_DIRS ${PREFIX}_LDFLAGS ${PREFIX}_CFLAGS)
#  else (${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4)
#    find_package(PkgConfig)
#    if (PKG_CONFIG_FOUND)
#      pkg_check_modules(${PREFIX} ${PKGNAME})
#    endif (PKG_CONFIG_FOUND)
#  endif (${CMAKE_MAJOR_VERSION} EQUAL 2 AND ${CMAKE_MINOR_VERSION} EQUAL 4)
#endmacro (libfind_pkg_check_modules)
#

libfind_pkg_check_modules(XSETPIC_PC_OPENCV opencv)

#pkg_check_modules(XSETPIC_PC_OPENCV opencv)


if(${XSETPIC_PC_OPENCV_VERSION} VERSION_LESS ${XSETPIC_WITH_OPENCV_VERSION})
    message(FATAL_ERROR "\nXSetpic require OpenCV version at least : ${XSETPIC_WITH_OPENCV_VERSION}\nFor better compile or run,suggest your install the version of opencv:${XSETPIC_WITH_OPENCV_VERSION}\nCurrent version of opencv :${XSETPIC_PC_OPENCV_VERSION}\n\n")
endif()
#message(STATUS "${XSETPIC_PC_OPENCV_VERSION}")

if(NOT XSETPIC_PC_OPENCV_FOUND)
    message(FATAL_ERROR "Cmake failed to find opencv.Please check your opencv installation and ensure it works well.")
endif(NOT XSETPIC_PC_OPENCV_FOUND)

set(LIST_OPENCV_INCLUDE_DIR_ITEM ${XSETPIC_EMPTY})
list(APPEND LIST_OPENCV_INCLUDE_DIR_ITEM opencv/cv.hpp)
list(APPEND LIST_OPENCV_INCLUDE_DIR_ITEM opencv2/calib3d/calib3d.hpp)
list(APPEND LIST_OPENCV_INCLUDE_DIR_ITEM opencv2/contrib/contrib.hpp)
list(APPEND LIST_OPENCV_INCLUDE_DIR_ITEM opencv2/core/core.hpp)
list(APPEND LIST_OPENCV_INCLUDE_DIR_ITEM opencv2/features2d/features2d.hpp)
list(APPEND LIST_OPENCV_INCLUDE_DIR_ITEM opencv2/flann/flann.hpp)
list(APPEND LIST_OPENCV_INCLUDE_DIR_ITEM opencv2/gpu/gpu.hpp)
list(APPEND LIST_OPENCV_INCLUDE_DIR_ITEM opencv2/highgui/highgui.hpp)
list(APPEND LIST_OPENCV_INCLUDE_DIR_ITEM opencv2/imgproc/imgproc.hpp)
list(APPEND LIST_OPENCV_INCLUDE_DIR_ITEM opencv2/legacy/legacy.hpp)
list(APPEND LIST_OPENCV_INCLUDE_DIR_ITEM opencv2/ml/ml.hpp)
list(APPEND LIST_OPENCV_INCLUDE_DIR_ITEM opencv2/nonfree/nonfree.hpp)
list(APPEND LIST_OPENCV_INCLUDE_DIR_ITEM opencv2/photo/photo.hpp)
list(APPEND LIST_OPENCV_INCLUDE_DIR_ITEM opencv2/stitching/stitcher.hpp)
list(APPEND LIST_OPENCV_INCLUDE_DIR_ITEM opencv2/ts/ts.hpp)
list(APPEND LIST_OPENCV_INCLUDE_DIR_ITEM opencv2/video/video.hpp)
list(APPEND LIST_OPENCV_INCLUDE_DIR_ITEM opencv2/videostab/videostab.hpp)
list(APPEND LIST_OPENCV_INCLUDE_DIR_ITEM opencv2/opencv.hpp)

#message(STATUS "====================== ${LIST_OPENCV_INCLUDE_DIR_ITEM}")

set(___xsetpic_include_dir_list ${XSETPIC_EMPTY})
list(APPEND ___xsetpic_include_dir_list ${XSETPIC_PC_OPENCV_INCLUDE_DIRS})
foreach(__m_LIST_OPENCV_INCLUDE_DIR_ITEM ${LIST_OPENCV_INCLUDE_DIR_ITEM})
    set(_include ${__m_LIST_OPENCV_INCLUDE_DIR_ITEM})
    set(_result  _XSETPIC_${__m_LIST_OPENCV_INCLUDE_DIR_ITEM})
    find_path(${_result} NAMES ${_include} HINT ${XSETPIC_PC_OPENCV_INCLUDE_DIRS} ENV )
    if(_temp_XSETPIC_PC_OPENCV_INCLUDE_DIR_NOTFOUND)
        message(FATAL_ERROR "Cmake failed to find opencv include directory.Please check your opencv installation and ensure it works well.")
    else()
        list(APPEND ___xsetpic_library_list ${${_result}})
        #message(STATUS "===${${_result}}")
    endif(_temp_XSETPIC_PC_OPENCV_INCLUDE_DIR_NOTFOUND)
    unset(_include CACHE)
    unset(${_result} CACHE)
    unset(_result CACHE)
endforeach()
list(REMOVE_DUPLICATES ___xsetpic_include_dir_list)
set(XSETPIC_PC_OPENCV_INCLUDE_ITEMS ${___xsetpic_include_dir_list})
unset(___xsetpic_include_dir_list CACHE)

set(LIST_OPENCV_LIBRARY_DIR_ITEM ${XSETPIC_EMPTY})
list(APPEND LIST_OPENCV_LIBRARY_DIR_ITEM calib3d)
list(APPEND LIST_OPENCV_LIBRARY_DIR_ITEM contrib)
list(APPEND LIST_OPENCV_LIBRARY_DIR_ITEM core)
list(APPEND LIST_OPENCV_LIBRARY_DIR_ITEM features2d)
list(APPEND LIST_OPENCV_LIBRARY_DIR_ITEM flann)
list(APPEND LIST_OPENCV_LIBRARY_DIR_ITEM gpu)
list(APPEND LIST_OPENCV_LIBRARY_DIR_ITEM highgui)
list(APPEND LIST_OPENCV_LIBRARY_DIR_ITEM imgproc)
list(APPEND LIST_OPENCV_LIBRARY_DIR_ITEM legacy)
list(APPEND LIST_OPENCV_LIBRARY_DIR_ITEM ml)
list(APPEND LIST_OPENCV_LIBRARY_DIR_ITEM nonfree)
list(APPEND LIST_OPENCV_LIBRARY_DIR_ITEM objdetect)
list(APPEND LIST_OPENCV_LIBRARY_DIR_ITEM photo)
list(APPEND LIST_OPENCV_LIBRARY_DIR_ITEM stitching)
list(APPEND LIST_OPENCV_LIBRARY_DIR_ITEM ts)
list(APPEND LIST_OPENCV_LIBRARY_DIR_ITEM video)
list(APPEND LIST_OPENCV_LIBRARY_DIR_ITEM videostab)
set(LIST_OPENCV_LIBRARY_DIR_ITEM_PREFIXS libopencv_)
set(LIST_OPENCV_LIBRARY_DIR_ITEM_SUBFIXS .so)

set(___xsetpic_library_list ${XSETPIC_EMPTY})
foreach(__m_LIST_OPENCV_LIBRARY_DIR_ITEM ${LIST_OPENCV_LIBRARY_DIR_ITEM})
    if(XSETPIC_WITH_OPENCV_${__m_LIST_OPENCV_LIBRARY_DIR_ITEM})
        set(_library ${LIST_OPENCV_LIBRARY_DIR_ITEM_PREFIXS}${__m_LIST_OPENCV_LIBRARY_DIR_ITEM}${LIST_OPENCV_LIBRARY_DIR_ITEM_SUBFIXS})
        set(_result __XSETPIC_${__m_LIST_OPENCV_LIBRARY_DIR_ITEM})
        find_library(${_result} NAMES ${_library} PATH_SUFFIXES ${LIST_OPENCV_LIBRARY_DIR_ITEM_SUBFIXS} HINT ENV)
        #message(STATUS "${_library}         =>   ${${_result}}")
        if(${_result}_NOTFOUND)
            message(FATAL_ERROR "Cmake failed to find ${_library}.Please check your opencv installation and ensure it works well.")
        else()
            list(APPEND ___xsetpic_library_list ${${_result}})
        endif(${_result}_NOTFOUND)
        unset(_library CACHE)
        unset(${_result} CACHE)
        unset(_result CACHE)
    endif(XSETPIC_WITH_OPENCV_${__m_LIST_OPENCV_LIBRARY_DIR_ITEM})
endforeach()
set(XSETPIC_PC_OPENCV_LIBRARY_ITEMS ${___xsetpic_library_list})
unset(___xsetpic_library_list)

#message(STATUS "INCLUDE:${XSETPIC_PC_OPENCV_INCLUDE_ITEMS}")
#message(STATUS "LIBRARY:${XSETPIC_PC_OPENCV_LIBRARY_ITEMS}")

#include(FindPackageHandleStandardArgs)

#find_package_handle_standard_args

#mark_as_advanced(XSETPIC_PC_OPENCV_LIBRARY_ITEMS XSETPIC_PC_OPENCV_INCLUDE_ITEMS )
