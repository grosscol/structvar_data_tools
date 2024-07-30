#include "app.hpp"
#include "boost/program_options.hpp"

namespace po = boost::program_options;
namespace bj = boost::json;

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

SimpleAlignment make_simple_alignment(AlignmentReader& reader){
  return SimpleAlignment(
      reader.get_query_name(),
      reader.get_chrom(),
      reader.get_start(),
      reader.get_end(),
      reader.is_forward_strand());
}

void add_alignment(bj::object& container, SimpleAlignment& sa,  AlnType aln_type){

  // reference to top level all_splits or all_pairs
  bj::object& aln_type_container = container[AlnTypeJsonKeyMap[aln_type]].as_object();

  if(!aln_type_container.contains(sa.qname)) {
    aln_type_container[sa.qname] = bj::array{};
  }
  aln_type_container[sa.qname].as_array().emplace_back(sa.to_json());
}

bj::object init_top_level_json(){
  bj::object obj {};

  for(auto const& aln_kv : AlnTypeJsonKeyMap){
    obj[ aln_kv.second ] = bj::object{};
  }
  return obj;
}

void print_counts(Accounting& counts, std::ostream& dest){
  dest
    <<std::endl
    << "cnt: " << counts.total
    <<" qc: " << counts.qc_fail
    <<" unmap: " << counts.unmapped
    <<" dup: " << counts.duplicate
    <<" mapq: " << counts.bad_mapq
    <<" paired: " << counts.paired
    <<" split: " << counts.split
    <<" split_sa: " << counts.split_sa
    <<std::endl;
}

bool run(const AppControlData& control){
  bj::object all_data = init_top_level_json();
  Accounting counts;

  try{
    AlignmentReader reader{control.input_path, control.ref_path};

    while(reader.next_alignment()){
      // Validity checking
      if(reader.is_qc_fail()){
        counts.qc_fail++;
        continue;
      }
      if(reader.is_unmapped()){
        counts.unmapped++;
        continue; }
      if(reader.is_duplicate()){
        counts.duplicate++;
        continue;
      }
      if(!reader.is_mapq_sufficent()){
        counts.bad_mapq++;
        continue;
      }
      // Process alignment into output category.

      SimpleAlignment sa = make_simple_alignment(reader);

      if(reader.meets_pair_criteria()){
        counts.paired++;
        add_alignment(all_data, sa, AlnType::PAIRED);
      }
      if(reader.meets_split_criteria()){
        counts.split++;
        std::string sa_tag = reader.get_sa_tag();
        counts.split_sa += reader.count_sa_tag();

        add_alignment(all_data, sa, AlnType::SPLIT);
      }



      counts.total++;
    }
  } catch(std::runtime_error& ex){
    std::cerr<<"Error creating CRAM reader: "<<ex.what()<<"\n";
    return false;
  }

  //debugging
  print_counts(counts, std::cerr);

  std::cout<<all_data<<std::endl;
  return true;
}
