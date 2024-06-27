#include <iostream>
#include "boost/program_options.hpp"

#include "app_control_data.hpp"
#include "app.hpp"
#include "cram_reader.hpp"

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
  std::cout
    << PROGRAM_TITLE << "\n"
    << "Version: "
    << PROJECT_VERSION_MAJOR << "."
    << PROJECT_VERSION_MINOR << "."
    << PROJECT_VERSION_PATCH << "\n";
}

bool parse_cli_args(const int argc, const char* argv[], AppControlData& controls){

  // Break up options to hide flags for positional args
  po::options_description desc{"OPTIONS"};
  po::options_description hidden{"Hidden positional options"};
  po::options_description full_opts{"All options"};
  po::positional_options_description pos_opts{};
  po::variables_map vm {};

  desc.add_options()
      ("help,h", "Print usage and exit.")
      ("version,v", "Print version and exit.")
      ("ref,r", po::value(&controls.ref_path),"Path to reference fasta for crams.")
  ;

  hidden.add_options()
      ("file", po::value(&controls.input_path), "Path to input file.")
  ;

  pos_opts.add("file", 1);

  full_opts.add(desc);
  full_opts.add(hidden);

  po::command_line_parser clp{argc, argv};
  clp.options(full_opts)
     .positional(pos_opts);

  try {
    po::store(clp.run(), vm);

    if (vm.count("help")) {
      emit_version_text();
      std::cout
        << "Usage:" << "\n"
        << "  " << PROGRAM_NAME << "[OPTIONS] [FILE]" << "\n"
        << desc << "\n";

      controls.just_exit = true;
    }

    if(vm.count("version")) {
      controls.print_version = true;
      controls.just_exit = true;
    }

    po::notify(vm);

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
    CramReader cram{control.input_path, control.ref_path};

    int count{0};
    int c_qc_fail{0};
    int c_unmapped{0};
    int c_duplicate{0};
    int c_bad_mapq{0};

    while(cram.next_alignment()){
      if(cram.is_qc_fail()){c_qc_fail++;}
      if(cram.is_unmapped()){c_unmapped++;}
      if(cram.is_duplicate()){c_duplicate++;}
      if(!cram.is_mapq_sufficent()){c_bad_mapq++;}
      count++;

      std::cout<<cram.get_cigar_string()<<",";

    }
    std::cout
      <<std::endl
      << "cnt: " << count
      <<" qc: " << c_qc_fail
      <<" unmap: " << c_unmapped
      <<" dup: " << c_duplicate
      <<" mapq: " << c_bad_mapq
      <<std::endl;

  } catch(std::runtime_error& ex){
    std::cerr<<"Error creating CRAM reader: "<<ex.what()<<"\n";
    return false;
  }

  return true;
}
