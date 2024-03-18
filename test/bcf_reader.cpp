#include <gtest/gtest.h>
#include <gmock/gmock.h>
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

TEST_F(StructVarTest, InitNumHetsAndHoms){
  BcfReader reader{test_data_path.string()};
  EXPECT_EQ(reader.n_hets(), 0);
  EXPECT_EQ(reader.n_homs(), 0);
}

TEST_F(StructVarTest, ExpectedNumHetsAndHoms){
  BcfReader reader{test_data_path.string()};

  reader.next_variant();
  EXPECT_EQ(reader.n_hets(), 2);
  EXPECT_EQ(reader.n_homs(), 1);

  reader.next_variant();
  EXPECT_EQ(reader.n_hets(), 0);
  EXPECT_EQ(reader.n_homs(), 0);

  reader.next_variant();
  EXPECT_EQ(reader.n_hets(), 5);
  EXPECT_EQ(reader.n_homs(), 2);
}

TEST_F(StructVarTest, ExpectedHetsAndHomsIndexes){
  BcfReader reader{test_data_path.string()};

  reader.next_variant();
  EXPECT_THAT(reader.het_idxs(), testing::ElementsAre(1,2));
  EXPECT_THAT(reader.hom_idxs(), testing::ElementsAre(0));

  reader.next_variant();
  EXPECT_TRUE(reader.het_idxs().empty());
  EXPECT_TRUE(reader.hom_idxs().empty());

  reader.next_variant();
  EXPECT_THAT(reader.het_idxs(), testing::ElementsAre(5,6,7,8,9));
  EXPECT_THAT(reader.hom_idxs(), testing::ElementsAre(3,4));
}

TEST_F(StructVarTest, ExpectedSampleNames){
  BcfReader reader{test_data_path.string()};

  EXPECT_EQ(reader.sample_idx_to_id(0), "EXAMPLE01");
  EXPECT_EQ(reader.sample_idx_to_id(4), "EXAMPLE05");
  EXPECT_EQ(reader.sample_idx_to_id(9), "EXAMPLE10");

  std::vector<int> input_ids{0,4,9};
  EXPECT_THAT(reader.sample_idxs_to_ids(input_ids),
              testing::ElementsAre("EXAMPLE01","EXAMPLE05","EXAMPLE10"));
}
