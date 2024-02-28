#include <gtest/gtest.h>
#include <filesystem>
#include "structvar_fixture.hpp"
#include "bcf_reader.hpp"

TEST_F(StructVarTest, HeaderVersion){
  BcfReader reader{test_data_path.string()};
  std::string version{reader.vcf_version()};

  EXPECT_EQ(version, "VCFv4.1") << "Version:" << version;
};

TEST_F(StructVarTest, ReadThroughFile){
  BcfReader reader{test_data_path.string()};
  int itt_count{0};

  while(reader.next_variant()){
    itt_count++;
  }

  // 23 variants in the sample data 
  EXPECT_EQ(itt_count, 23);
}

TEST_F(StructVarTest, NumberOfSamples){
  BcfReader reader{test_data_path.string()};

  // 10 samples in the sample data
  EXPECT_EQ(reader.n_samples(), 10);
}

TEST_F(StructVarTest, PrintVariantIds){
  BcfReader reader{test_data_path.string()};
  std::string var_id{};

  // Verify first variant id
  reader.next_variant();
  var_id = reader.variant_id();
  EXPECT_EQ(var_id, "INV_1:2972403-10562308");

  // Verify last variant id
  while(reader.next_variant());
  var_id = reader.variant_id();
  EXPECT_EQ(var_id, "DEL_1:10309401-10318600");
}
