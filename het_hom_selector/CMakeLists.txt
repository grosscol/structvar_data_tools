cmake_minimum_required(VERSION 3.16)
project(
  HetHomSelector
  VERSION 0.4.0
  DESCRIPTION "Select representative samples for each variant of VCF/BCF input."
  LANGUAGES CXX)
set(PROGRAM_TITLE "Heterozygote and Homozygote Selector")

######################
# HH Selector Binary #
######################
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH bin)
set(CONFIGURED_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/configured_include)
set(CLI_NAME het_hom_sel)

# Directory for the cmake configured header files
file(MAKE_DIRECTORY ${CONFIGURED_INCLUDE_DIR})

# Add VERSION info into application header file.
configure_file(include/app.hpp.in ${CONFIGURED_INCLUDE_DIR}/app.hpp)

# Build static library of application logic
add_library(${CLI_NAME}_lib
  STATIC
    src/bcf_reader.cpp
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
find_program(BCFTOOLS bcftools)

if(BCFTOOLS)
  set(GENERATED_DATA_DIR ${CMAKE_BINARY_DIR}/Testing/data)
  file(MAKE_DIRECTORY ${GENERATED_DATA_DIR})

  set(VCF_TEST_FILE ${CMAKE_CURRENT_SOURCE_DIR}/test/data/structvar_sample_input.vcf)
  set(BCF_TEST_FILE ${GENERATED_DATA_DIR}/structvar_sample_input.bcf)
  if(NOT EXISTS ${BCF_TEST_FILE})
    message(STATUS "Generating testing BCF file.")
    execute_process(
      COMMAND ${BCFTOOLS} view --output-type b --output ${BCF_TEST_FILE} ${VCF_TEST_FILE})
  else()
    message(STATUS "Testing BCF file already exists.")
  endif()
else()
  message(STATUS "bcftools NOT found. BCF testing artifacts not built")
endif()

# Make full path to test data directory available in testing header file
set(SRC_TEST_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test/data)
configure_file(test/structvar_fixture.hpp.in ${CONFIGURED_INCLUDE_DIR}/structvar_fixture.hpp)

add_executable(test_het_hom_selector
  test/bcf_reader.cpp
  test/control_flow.cpp)

target_include_directories(test_het_hom_selector
  PUBLIC
    ${CONFIGURED_INCLUDE_DIR}
    ${HHSelector_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(test_het_hom_selector
  GTest::gtest_main
  GTest::gmock_main
  ${CLI_NAME}_lib)

enable_testing()
gtest_discover_tests(test_het_hom_selector)
