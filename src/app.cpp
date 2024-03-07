#include <iostream>
#include <string>
#include <cstdlib>
#include <boost/program_options.hpp>
#include <htslib/vcf.h>
#include "variant_processor.hpp"
#include "bcf_reader.hpp"
#include "app_control_data.hpp"
#include "app.hpp"

namespace po = boost::program_options;

int app_main(const int argc, const char* argv[]) {

  bool has_run_succeeded{false};
  bool has_parse_succeeded{false};
  AppControlData app_ctl{};

  // Get the initial state of the app from the command line options
  has_parse_succeeded = parse_cli_args(argc, argv, app_ctl);

  // Handle exit early states
  if(has_parse_succeeded == false){
    std::cerr<<"Arg parsing error!";
    return EXIT_FAILURE;
  }

  if(app_ctl.print_version){
    emit_version_text();
  }

  if(app_ctl.just_exit){
    return EXIT_SUCCESS;
  }

  has_run_succeeded = run(app_ctl);

  if(has_run_succeeded){
    return EXIT_SUCCESS;
  } else {
    return EXIT_FAILURE;
  }
}

void emit_version_text(){
  std::cout<<"Version: "
    << PROJECT_VERSION_MAJOR << "."
    << PROJECT_VERSION_MINOR << "."
    << PROJECT_VERSION_PATCH << "\n";
}


bool parse_cli_args(const int argc, const char* argv[], AppControlData& controls){

  po::options_description desc{"Program options"};
  po::variables_map vm;
  std::string in_path;

  try {
    desc.add_options()
        ("help", "Print usage and exit.")
        ("version", "Print version and exit.")
        ("file", po::value(&in_path), "Path to input file.")
    ;

    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    // Handle here the event that requires desc.  Avoid needing desc outside of this scope.
    if (vm.count("help")) {
      std::cout << desc << "\n";
      controls.just_exit = true;
    }

    if (vm.count("version")) {
      controls.print_version = true;
      controls.just_exit = true;
    }

    if (vm.count("file")) {
      controls.input_path = vm["file"].as<std::string>();
    }

    return true;
  }
  catch(std::exception& e) {
      std::cerr << "error: " << e.what() << "\n";
      return false;
  }
  catch(...) {
      std::cerr << "Exception of unknown type!\n";
      return false;
  }
}

bool run(const AppControlData& control){
  try{
    BcfReader bcf{control.input_path};
    std::cout<<"N samples: "<<bcf.n_samples()<<std::endl;
    while(bcf.next_variant()){
      bcf.print_genotypes();
      std::cout<<std::endl;
    }
  } catch(std::runtime_error& ex){
    std::cerr<<"Error creating BCF reader: "<<ex.what()<<"\n";
    return false;
  }

  std::cout<<"Application Done"<<std::endl;
  return true;
}
