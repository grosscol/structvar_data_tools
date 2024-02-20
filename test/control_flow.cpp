#include <gtest/gtest.h>
#include <cstdlib>
#include <iostream>
#include "app_control_data.hpp"
#include "app.hpp"

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

	/* Capture cout */
	std::stringstream buffer;
	std::streambuf *old_buff = std::cout.rdbuf();
	std::cout.rdbuf(buffer.rdbuf());

  retval = app_main(argc, argv);

	/* Restore cout */
	std::cout.rdbuf(old_buff);

  EXPECT_EQ(retval, EXIT_SUCCESS);
};
