cmake_minimum_required(VERSION 3.16)
project(
  CramSummarizer
  VERSION 0.1.0
  DESCRIPTION "Summarize read information from CRAMs to power subsequent visualization."
  LANGUAGES CXX)
set(PROGRAM_TITLE "Cram Summarizer")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH bin)

set(CONFIGURED_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/configured_include)
set(CLI_NAME cram_summ)

# Directory for the cmake configured header files
file(MAKE_DIRECTORY ${CONFIGURED_INCLUDE_DIR})

# Add VERSION info into application header file.
configure_file(include/app.hpp.in ${CONFIGURED_INCLUDE_DIR}/app.hpp)

# Build static library of application logic
add_library(${CLI_NAME}_lib
  STATIC
    src/cram_reader.cpp
    src/simple_alignment.cpp
    src/app.cpp)

add_dependencies(${CLI_NAME}_lib htslib)

target_include_directories(${CLI_NAME}_lib
  PUBLIC
    ${CONFIGURED_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE
    ${htslib_INSTALL}/include)

target_link_libraries(${CLI_NAME}_lib
  PUBLIC
    Boost::json
  PRIVATE
    ${htslib_LIB}
    Boost::filesystem
    Boost::program_options
    ZLIB::ZLIB
    BZip2::BZip2
    LibLZMA::LibLZMA
    CURL::libcurl
    OpenSSL::Crypto
    Threads::Threads)

# Build executable wrapper around application lib
add_executable(${CLI_NAME}
  src/main.cpp)

target_include_directories(${CLI_NAME}
  PUBLIC
  ${CONFIGURED_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${CLI_NAME}
  PRIVATE
    ${CLI_NAME}_lib
  )

#########
# Tests #
#########
message(STATUS "CRAM Test Data Generation")

# If available, use samtools to create cram artifacts to test against.
find_program(SAMTOOLS samtools)

set(GENERATED_DATA_DIR ${CMAKE_BINARY_DIR}/Testing/data)
file(MAKE_DIRECTORY ${GENERATED_DATA_DIR})

# Enumerate fasta files
# Fasta file basenames are proxies for the structural variant ids.
# Test examples are keyed from the fasta file basenames e.g. dup_3
file(GLOB FASTA_PATHS "test/data/*.fa")

if(SAMTOOLS)
  foreach(FASTA IN LISTS FASTA_PATHS)
    cmake_path(GET FASTA FILENAME FA_FILENAME)
    cmake_path(REMOVE_EXTENSION FA_FILENAME OUTPUT_VARIABLE BASENAME)

    set(FAIDX ${GENERATED_DATA_DIR}/${FA_FILENAME}.fai)

    # Copy Fasta into Generated Data & generate fasta index
    file(COPY ${FASTA} DESTINATION ${GENERATED_DATA_DIR})
    if(NOT EXISTS ${FAIDX})
      message(STATUS "Generating fai for: " ${FA_FILENAME})
      execute_process(COMMAND ${SAMTOOLS} faidx ${GENERATED_DATA_DIR}/${FA_FILENAME})
    endif()

    # Enumerate SAM files corresponding to fasta basename
    file(GLOB SAM_PATHS "test/data/${BASENAME}*.sam")

    # Generate CRAMs using SAM and FASTA
    # Copy corresponding FASTA to avoid parsing more than file extension in tests.
    foreach(SAM IN LISTS SAM_PATHS)
      cmake_path(GET SAM FILENAME SAMPLE_NAME)
      cmake_path(REMOVE_EXTENSION SAMPLE_NAME)
      set(CRAM_OUTFILE ${GENERATED_DATA_DIR}/${SAMPLE_NAME}.cram)
      set(CRAM_INDEX ${GENERATED_DATA_DIR}/${SAMPLE_NAME}.cram.crai)
      set(CRAM_FASTA ${GENERATED_DATA_DIR}/${SAMPLE_NAME}.fa)
      set(CRAM_FAIDX ${GENERATED_DATA_DIR}/${SAMPLE_NAME}.fa.fai)

      if(NOT EXISTS ${CRAM_OUTFILE})
        message(STATUS "Generating CRAM for: " ${SAMPLE_NAME})
        execute_process(
          COMMAND ${SAMTOOLS} view --cram
            --reference ${GENERATED_DATA_DIR}/${FA_FILENAME}
            --output ${GENERATED_DATA_DIR}/${SAMPLE_NAME}.cram
            ${SAM})
      else()
        message(STATUS "CRAM exists for: " ${SAMPLE_NAME})
      endif()

      if(NOT EXISTS ${CRAM_INDEX})
        message(STATUS "Generating index for: " ${CRAM_OUTFILE})
        execute_process(
          COMMAND ${SAMTOOLS} index ${CRAM_OUTFILE})
      else()
        message(STATUS "index exists for: " ${CRAM_OUTFILE})
      endif()

    endforeach(SAM)
  endforeach(FASTA)

else()
  message(STATUS "samtools NOT found. CRAM testing artifacts not built.")
endif()

# Make full path to source and built test data directory available in testing header file
set(SRC_TEST_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test/data)
set(SRC_TEST_COUNTS_FILE ${SRC_TEST_DATA_DIR}/counts.txt)
set(BLT_TEST_DATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/Testing/data)

configure_file(test/alignment_fixture.hpp.in ${CONFIGURED_INCLUDE_DIR}/alignment_fixture.hpp)

add_executable(test_cram_summarizer
  test/app_utils.cpp
  test/simple_alignment.cpp
  test/alignment_reader.cpp
  test/summarizer.cpp)

target_include_directories(test_cram_summarizer
  PUBLIC
    ${CONFIGURED_INCLUDE_DIR}
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(test_cram_summarizer
  GTest::gtest_main
  GTest::gmock_main
  ${CLI_NAME}_lib)

enable_testing()
gtest_discover_tests(test_cram_summarizer)
