#include <iostream>
#include "boost/program_options.hpp"

#include "app_control_data.hpp"
#include "app.hpp"

namespace po = boost::program_options;

int app_main(const int argc, const char* argv[]) {

  return 1;
}

void emit_version_text(){
  std::cout
    << PROGRAM_TITLE << "\n"
    << "Version: "
    << PROJECT_VERSION_MAJOR << "."
    << PROJECT_VERSION_MINOR << "."
    << PROJECT_VERSION_PATCH << "\n";
}
