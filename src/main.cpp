#include <iostream>
#include "main.hpp"
#include "variant_processor.hpp"

int main(int argc, char* argv[]) {
  std::cout<<"Version: " 
    << PROJECT_VERSION_MAJOR << "." 
    << PROJECT_VERSION_MINOR << "."
    << PROJECT_VERSION_PATCH << "\n";

  het_hom_id_list result;
  result = separate_hets_homs();

  printf("%d\n", result.foo);
  std::cout<<"Fin\n";
  return 0;
}
