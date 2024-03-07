/* scratch space for experimenting */
#include <vector>
#include <iostream>
#include <random>
#include <string>
#include <htslib/vcf.h>

std::vector<int> mock_process_variant(std::mt19937& generator)
{
	std::uniform_int_distribution<int> distr(0, 100'000);
  std::vector<int> selections(10);

	for(int n=0; n<10; ++n){
		selections[n] =  distr(generator);
  }

  std::cout<<&selections<<std::endl;
  return selections;
}

int main(){
  int max_iterations{2};
  std::vector<int> result_1{};
  std::vector<int> result_2{};

  /*
   * Setup random generator
   */
	std::random_device rd; 
	std::mt19937 generator(rd()); 

  result_1 = mock_process_variant(generator);
  result_2 = mock_process_variant(generator);

  for(const int& i : result_1){
    std::cout<<i<<' ';
  }
  std::cout<<std::endl;
  for(const int& i : result_2){
    std::cout<<i<<' ';
  }
  std::cout<<std::endl;

  std::cout<<&result_1<<std::endl;
  std::cout<<&result_2<<std::endl;

  /*
   * Sample BCF init and free
   */
  std::string in_path{"/home/grosscol/workspace/het_hom_selector/test/data/structvar_sample_input.vcf"};
  htsFile*    infile{nullptr};
  bcf_hdr_t* header {nullptr};

  infile = bcf_open(in_path.data(), "r");
  header = bcf_hdr_read(infile);

  // do something trivial
  std::cout<<"htsfile ptr: "<<infile<<std::endl;

  // immediately close file
  hts_close(infile);
  bcf_hdr_destroy(header);

  std::cout<<"done"<<std::endl;
  return 0;
}
