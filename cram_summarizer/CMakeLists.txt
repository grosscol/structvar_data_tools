cmake_minimum_required(VERSION 3.16)
project(
  CramSummarizer
  VERSION 0.1.0
  DESCRIPTION "Summarize read information from CRAMs to power subsequent visualization."
  LANGUAGES CXX)
set(PROGRAM_TITLE "Cram Summarizer")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH bin)
set(CONFIGURED_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/configured_include)
set(CLI_NAME read_summ)

# Directory for the cmake configured header files
file(MAKE_DIRECTORY ${CONFIGURED_INCLUDE_DIR})

# Add VERSION info into application header file.
configure_file(include/app.hpp.in ${CONFIGURED_INCLUDE_DIR}/app.hpp)

# Build static library of application logic
add_library(${CLI_NAME}_lib
  STATIC
    src/cram_reader.cpp
    src/app.cpp)

add_dependencies(${CLI_NAME}_lib htslib)

target_include_directories(${CLI_NAME}_lib
  PUBLIC
    ${CONFIGURED_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE
    ${htslib_INSTALL}/include)

target_link_libraries(${CLI_NAME}_lib
  PRIVATE
    ${htslib_LIB}
    Boost::filesystem
    Boost::program_options
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
  ${CONFIGURED_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${CLI_NAME} PRIVATE ${CLI_NAME}_lib)

#########
# Tests #
#########

# If available, use bcftools to create bcf artifacts to test against.
find_program(SAMTOOLS samtools)

if(SAMTOOLS)
  set(GENERATED_DATA_DIR ${CMAKE_BINARY_DIR}/Testing/data)
  message(STATUS "Generating CRAMS placeholder")
else()
  message(STATUS "samtools NOT found. CRAM testing artifacts not built.")
endif()

# Make full path to test data directory available in testing header file
set(SRC_TEST_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test/data)
