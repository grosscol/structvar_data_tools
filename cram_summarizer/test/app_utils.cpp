#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string_view>
#include "app_utils.hpp"

TEST(AppUtil, ValidInteger){
  constexpr std::string_view input{"1234"};
  int num_val{0};
  bool ret_val{false};

  ret_val = view_to_numeric(input, num_val);

  EXPECT_TRUE(ret_val);
  EXPECT_EQ(num_val, 1234);
}

TEST(AppUtil, NonNumeric){
  constexpr std::string_view input{"abc"};
  int num_val{0};
  bool ret_val{true};

  ret_val = view_to_numeric(input, num_val);

  EXPECT_FALSE(ret_val);
  EXPECT_EQ(num_val, 0);
}

TEST(AppUtil, NumericThenNonNumeric){
  constexpr std::string_view input{"123abc"};
  int num_val{0};
  bool ret_val{true};

  ret_val = view_to_numeric(input, num_val);

  EXPECT_FALSE(ret_val);
  EXPECT_EQ(num_val, 123);
}

TEST(AppUtil, NonNumericThenNumeric){
  constexpr std::string_view input{"abc123"};
  int num_val{0};
  bool ret_val{true};

  ret_val = view_to_numeric(input, num_val);

  EXPECT_FALSE(ret_val);
  EXPECT_EQ(num_val, 0);
}
