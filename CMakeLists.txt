cmake_minimum_required(VERSION 3.16)
project(
  HHSelector 
  VERSION 0.1.0
  DESCRIPTION "heterozygote and homozygote representative selector"
  LANGUAGES CXX)

include(CMakePrintHelpers)

# Requirements for HTSLib
include(ExternalProject)
include(FetchContent)
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
# adapted from https://stackoverflow.com/a/73933840
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
    # when using the makefile generator, use the special variable $(MAKE) to invoke make
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
  LOG_DOWNLOAD 1
)

set(htslib_LIB ${htslib_INSTALL}/lib/libhts.a)

######################
# HH Selector Binary #
######################
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(CONFIGURED_INCLUDE_PATH ${PROJECT_BINARY_DIR}/configured_include)
set(CLI_NAME het_hom_sel)

# Directory for the cmake configured header files
file(MAKE_DIRECTORY ${CONFIGURED_INCLUDE_PATH})

# Add VERSION info into application header.
configure_file(include/app.hpp.in ${CONFIGURED_INCLUDE_PATH}/app.hpp)

# Build static library of application logi
add_library(${CLI_NAME}_lib
  STATIC
    src/variant_processor.cpp
    src/bcf_sampler.cpp
    src/app.cpp)

target_include_directories(${CLI_NAME}_lib 
  PUBLIC 
    ${CONFIGURED_INCLUDE_PATH}
    ${CMAKE_SOURCE_DIR}/include
  PRIVATE 
    ${htslib_INSTALL}/include)

target_link_libraries(${CLI_NAME}_lib 
  PRIVATE 
    ${htslib_LIB}
    Boost::filesystem Boost::program_options
    ZLIB::ZLIB
    BZip2::BZip2
    LibLZMA::LibLZMA
    CURL::libcurl
    OpenSSL::Crypto)

# Build executable wrapper around application lib
add_executable(${CLI_NAME} 
  src/main.cpp)

target_include_directories(${CLI_NAME} 
  PUBLIC 
    ${CONFIGURED_INCLUDE_PATH}
    ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(${CLI_NAME} PRIVATE ${CLI_NAME}_lib)


#########
# Tests #
#########
enable_testing()
add_executable(
  test_control
  test/control_flow.cpp)

target_include_directories(test_control 
  PUBLIC 
    ${PROJECT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(
  test_control
  GTest::gtest_main
  ${CLI_NAME}_lib)

include(GoogleTest)
gtest_discover_tests(test_control)
