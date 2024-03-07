#include <iostream>
#include <string>
#include <stdexcept>
#include <memory>
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
  variant = bcf_init();

  infile = bcf_open(in_path.data(), "r");
  if(!infile){
    throw std::runtime_error(std::string("Failed to open ") + in_path);
  }

  header = bcf_hdr_read(infile);
  if(!header){
    throw std::runtime_error(std::string("Failed to read header."));
  }

  m_num_samples = bcf_hdr_nsamples(header);

  // Allocate max the space list of indexes could need.
  //   Probably 1.5Mb altogether for big inputs
  m_het_sample_id_idxs.reserve(m_num_samples);
  m_hom_sample_id_idxs.reserve(m_num_samples);
}

BcfReader::~BcfReader(){
  hts_close(infile);
  bcf_hdr_destroy(header);
  bcf_destroy(variant);
}

int BcfReader::n_hets(){
  return m_het_sample_id_idxs.size();
}

int BcfReader::n_homs(){
  return m_hom_sample_id_idxs.size();
}

bool BcfReader::next_variant(){
  int read_status{bcf_read(infile, header, variant)};

  if(read_status == -1){
    m_is_data_exhausted = true;
    return false;
  }else if(read_status < -1){
    throw std::runtime_error(std::string("Error reading next variant."));
  }

  m_het_sample_id_idxs.clear();
  m_hom_sample_id_idxs.clear();
  parse_genotypes();
  return true;
}

int BcfReader::n_samples(){
  return m_num_samples;
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

void BcfReader::read_genotypes(){
  /* init_ptr: Initial pointer and will be allocated GT data by htslib.
   * num_gt_arr: Number of genotypes read will be filled in by htslib.
   */
  int32_t* init_ptr{nullptr};
  int32_t num_gt_arr{0};

  // Fill in initial pointer
  m_num_gt = bcf_get_genotypes(header, variant, &init_ptr, &num_gt_arr);

  // Take responsibility using unique_ptr deleting any existing data
  gt_array.reset(init_ptr);
}

void BcfReader::print_genotypes(){
  int allele_idx{0};
  int allele_val{0};
  bool is_even_idx{true};

  for(int sample_idx=0; sample_idx < m_num_gt; sample_idx += 2){
    for(int allele_offset=0; allele_offset < 2; allele_offset++){
      allele_idx = sample_idx + allele_offset;
      allele_val = gt_array[allele_idx];

      if(bcf_gt_is_missing(allele_val)){
        std::cout<<".";
      }else{
        std::cout<<bcf_gt_allele(allele_val);
      }

      allele_offset < 1 ? std::cout<<"/" : std::cout<<" ";
    }
  }
}

void BcfReader::parse_genotypes(){
  read_genotypes();

  // No genotypes present
  if(m_num_gt <=0){ return; }

  /* Genotype parsing variables
   *
   * max_ploidy: Number of alleles per sample.
   * allele_val: htslib representation of allele. Different than VCF 0-based index.
   *             Use bcf_gte_allele(val) to translate.
   * alt_count:  Count ALTs to determine if sample is het or hom.
   * allele_idx: Index of given sample-allele into data.
   */
  int max_ploidy = m_num_gt / m_num_samples;
  int allele_val {0};
  int alt_count  {0};
  int allele_idx {0};

  for(int sample_idx=0; sample_idx < m_num_gt; sample_idx += 2){
    alt_count = 0;

    for(int allele_offset=0; allele_offset < max_ploidy; allele_offset++){
      allele_idx = sample_idx + allele_offset;
      allele_val = gt_array[allele_idx];

      if(bcf_gt_allele(allele_val) > 0){
        alt_count++;
      }
    }

    // Add sample id to list of het or hom sample indexes when appropriate.
    switch(alt_count){
      case 2:
        m_hom_sample_id_idxs.insert(m_hom_sample_id_idxs.end(), sample_idx);
        break;
      case 1:
        m_het_sample_id_idxs.insert(m_het_sample_id_idxs.end(), sample_idx);
        break;
    }
  }
}
