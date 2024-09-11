#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <filesystem>
#include <vector>
#include <iostream>
#include <fstream>
#include <tuple>
#include <string>
#include <string_view>
#include "alignment_fixture.hpp"
#include "cram_reader.hpp"

/******************************************************************************
 * Read expected counts and generate path strings from precomputed count data *
 ******************************************************************************/

std::vector<ExpectedCounts> parse_precomputed_counts(){
  std::vector<ExpectedCounts> expectations;

  std::filesystem::path test_data_dir{SRC_TEST_DATA_DIR};
  std::filesystem::path counts_filename{"counts.txt"};
  std::filesystem::path counts_path{test_data_dir / counts_filename};

  std::ifstream infile{ counts_path };
  if( infile ){
    // ignore header line
    infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    // each line contains id and expected counts
    while(!infile.eof()){
      ExpectedCounts exp{}; 
      infile >> exp.id >> exp.n_pair >> exp.n_pair_align >> exp.n_split >> 
        exp.n_split_align >> exp.n_all_align >> exp.n_records;
      expectations.push_back(exp);
    }
  }
  infile.close();

  // pop erroneous final entry from trailing line ending.
  expectations.pop_back();

  return expectations;
}

std::vector<TestParam> generate_path_parameters(){
  std::filesystem::path src_test_data_dir{SRC_TEST_DATA_DIR};
  std::filesystem::path gen_test_data_dir{GENERATED_DATA_DIR};

  std::vector<ExpectedCounts> expectations{parse_precomputed_counts()}; 
  std::vector<TestParam> path_parameters;

  std::filesystem::path src_test_data_filename;
  std::filesystem::path src_test_data_path;
  std::filesystem::path gen_test_data_filename;
  std::filesystem::path gen_test_data_path;

  for(auto& exp : expectations){
    src_test_data_filename = std::filesystem::path(exp.id + ".sam");
    gen_test_data_filename = std::filesystem::path(exp.id + ".cram");
    src_test_data_path = std::filesystem::path(src_test_data_dir / src_test_data_filename);
    gen_test_data_path = std::filesystem::path(gen_test_data_dir / gen_test_data_filename);

    // Test data commited to repo must be present
    if(std::filesystem::is_regular_file(src_test_data_path)){
      path_parameters.push_back(TestParam(src_test_data_path.string(), exp));
    }else{
      std::cerr<<"Source Test Data Missing: "<<src_test_data_path<<std::endl;
      exit(1);
    }

    if(std::filesystem::is_regular_file(gen_test_data_path)){
      path_parameters.push_back(TestParam(gen_test_data_path.string(), exp));
    }else{
      std::cerr<<"Generated Test Data Missing: "<<gen_test_data_path<<std::endl;
    }

  }

  return path_parameters;
}


/*******************************************************
 * Parameterized tests to verify SAM & CRAM inputs work *
 *******************************************************/
TEST_P(PathAndCountsFixture, ExpectedSampleNames){
  TestParam path_and_counts = GetParam();
  std::string infile_path{std::get<0>(path_and_counts)};
  ExpectedCounts expected{std::get<1>(path_and_counts)};

  AlignmentReader reader{infile_path, ""};
  uint32_t count{0};
  while(reader.next_alignment()){ count++; }

  EXPECT_EQ(count, expected.n_records);
}

INSTANTIATE_TEST_SUITE_P( SummarizeFiles, PathAndCountsFixture,
    testing::ValuesIn(generate_path_parameters()));


/******************
 * Static Methods *
 *****************/
TEST(AlignReader, RefSpanString){
  std::string cigar_all_matches{"151M"};
  int result_all_matches{AlignmentReader::reference_span(cigar_all_matches)};

  EXPECT_EQ(result_all_matches, 151);

}

TEST(AlignReader, RefSpanStringView){
  std::string_view cigar_all_matches{"151M"};
  int result_all_matches{AlignmentReader::reference_span(cigar_all_matches)};

  EXPECT_EQ(result_all_matches, 151);
}
