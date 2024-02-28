#include <iostream>
#include <string>
#include <stdexcept>
#include <bcf_reader.hpp>
#include <htslib/hts_log.h>
#include <htslib/vcf.h>

BcfReader::BcfReader(const std::string& in_path, const bool silent)
{ 
  if(silent){
    hts_set_log_level(HTS_LOG_OFF);
  }

  m_in_path = in_path;
  m_silent = silent;
  header = bcf_hdr_init("r");
  variant = bcf_init();

  infile = bcf_open(in_path.data(), "r");
  if(!infile){
    throw std::runtime_error(std::string("Failed to open ") + in_path);
  }

  header = bcf_hdr_read(infile);
  if(!header){
    throw std::runtime_error(std::string("Failed to read header."));
  }

  m_n_samples = bcf_hdr_nsamples(header);

}

BcfReader::~BcfReader(){
  hts_close(infile);
  bcf_hdr_destroy(header);
  bcf_destroy(variant);
}

bool BcfReader::next_variant(){
  int read_status{bcf_read(infile, header, variant)};

  if(read_status == -1){
    m_is_data_exhausted = true;
    return false;
  }else if(read_status < -1){
    throw std::runtime_error(std::string("Error reading next variant."));
  }

  return true;
}

int BcfReader::n_samples(){
  return m_n_samples;
}

bool BcfReader::is_data_exhausted(){ 
  return m_is_data_exhausted;
}

std::string BcfReader::vcf_version(){
  const char* header_version{bcf_hdr_get_version(header)};
  return std::string{header_version};
}

std::string BcfReader::variant_id(){
  bcf_unpack(variant, BCF_UN_STR);

  std::string id_str{variant->d.id};
  return id_str;
}

void BcfReader::parse_genotypes(){
  bcf_unpack(variant, BCF_UN_FMT);

}
