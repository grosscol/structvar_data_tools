#ifndef BCF_SAMPLER
#define BCF_SAMPLER

#include <htslib/vcf.h>

class BcfSampler {
  public:
    BcfSampler(const std::string& in_path);
    ~BcfSampler();
  private:
    htsFile* infile{nullptr};
    bcf_hdr_t* bcf_header{nullptr};
};

#endif /*BCF_SAMPLER*/
