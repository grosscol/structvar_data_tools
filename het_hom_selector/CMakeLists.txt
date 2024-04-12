cmake_minimum_required(VERSION 3.16)
project(
  HetHomSelector
  VERSION 0.3.0
  DESCRIPTION "Select representative samples for each variant of VCF/BCF input."
  LANGUAGES CXX)
set(PROGRAM_TITLE "Heterozygote and Homozygote Selector")

message(STATUS "SUBPROJECT: ")
