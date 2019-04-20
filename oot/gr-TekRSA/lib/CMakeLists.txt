# Copyright 2011,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

include_directories(${Boost_INCLUDE_DIR} /opt/include)
link_directories(${Boost_LIBRARY_DIRS})

list(APPEND TekRSA_sources
    iq_blk_cmplx_impl.cc
    iq_stream_impl.cc
)

set(TekRSA_sources "${TekRSA_sources}" PARENT_SCOPE)
if(NOT TekRSA_sources)
	MESSAGE(STATUS "No C++ sources... skipping lib/")
	return()
endif(NOT TekRSA_sources)

find_library(ll ippch)
list(APPEND RSA_API_Libs ${ll})
unset(ll CACHE)
find_library(ll ippcv)
list(APPEND RSA_API_Libs ${ll})
unset(ll CACHE)
find_library(ll ippdc)
list(APPEND RSA_API_Libs ${ll})
unset(ll CACHE)
find_library(ll ippi)
list(APPEND RSA_API_Libs ${ll})
unset(ll CACHE)
find_library(ll ipps)
list(APPEND RSA_API_Libs ${ll})
unset(ll CACHE)
find_library(ll ippvm)
list(APPEND RSA_API_Libs ${ll})
unset(ll CACHE)
find_library(ll ippcore)
list(APPEND RSA_API_Libs ${ll})
unset(ll CACHE)
find_library(ll ippcc)
list(APPEND RSA_API_Libs ${ll})
unset(ll CACHE)

find_library(ll UnifiedAPI)
list(APPEND RSA_API_Libs ${ll})
unset(ll CACHE)
find_library(ll RSA300API)
list(APPEND RSA_API_Libs ${ll})
unset(ll CACHE)
find_library(ll RSA500API)
list(APPEND RSA_API_Libs ${ll})
unset(ll CACHE)
find_library(ll GPMeasDSP)
list(APPEND RSA_API_Libs ${ll})
unset(ll CACHE)
find_library(ll BaseDSPL)
list(APPEND RSA_API_Libs ${ll})
unset(ll CACHE)

find_library(ll dl)
list(APPEND RSA_API_Libs ${ll})
unset(ll CACHE)
find_library(ll icui18n)
message("icui18n is at: ${ll}")
list(APPEND RSA_API_Libs ${ll})
unset(ll CACHE)
find_library(ll icuuc)
list(APPEND RSA_API_Libs ${ll})
unset(ll CACHE)
find_library(ll icudata)
list(APPEND RSA_API_Libs ${ll})
unset(ll CACHE)
find_library(ll boost_date_time)
list(APPEND RSA_API_Libs ${ll})
unset(ll CACHE)
find_library(ll boost_chrono)
list(APPEND RSA_API_Libs ${ll})
unset(ll CACHE)
find_library(ll boost_system)
list(APPEND RSA_API_Libs ${ll})
unset(ll CACHE)
find_library(ll boost_filesystem)
list(APPEND RSA_API_Libs ${ll})
unset(ll CACHE)
find_library(ll rt)
list(APPEND RSA_API_Libs ${ll})
unset(ll CACHE)
find_library(ll boost_thread)
list(APPEND RSA_API_Libs ${ll})
unset(ll CACHE)
find_library(ll boost_regex)
list(APPEND RSA_API_Libs ${ll})
unset(ll CACHE)
find_library(ll pthread)
list(APPEND RSA_API_Libs ${ll})
unset(ll CACHE)
find_library(ll udev)
list(APPEND RSA_API_Libs ${ll})
unset(ll CACHE)
find_library(ll usb-1.0)
list(APPEND RSA_API_Libs ${ll})
unset(ll CACHE)
find_library(ll boost_log)
list(APPEND RSA_API_Libs ${ll})
unset(ll CACHE)
#set (RSA_API_Libs ippch.a ippcv.a ippdc.a ippi.a ipps.a ippvm.a ippcore.a ippcc.a )
message("RSA_API_Libs are: ${RSA_API_Libs}")
#message("gnu all libs: ${GNURADIO_ALL_LIBRARIES}")
#link_directories(/opt/lib)
#message("include directories are: ${LINK_DIRECTORIES}")
#set(ENV LD_LIBRARY_PATH /opt/lib)
 
add_library(gnuradio-TekRSA SHARED ${TekRSA_sources})
target_link_libraries(gnuradio-TekRSA ${Boost_LIBRARIES} ${GNURADIO_ALL_LIBRARIES} ${RSA_API_Libs})
set_target_properties(gnuradio-TekRSA PROPERTIES DEFINE_SYMBOL "gnuradio_TekRSA_EXPORTS")

if(APPLE)
    set_target_properties(gnuradio-TekRSA PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif(APPLE)

########################################################################
# Install built library files
########################################################################
install(TARGETS gnuradio-TekRSA
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)

########################################################################
# Build and register unit test
########################################################################
include(GrTest)

include_directories(${CPPUNIT_INCLUDE_DIRS})

list(APPEND test_TekRSA_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test_TekRSA.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_TekRSA.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_iq_blk_cmplx.cc
)

add_executable(test-TekRSA ${test_TekRSA_sources})

target_link_libraries(
  test-TekRSA
  ${GNURADIO_RUNTIME_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  gnuradio-TekRSA
)

GR_ADD_TEST(test_TekRSA test-TekRSA)
