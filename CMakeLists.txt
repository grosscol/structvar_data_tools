cmake_minimum_required(VERSION 3.16)
project(
  StructVarTooling
  DESCRIPTION "Tooling for extracting data about structural variants."
  LANGUAGES CXX)

# Module for getting external content
include(FetchContent)

# Useful for debuggging CMake values
include(CMakePrintHelpers)

# Testing module
#   Include CTest prior to any add_subdirectory.
include(CTest)
include(GoogleTest)

# Requirements for HTSLib
include(ExternalProject)
include(FindZLIB)
include(FindBZip2)
include(FindLibLZMA)
include(FindCURL)
include(FindOpenSSL)


##########################
# Avoid in-source builds #
##########################
# adapted from https://stackoverflow.com/a/29678916
if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
  message(FATAL_ERROR
    "In-source builds not allowed.\n"
    "Please run from CMake from the build directory, or use -B flag."
    "After removing CMakeCache.txt (rm CMakeCache.txt)\n"
    " cd ./build\n"
    " cmake --build ..\n"
    "### OR ###\n"
    " cmake -S . -B ./build\n"
    " cmake --build ./build")

endif()

#################################
# Get Boost and GTest libraries #
#################################
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()
set(BOOST_INCLUDE_LIBRARIES thread filesystem system program_options)
set(BOOST_ENABLE_CMAKE ON)

FetchContent_Declare(
  boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_SHALLOW TRUE
  GIT_TAG boost-1.80.0
)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

FetchContent_MakeAvailable(boost googletest)

##############
# Get HTSlib #
##############
set(htslib_PREFIX ${CMAKE_BINARY_DIR}/contrib/htslib-prefix)
set(htslib_INSTALL ${CMAKE_BINARY_DIR}/contrib/htslib-install)
if (CMAKE_GENERATOR STREQUAL "Unix Makefiles")
    # When using the makefile generator, use the special variable $(MAKE) to invoke make
    #   This handles recursive invokation of make within a makefile
    set(MAKE_COMMAND "$(MAKE)")
else()
    # invoke make explicitly
    find_program(MAKE_COMMAND NAMES make gmake)
endif()

ExternalProject_Add(
  htslib
  PREFIX ${htslib_PREFIX}
  URL https://github.com/samtools/htslib/releases/download/1.19.1/htslib-1.19.1.tar.bz2
  UPDATE_COMMAND ""
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND autoreconf -i && ./configure --without-libdeflate --prefix=${htslib_PREFIX}
  BUILD_COMMAND ${MAKE_COMMAND} lib-static
  INSTALL_COMMAND ${MAKE_COMMAND} install prefix=${htslib_INSTALL}
  LOG_DOWNLOAD 0
  LOG_BUILD 0
)

set(htslib_LIB ${htslib_INSTALL}/lib/libhts.a)

################################
# BRAVO Data Tools Subprojects #
################################

# Het Hom Selector
add_subdirectory(het_hom_selector)

# Cram Summarizer
add_subdirectory(cram_summarizer)

##########################################
#            DEBUG CMake                 #
##########################################
# Get all propreties that cmake supports
if(NOT CMAKE_PROPERTY_LIST)
    execute_process(COMMAND cmake --help-property-list OUTPUT_VARIABLE CMAKE_PROPERTY_LIST)

    # Convert command output into a CMake list
    string(REGEX REPLACE ";" "\\\\;" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
    string(REGEX REPLACE "\n" ";" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
    list(REMOVE_DUPLICATES CMAKE_PROPERTY_LIST)
endif()

function(print_properties)
    message("CMAKE_PROPERTY_LIST = ${CMAKE_PROPERTY_LIST}")
endfunction()

# Print all the properties of a target.
# https://stackoverflow.com/a/34292622
function(print_target_properties target)
    if(NOT TARGET ${target})
      message(STATUS "There is no target named '${target}'")
      return()
    endif()

    foreach(property ${CMAKE_PROPERTY_LIST})
        string(REPLACE "<CONFIG>" "${CMAKE_BUILD_TYPE}" property ${property})

        if(property STREQUAL "LOCATION" OR property MATCHES "^LOCATION_" OR property MATCHES "_LOCATION$")
            continue()
        endif()

        get_property(was_set TARGET ${target} PROPERTY ${property} SET)
        if(was_set)
            get_target_property(value ${target} ${property})
            message("${target} ${property} = ${value}")
        endif()
    endforeach()
endfunction()
