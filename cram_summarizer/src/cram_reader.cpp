#include <iostream>
#include <string>
#include <stdexcept>
#include <sstream>
#include "cram_reader.hpp"
#include "htslib/hts_log.h"
#include "htslib/hts.h"
#include "htslib/sam.h"

CramReader::CramReader(const std::string& in_path, const std::string& ref_path, const bool silent)
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

CramReader::~CramReader(){
  bam_destroy1(alignment);
  sam_hdr_destroy(header);
  hts_close(infile);
}

bool CramReader::next_alignment(){

  int ret_val{0};

  ret_val = sam_read1(infile, header, alignment);
  return ret_val == -1 ? false: true;
}

/***************************
 * Flag Checking functions *
 ***************************/
bool CramReader::is_paired(){        return alignment->core.flag & BAM_FPAIRED; }
bool CramReader::is_proper_pair(){   return alignment->core.flag & BAM_FPROPER_PAIR; }
bool CramReader::is_unmapped(){      return alignment->core.flag & BAM_FUNMAP; }
bool CramReader::is_mate_unmapped(){       return alignment->core.flag & BAM_FMUNMAP; }
bool CramReader::is_reverse_strand(){      return alignment->core.flag & BAM_FREVERSE; }
bool CramReader::is_mate_reverse_strand(){ return alignment->core.flag & BAM_FMREVERSE; }
bool CramReader::is_read_1(){        return alignment->core.flag & BAM_FREAD1; }
bool CramReader::is_read_2(){        return alignment->core.flag & BAM_FREAD2; }
bool CramReader::is_secondary(){     return alignment->core.flag & BAM_FSECONDARY; }
bool CramReader::is_qc_fail(){       return alignment->core.flag & BAM_FQCFAIL; }
bool CramReader::is_duplicate(){     return alignment->core.flag & BAM_FDUP; }
bool CramReader::is_supplementary(){ return alignment->core.flag & BAM_FSUPPLEMENTARY; }

bool CramReader::is_mapq_sufficent(){
  return alignment->core.qual > 1 && alignment->core.qual < 255;
}

uint32_t CramReader::get_n_cigar(){
  return alignment->core.n_cigar;
}

std::string CramReader::get_cigar_string(){
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
