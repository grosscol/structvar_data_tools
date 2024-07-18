#ifndef CRAM_READER
#define CRAM_READER

#include <string>
#include "htslib/hts.h"
#include "htslib/sam.h"

class AlignmentReader {
  public:
    AlignmentReader(const std::string& in_path, const std::string& ref_path, const bool silent = true);
    ~AlignmentReader();

    /* Advance reader */
    bool next_alignment();

    /* Status checking */
    bool has_sa_tag();
    bool is_mapq_sufficent();
    bool meets_split_criteria();
    bool meets_pair_criteria();

    /* Flag checking */
    bool is_paired();
    bool is_proper_pair();
    bool is_unmapped();
    bool is_mate_unmapped();
    bool is_reverse_strand();
    bool is_mate_reverse_strand();
    bool is_read_1();
    bool is_read_2();
    bool is_secondary();
    bool is_qc_fail();
    bool is_duplicate();
    bool is_supplementary();

    /* Field accessors */
    uint32_t get_n_cigar();
    std::string get_cigar_string();
    std::string get_sa_tag();

    /* Process data */

    // Sum lengths of matches, insertions, mismatches and skips. (MIXS)
    static int reference_span(std::string cigar);
    static std::vector<std::pair<int, char>> tokenize_cigar(const std::string& cigar);
    int get_alignment_end();
    int parse_sa_value();
    int count_sa_tag();

  private:
    bam1_t*     alignment{};
    htsFile*    infile{nullptr};
    sam_hdr_t*  header{nullptr};
};
#endif
