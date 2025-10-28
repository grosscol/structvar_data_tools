#include "app.hpp"
#include "boost/program_options.hpp"
#include "app_utils.hpp"
#include <ranges>
#include <iomanip>

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

SimpleAlignment make_simple_alignment(const std::string& qname, const std::vector<std::string_view>& fields){
  // Each supplemental alignment (SA tag) record should have 6 fields:
  //   rname, pos, strand, CIGAR, mapQ, NM
  // Each SimpleAlignment has 5 fields:
  //   qname, chrom, start, end, strand

  bool is_forward_strand{fields[2] == "+" ? true : false};

  // Convert string_view to int for start fields[1]
  int pos{0};
  view_to_numeric(fields[1], pos);

  std::vector<std::pair<int, char>> tokens{AlignmentReader::tokenize_cigar(fields[3])};
  int end{pos + AlignmentReader::reference_span_from_tokens(tokens)};

  return SimpleAlignment(
      std::string(qname),
      std::string(fields[0]),
      pos,
      end,
      is_forward_strand);
}

std::vector<std::string_view> parse_sa_record(std::string_view record){
	constexpr std::string_view field_delim{","};
  std::vector<std::string_view> fields{};
  fields.reserve(6);

  // Must be five delimiters and the tag identifier must be present.
  size_t count = std::count_if(record.begin(), record.end(), [](char c){return c == ',';});
  if(count != 5){
    std::cerr<<"Unexpected number of fields in SA tag"<<std::endl;
    return fields;
  }

  // For the first SA record, first five characters are tag identifier. Field data begins at 6th.
  size_t field_start{5};
  if(record.substr(0,5) != "SA:Z:"){
    field_start = 0;
  }

  // Ignoring NM field as it's not being used.
  for(size_t field_delim_pos{record.find(field_delim, field_start)};
      field_delim_pos != std::string_view::npos;
      field_delim_pos = record.find(field_delim, field_start)){

    fields.push_back(record.substr(field_start, field_delim_pos - field_start));
    field_start = field_delim_pos + 1;
  }

  return fields;
}

std::vector<SimpleAlignment> sa_value_to_alignments(std::string& qname, std::string_view sa_str){
  // Each record should be semicolon terminated with comma delimited fields.
	constexpr std::string_view record_delim{";"};
	constexpr std::string_view field_delim{","};

  size_t count = std::count_if(sa_str.begin(), sa_str.end(), [](char c){return c == ';';});
  std::vector<SimpleAlignment> result{};

  // Each record should have 6 fields: rname, pos, strand, CIGAR, mapQ, NM
  std::vector<std::string_view> records{};
  std::vector<std::string_view> fields;

  result.reserve(count);
  records.reserve(count);
  fields.reserve(6);

  size_t record_start{0};
  size_t record_delim_pos{sa_str.find(record_delim, record_start)};
  std::string_view rec{};

  while( record_delim_pos != std::string_view::npos && record_delim_pos < sa_str.length() ){
    rec = sa_str.substr(record_start, record_delim_pos - record_start);

    fields = parse_sa_record(rec);
    result.push_back(make_simple_alignment(qname, fields));

    record_start = record_delim_pos +1;
    record_delim_pos = sa_str.find(record_delim, record_start);
  }

  return result;
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
  std::vector<SimpleAlignment> sa_alignments;

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
        // Add the primary alignment to the output data
        add_alignment(all_data, sa, AlnType::SPLIT);
        counts.split++;

        std::string query_name = reader.get_query_name();
        std::string_view sa_tag = reader.get_sa_tag();

        // Add the supplemental alignment to the output data
        sa_alignments = sa_value_to_alignments(query_name, sa_tag);
        for(auto& supplemental_alignment : sa_alignments){
          add_alignment(all_data, supplemental_alignment, AlnType::SPLIT);
        }

        counts.split_sa += reader.count_sa_tag();
      }

      counts.total++;
    }
  } catch(std::runtime_error& ex){
    std::cerr<<"Error creating CRAM reader: "<<ex.what()<<"\n";
    return false;
  }

  std::cout<<all_data<<std::endl;
  return true;
}
