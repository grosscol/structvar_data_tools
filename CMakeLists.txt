cmake_minimum_required(VERSION 3.16)
project(
  HHSelector 
  VERSION 0.1.0
  DESCRIPTION "heterozygote and homozygote representative selector"
  LANGUAGES CXX)

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

#######################
# Get Boost libraries #
#######################
# adapted from https://stackoverflow.com/a/73933840
set(BOOST_INCLUDE_LIBRARIES thread filesystem system program_options)
set(BOOST_ENABLE_CMAKE ON)

include(FetchContent)
FetchContent_Declare(
  Boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_SHALLOW TRUE
  GIT_TAG boost-1.80.0
)
FetchContent_MakeAvailable(Boost)

#######################
# HH Selector Project #
#######################
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED False)
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")

add_executable(het_hom_sel src/main.cpp src/variant_processor.cpp)
set_target_properties(het_hom_sel PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(het_hom_sel PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(het_hom_sel PUBLIC "${CMAKE_SOURCE_DIR}/include")

target_link_libraries(het_hom_sel PRIVATE Boost::filesystem Boost::program_options)

# Add VERSION info into main header.
configure_file(include/main.hpp.in ./main.hpp)
