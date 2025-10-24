#include "htslib/hts.h"
#include "htslib/sam.h"
#include <iostream>

int main(){

  std::cout<<"Demo"<<std::endl;

  std::cout<<"Ops"<<std::endl
    <<bam_cigar_op('m')<<", "
    <<bam_cigar_op('i')<<", "
    <<bam_cigar_op('d')<<", "
    <<bam_cigar_op('n')<<", "
    <<bam_cigar_op('s')<<std::endl;

  return 0;
}
