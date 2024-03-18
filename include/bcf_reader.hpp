#ifndef BCF_READER
#define BCF_READER

#include <string>
#include <vector>
#include <memory>
#include <htslib/vcf.h>

/* Custom deleter to call free on genotype array data */
struct GT_Deleter {
  void operator() (int32_t* ptr) {
    free(ptr);
  }
};

class BcfReader {
  public:
    BcfReader(const std::string& in_path, const bool silent = true);
    ~BcfReader();

    // Advance state to next variant.  Return true if successful.
    bool next_variant();

    /* Lookup sample Id corresponding to given index */
    std::string sample_idx_to_id(const int& idx) const;
    std::vector<std::string> sample_idxs_to_ids(const std::vector<int>& idxs) const;

    /* Accessors */
    bool is_data_exhausted() const;
    std::string variant_id() const;
    std::string vcf_version() const;
    int n_samples() const;
    int n_hets() const;
    int n_homs() const;
    int64_t pos() const;
    std::string chr() const;
    std::string ref() const;
    std::string alt() const;

    const std::vector<int>& het_idxs() const;
    const std::vector<int>& hom_idxs() const;
    void print_genotypes() const;

    void scratch();

  private:
    std::string m_in_path{};
    htsFile*    infile{nullptr};
    bcf_hdr_t*  header{nullptr};
    bcf1_t*     variant{nullptr};

    // Flag that end of data has been reached.
    bool m_is_data_exhausted{false};

    // Status of the last read operation
    int  m_read_status{0};

    // number of samples
    int  m_num_samples{0};

    std::string m_chr{};
    int64_t m_pos{0};
    std::string m_ref{};
    // Assumes only one alt per record
    std::string m_alt{};

    /* Array of GT data with appropriate deleter.
    *  Data is sequence of alleles in sample order:
    *  [sample0_allele0, sample0_allele1, sample1_allele0, sample1_allele1,...]
    */
    std::unique_ptr<int32_t[], GT_Deleter> gt_array{nullptr};
    int m_num_gt{0};

    // indexes of heterozygous and homozygous sample ids
    std::vector<int> m_het_sample_id_idxs;
    std::vector<int> m_hom_sample_id_idxs;

    /* Set m_gt_array member to GT data.
    *  Set m_num_gt to length of GT data or negative number indicating an error.
    */
    void read_genotypes();

    /* parse out first four vcf columns */
    void parse_chr_pos_ref_alt();

    /* parse out indexes of het and hom samples */
    void parse_genotypes();
};

#endif /*BCF_READER*/
