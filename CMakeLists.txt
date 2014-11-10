#
# This file is a part of the Sharemind framework.
# Copyright (C) Cybernetica AS
#
# All rights are reserved. Reproduction in whole or part is prohibited
# without the written consent of the copyright owner. The usage of this
# code is subject to the appropriate license agreement.
#

CMAKE_MINIMUM_REQUIRED(VERSION "2.8.12")
PROJECT(SHAREMIND_LIBMODAPICXX "CXX")

SET(LIBMODAPICXX_VERSION "0.2.0.0")
SET(LIBMODAPICXX_COMPAT_VERSION "0.2.0.0")

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/config.local" OPTIONAL)
INCLUDE("${CMAKE_CURRENT_BINARY_DIR}/config.local" OPTIONAL)

FIND_PACKAGE(SharemindCxxHeaders REQUIRED)
FIND_PACKAGE(SharemindLibmodapi 0.2.0 REQUIRED)

SET(LIBMODAPICXX_INSTALL_DEFINITIONS ${SharemindCxxHeaders_DEFINITIONS}
                                     ${SharemindLibmodapi_DEFINITIONS})
LIST(REMOVE_DUPLICATES LIBMODAPICXX_INSTALL_DEFINITIONS)
SET(LIBMODAPICXX_INSTALL_LINK_LIBRARIES ${SharemindCxxHeaders_LIBRARIES}
                                        ${SharemindLibmodapi_LIBRARIES})
LIST(REMOVE_DUPLICATES LIBMODAPICXX_INSTALL_LINK_LIBRARIES)
SET(LIBMODAPICXX_INSTALL_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/include"
                                      ${SharemindCxxHeaders_INCLUDE_DIRS}
                                      ${SharemindLibmodapi_INCLUDE_DIRS})
LIST(REMOVE_DUPLICATES LIBMODAPICXX_INSTALL_INCLUDE_DIRS)

FILE(GLOB_RECURSE SHAREMIND_LIBMODAPICXX_HEADERS
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
ADD_CUSTOM_TARGET("include_libmodapicxx_headers_in_IDE"
                  SOURCES ${SHAREMIND_LIBMODAPICXX_HEADERS})
INSTALL(FILES ${SHAREMIND_LIBMODAPICXX_HEADERS}
        DESTINATION "include/sharemind/libmodapi")

# Install cmake files:
FILE(GLOB_RECURSE sharemind_libmodapicxx_in_files
    "${CMAKE_CURRENT_SOURCE_DIR}/*.in")
ADD_CUSTOM_TARGET("include_sharemind_libmodapicxx_in_files_in_IDE"
                  SOURCES ${sharemind_libmodapicxx_in_files})
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/SharemindLibmodapiCxxConfig.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibmodapiCxxConfig.cmake"
               @ONLY)
CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/SharemindLibmodapiCxxConfigVersion.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibmodapiCxxConfigVersion.cmake" @ONLY)
INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibmodapiCxxConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibmodapiCxxConfigVersion.cmake"
        DESTINATION "lib/SharemindLibmodapiCxx")
