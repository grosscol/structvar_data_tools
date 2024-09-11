#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <sstream>
#include "simple_alignment.hpp"

TEST(SimpleAlignment, StreamOut){
  std::stringstream s_out(std::ios_base::out);
  SimpleAlignment sa{"QUERY:NAME", "chrZ", 100, 200, true};

  s_out << sa;

  EXPECT_EQ(s_out.str(), "chrZ_100_200_1");
}

TEST(SimpleAlignment, JsonOutput){
  SimpleAlignment sa{"QUERY:NAME", "chrZ", 100, 200, true};
  boost::json::object obj {sa.to_json()};

  EXPECT_EQ(obj["chr"],   "chrZ");
  EXPECT_EQ(obj["start"], 100);
  EXPECT_EQ(obj["end"],   200);
  EXPECT_EQ(obj["is_reverse"],  false);
}
