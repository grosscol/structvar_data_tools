#ifndef CRAM_READER
#define CRAM_READER

#include <string>
#include "htslib/hts.h"
#include "htslib/sam.h"

class CramReader {
  public:
    CramReader(const std::string& in_path, const std::string& ref_path, const bool silent = true);
    ~CramReader();

    /* Advance reader */
    bool next_alignment();

    bool is_mapq_sufficent();

    /* Flag Checking functions */
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

    /* info field accessors */
    uint32_t get_n_cigar();
    std::string get_cigar_string();

  private:
    bam1_t*     alignment{};
    htsFile*    infile{nullptr};
    sam_hdr_t*  header{nullptr};
};
#endif
