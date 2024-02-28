#ifndef BCF_READER
#define BCF_READER

#include <string>
#include <vector>
#include <htslib/vcf.h>

class BcfReader {
  public:
    BcfReader(const std::string& in_path, const bool silent = true);
    ~BcfReader();

    // Advance state to next variant.  Return true if successful.
    bool next_variant(); 

    /* Accessors */
    bool is_data_exhausted();
    std::string variant_id();
    std::string vcf_version();
    int n_samples();
    std::vector<int> random_hets(int num);
    std::vector<int> random_homs(int num);


  private:
    std::string m_in_path{};
    htsFile*    infile{nullptr};
    bcf_hdr_t*  header{nullptr};
    bcf1_t*     variant{nullptr};

    // parse out indexes of het and hom samples
    void parse_genotypes();
    
    // flag that end of data has been reached.
    //   state of instance will be last variant read.
    bool m_is_data_exhausted{false};

    // Control allowing htslib to print to stderr
    bool m_silent{true};

    // Status of the last read operation
    int  m_read_status{0};

    // number of samples
    int  m_n_samples{0};
};

#endif /*BCF_READER*/
