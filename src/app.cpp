#include <iostream>
#include <string>
#include <vector>
#include <random>
#include "boost/program_options.hpp"
#include "boost/algorithm/string/join.hpp"
#include "htslib/vcf.h"
#include "bcf_reader.hpp"
#include "app_control_data.hpp"
#include "app.hpp"

namespace po = boost::program_options;
namespace alg = boost::algorithm;

int app_main(const int argc, const char* argv[]) {

  // Avoid flushing to stdout after every line
  std::ios::sync_with_stdio(false);

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

  po::options_description desc {"Program options"};
  po::variables_map vm {};

  try {
    desc.add_options()
        ("help,h", "Print usage and exit.")
        ("version,v", "Print version and exit.")
        ("file,f", po::value(&controls.input_path), "Path to input file.")
        ("num,n", po::value(&controls.num_rnd_samples), "Number of samples to take.")
        ("seed,s", po::value(&controls.rnd_seed), "Seed for PRNG.")
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

std::vector<std::string> random_samples(const BcfReader& bcf, std::mt19937& gen, const std::vector<int>& idxs, const int n){

  std::vector<int> rnd_idxs{};
  std::vector<std::string> samples{};

  rnd_idxs.reserve(n);
  samples.reserve(n);

  std::sample(idxs.begin(), idxs.end(), std::back_inserter(rnd_idxs), n, gen);

  samples = bcf.sample_idxs_to_ids(rnd_idxs);

  return samples;
}

std::vector<std::string> random_hets(const BcfReader& bcf, std::mt19937& gen, const int n){
  return random_samples(bcf, gen, bcf.het_idxs(), n);
}

std::vector<std::string> random_homs(const BcfReader& bcf, std::mt19937& gen, const int n){
  return random_samples(bcf, gen, bcf.hom_idxs(), n);
}

void emit_header(const int seed, const int n_sample){
  std::cout<<"#RANDOM_SEED="<<std::to_string(seed)<<"\n"
    <<"#MAX_RANDOM_HOM_HETS=<<"<<std::to_string(n_sample)<<"\n"
    <<"#SAMPLES_USED=NA"<<"\n"
    <<"#CHROM\tPOS\tREF\tALT\tHOM\tHET\n";
}

void emit_selection(const BcfReader& bcf, const std::vector<std::string>& hets,
                    const std::vector<std::string>& homs){
  std::cout<<bcf.chr()<<'\t'
    <<std::to_string(bcf.pos())<<'\t'
    <<bcf.ref()<<'\t'
    <<bcf.alt()<<'\t'
    <<alg::join(homs, ",")<<'\t'
    <<alg::join(hets, ",")
    <<'\n';
}

/**
 * Top level logic for reading, processing, and output.
 */
bool run(const AppControlData& control){
  try{
    BcfReader bcf{control.input_path};

    emit_header(control.rnd_seed, control.num_rnd_samples);

    // Vars for sampling
    std::mt19937 rnd_gen{control.rnd_seed};
    std::vector<std::string> rnd_het_ids{};
    std::vector<std::string> rnd_hom_ids{};

    while(bcf.next_variant()){

      rnd_het_ids = random_hets(bcf, rnd_gen, control.num_rnd_samples);
      rnd_hom_ids = random_homs(bcf, rnd_gen, control.num_rnd_samples);

      emit_selection(bcf, rnd_het_ids, rnd_hom_ids);

      rnd_het_ids.clear();
      rnd_hom_ids.clear();
    }
  } catch(std::runtime_error& ex){
    std::cerr<<"Error creating BCF reader: "<<ex.what()<<"\n";
    return false;
  }

  return true;
}
