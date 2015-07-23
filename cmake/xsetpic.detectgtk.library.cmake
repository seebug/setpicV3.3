#--------------------------------------------------------------------------------------------------
#
# FILENAME  :xsetpic.detectgtk.library.cmake
#
# PURPOSE   :DETECT BOOST LIBRARY FOR XSETPIC
#
# NOTE      :
#
#                           <perturbed@sina.com>
#--------------------------------------------------------------------------------------------------


include(cmake/xsetpic.utils.cmake)

find_package(GTK2 2.8 QUIET COMPONENTS gtk)

if(NOT GTK2_FOUND)
    message(STATUS "Gtk Not Found,Compile \"test_setpic\" OFF")
else()
    message(STATUS "Found Gtk,version ${GTK2_VERSION}")
endif(NOT GTK2_FOUND)

