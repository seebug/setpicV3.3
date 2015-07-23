#--------------------------------------------------------------------------------------------------
#
# FILENAME  :xsetpic.detectboost.library.cmake
#
# PURPOSE   :DETECT BOOST LIBRARY FOR XSETPIC
#
# NOTE      :
#
#                           <perturbed@sina.com>
#--------------------------------------------------------------------------------------------------


include(cmake/xsetpic.utils.cmake)

message(STATUS "Detecting for module 'boost'")
find_package(Boost QUIET COMPONENTS date_time system filesystem program_options thread regex program_options REQUIRED)
if(Boost_FOUND)
    message(STATUS "Found Boost Library: ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION} (${Boost_VERSION})")
    if(${Boost_VERSION} LESS 104200)
        message(FATAL_ERROR "\nBoost Library Required Version At Least 1.42.0 
        \nCurrent Boost Version : ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}\n")
    endif(${Boost_VERSION} LESS 104200)
    #message(STATUS "${Boost_LIBRARIES}")
    include_directories(${Boost_INCLUDE_DIRS})
    #target_link_libraries(target ${Boost_LIBRARIES})   
endif()
