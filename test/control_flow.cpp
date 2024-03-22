#include <gtest/gtest.h>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include "app_control_data.hpp"
#include "app.hpp"
#include "shared_test_fixtures.hpp"

TEST(ControlFlow, VersionReturn) {
  const char* argv[]{"testing_app", "--version"};
  const int argc{2};
  int retval;

	/* Capture cout to prevent mucking up testing output
	   Store old buffer ptr, redirect to new buffer */
	std::stringstream buffer;
	std::streambuf *old_buff = std::cout.rdbuf();
	std::cout.rdbuf(buffer.rdbuf());

  retval = app_main(argc, argv);

	/* Redirect back to old buffer */
	std::cout.rdbuf(old_buff);

  EXPECT_EQ(retval, EXIT_SUCCESS);
};

TEST(ControlFlow, HelpReturn) {
  const char* argv[]{"testing_app", "--help"};
  const int argc{2};
  int retval;

  retval = app_main(argc, argv);

  EXPECT_EQ(retval, EXIT_SUCCESS);
};

TEST(ControlFlow, DataOpen){
  std::filesystem::path test_data_dir{TEST_DATA_DIR};
  std::filesystem::path test_data_file{"structvar_sample_input.vcf"};
  std::filesystem::path test_data_path{test_data_dir / test_data_file};

  const char* argv[]{"testing_app", "--file", test_data_path.c_str()};
  const int argc{3};
  int retval;

  retval = app_main(argc, argv);

  EXPECT_EQ(retval, EXIT_SUCCESS);
};

TEST(OptionParsing, DefaultInfile){
  const char* argv[]{"testing_app"};
  const int argc{1};
  bool parse_success{false};

  AppControlData app_ctl{};
  parse_success = parse_cli_args(argc, argv, app_ctl);

  EXPECT_TRUE(parse_success);
  EXPECT_EQ(app_ctl.input_path, "-");
}

TEST(OptionParsing, ActionRequired){
  const char* argv[]{"testing_app"};
  const int argc{1};
  bool parse_success{false};

  AppControlData app_ctl{};
  parse_success = parse_cli_args(argc, argv, app_ctl);

  EXPECT_TRUE(parse_success);
  EXPECT_EQ(app_ctl.action, "rnd");
}
