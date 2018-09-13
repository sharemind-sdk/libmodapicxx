#
# Copyright (C) 2015 Cybernetica
#
# Research/Commercial License Usage
# Licensees holding a valid Research License or Commercial License
# for the Software may use this file according to the written
# agreement between you and Cybernetica.
#
# GNU General Public License Usage
# Alternatively, this file may be used under the terms of the GNU
# General Public License version 3.0 as published by the Free Software
# Foundation and appearing in the file LICENSE.GPL included in the
# packaging of this file.  Please review the following information to
# ensure the GNU General Public License version 3.0 requirements will be
# met: http://www.gnu.org/copyleft/gpl-3.0.html.
#
# For further information, please contact us at sharemind@cyber.ee.
#

CMAKE_MINIMUM_REQUIRED(VERSION "3.0")
PROJECT(SharemindLibModapiCxx VERSION "0.3.4" LANGUAGES "CXX")

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/config.local" OPTIONAL)
INCLUDE("${CMAKE_CURRENT_BINARY_DIR}/config.local" OPTIONAL)

FIND_PACKAGE(SharemindCMakeHelpers 1.3 REQUIRED)


FIND_PACKAGE(SharemindCxxHeaders 0.2.0 REQUIRED)
FIND_PACKAGE(SharemindLibModapi 0.3.0 REQUIRED)

FILE(GLOB_RECURSE SharemindLibModapiCxx_HEADERS
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
ADD_CUSTOM_TARGET("include_libmodapicxx_headers_in_IDE"
                  SOURCES ${SharemindLibModapiCxx_HEADERS})
INSTALL(FILES ${SharemindLibModapiCxx_HEADERS}
        DESTINATION "include/sharemind/libmodapi"
        COMPONENT "dev")

# Install cmake files:
SharemindCreateCMakeFindFiles(
    DEFINITIONS
        ${SharemindCxxHeaders_DEFINITIONS}
        ${SharemindLibModapi_DEFINITIONS}
    LIBRARIES
        ${SharemindCxxHeaders_LIBRARIES}
        ${SharemindLibModapi_LIBRARIES}
    INCLUDE_DIRS
        "${CMAKE_INSTALL_PREFIX}/include"
        ${SharemindCxxHeaders_INCLUDE_DIRS}
        ${SharemindLibModapi_INCLUDE_DIRS}
)


# Packaging:
SharemindSetupPackaging()
SharemindAddComponentPackage("dev"
    NAME "libsharemind-modapi-cxx-dev"
    DESCRIPTION "Sharemind Module API C++ wrapper development headers"
    DEB_SECTION "libdevel"
    DEB_DEPENDS
        "libsharemind-modapi-dev (>= 0.3.0)"
        "libsharemind-cxxheaders-dev (>= 0.2.0)"
        "libstdc++-dev"
)
SharemindPackagingFinalize()
