INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_TEKRSA TekRSA)

FIND_PATH(
    TEKRSA_INCLUDE_DIRS
    NAMES TekRSA/api.h
    HINTS $ENV{TEKRSA_DIR}/include
        ${PC_TEKRSA_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    TEKRSA_LIBRARIES
    NAMES gnuradio-TekRSA
    HINTS $ENV{TEKRSA_DIR}/lib
        ${PC_TEKRSA_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TEKRSA DEFAULT_MSG TEKRSA_LIBRARIES TEKRSA_INCLUDE_DIRS)
MARK_AS_ADVANCED(TEKRSA_LIBRARIES TEKRSA_INCLUDE_DIRS)

