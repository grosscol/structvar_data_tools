#include <iostream>
#include <string>
#include <stdexcept>
#include <sstream>
#include <vector>
#include "cram_reader.hpp"
#include "htslib/hts_log.h"
#include "htslib/hts.h"
#include "htslib/sam.h"

AlignmentReader::AlignmentReader(const std::string& in_path, const std::string& ref_path, const bool silent)
{
  if(silent){
    hts_set_log_level(HTS_LOG_OFF);
  }

  infile = sam_open(in_path.data(), "r");
  if(!infile){
    throw std::runtime_error(std::string("Failed to open file: ") + in_path);
  }

  header = sam_hdr_read(infile);
  alignment = bam_init1();
}

AlignmentReader::~AlignmentReader(){
  bam_destroy1(alignment);
  sam_hdr_destroy(header);
  hts_close(infile);
}

bool AlignmentReader::next_alignment(){

  int ret_val{0};

  ret_val = sam_read1(infile, header, alignment);
  return ret_val == -1 ? false: true;
}


/*******************
 * Field Accessors *
 ******************/
uint32_t AlignmentReader::get_n_cigar(){
  return alignment->core.n_cigar;
}

std::string AlignmentReader::get_cigar_string(){
  uint32_t n_cigar{this->get_n_cigar()};
  uint32_t* cigar{bam_get_cigar(alignment)};

  std::ostringstream sstream;
  uint32_t operation{};
  uint32_t op_len{};
  char op_chr{};

  for(int i = 0; i < n_cigar; i++){
    operation = bam_cigar_op(cigar[i]);
    op_len = bam_cigar_oplen(cigar[i]);
    op_chr = bam_cigar_opchr(cigar[i]);

    sstream << op_len << op_chr;
  }

  return sstream.str();
}

std::string AlignmentReader::get_query_name(){
  return std::string( bam_get_qname(alignment) );
}

std::string AlignmentReader::get_sa_tag(){
  kstring_t kstr;
  ks_initialize(&kstr);
  int retcode = bam_aux_get_str(alignment,"SA", &kstr);

  return retcode == 1 ? std::string(kstr.s) : "";
}

bool AlignmentReader::is_forward_strand(){
  return !bam_is_rev(alignment);
}

int64_t AlignmentReader::get_start(){
  return alignment->core.pos;
}

int64_t AlignmentReader::get_end(){
  return alignment->core.pos + reference_span(get_cigar_string());
}

std::string AlignmentReader::get_chrom(){
  int tid = alignment->core.tid;
  return std::string(header->target_name[tid]);
}

/****************
 * Process Data *
 ***************/
int AlignmentReader::count_sa_tag(){
  int count{0};

  std::string sa_str = get_sa_tag();
  for (auto& ch : sa_str){
    if( ch == ';'){ count++; }
  }

  return count;
}

std::vector<std::pair<int, char>> AlignmentReader::tokenize_cigar(const std::string& cigar){
  std::vector<std::pair<int, char>> tokens{};
  int position{0};
  size_t used{0};
  int count{0};
  char operation{};

  while(position < cigar.length()){
    count = std::stoi(cigar.substr(position), &used);
    position += used;

    operation = cigar.at(position);
    position++;

    tokens.push_back(std::make_pair(count, operation));
  }

  return tokens;
}

int AlignmentReader::reference_span(const std::string& cigar){
  std::vector<std::pair<int, char>> tokens{AlignmentReader::tokenize_cigar(cigar)};
  int sum{0};

  for( auto &token : tokens ){
    switch(std::get<char>(token)) {
      case 'M':
      case 'D':
      case 'N':
      case 'X':
        sum += std::get<int>(token);
      default:
        break;
    }
  }
  return sum;
}

int AlignmentReader::get_alignment_end(){

  return 0;
}

int parse_sa_value(){
  return 0;
}

/*****************
 * Flag Checking *
 ****************/
bool AlignmentReader::is_paired(){        return alignment->core.flag & BAM_FPAIRED; }
bool AlignmentReader::is_proper_pair(){   return alignment->core.flag & BAM_FPROPER_PAIR; }
bool AlignmentReader::is_unmapped(){      return alignment->core.flag & BAM_FUNMAP; }
bool AlignmentReader::is_mate_unmapped(){       return alignment->core.flag & BAM_FMUNMAP; }
bool AlignmentReader::is_reverse_strand(){      return alignment->core.flag & BAM_FREVERSE; }
bool AlignmentReader::is_mate_reverse_strand(){ return alignment->core.flag & BAM_FMREVERSE; }
bool AlignmentReader::is_read_1(){        return alignment->core.flag & BAM_FREAD1; }
bool AlignmentReader::is_read_2(){        return alignment->core.flag & BAM_FREAD2; }
bool AlignmentReader::is_secondary(){     return alignment->core.flag & BAM_FSECONDARY; }
bool AlignmentReader::is_qc_fail(){       return alignment->core.flag & BAM_FQCFAIL; }
bool AlignmentReader::is_duplicate(){     return alignment->core.flag & BAM_FDUP; }
bool AlignmentReader::is_supplementary(){ return alignment->core.flag & BAM_FSUPPLEMENTARY; }

/*******************
 * Status Checking *
 ******************/
bool AlignmentReader::has_sa_tag(){
  return bam_aux_get(alignment,"SA");
}

bool AlignmentReader::is_mapq_sufficent(){
  return alignment->core.qual > 1 && alignment->core.qual < 255;
}

bool AlignmentReader::meets_pair_criteria(){
  return !is_unmapped() && !is_secondary() && !is_supplementary() && is_paired();
}

bool AlignmentReader::meets_split_criteria(){
  return !is_secondary() && !is_supplementary() && has_sa_tag();
}
