#include <iostream>
#include <string>
#include <stdexcept>
#include <bcf_sampler.hpp>
#include <htslib/hts_log.h>
#include <htslib/vcf.h>

BcfSampler::BcfSampler(const std::string& in_path)
{ 
  hts_set_log_level(HTS_LOG_OFF);

  bcf_header = bcf_hdr_init("r");

  infile = bcf_open(in_path.data(), "r");
  if(!infile){
    throw std::runtime_error(std::string("Failed to open ") + in_path);
  }

}

BcfSampler::~BcfSampler(){
  bcf_close(infile);
}
