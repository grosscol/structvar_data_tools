#ifndef SIMPLE_ALIGNMENT
#define SIMPLE_ALIGNMENT

#include <ostream>
#include "boost/json.hpp"

namespace bj = boost::json;

class SimpleAlignment {
  public:
    SimpleAlignment(const std::string& qname, const std::string& chr, const int start, const int end, const bool strand);
    SimpleAlignment(const std::string& chr, const int start, const int end, const bool strand);
    SimpleAlignment();
    ~SimpleAlignment();

    // Query name
    std::string qname {};
    // Chromosome without "chr" prefix
    std::string chr {};
    // Alignment start and stop aligned to reference genome.
    int start {0};
    int end {0};
    // Is this alignment on the forward strand
    bool strand {true};

    friend std::ostream& operator<<(std::ostream& os, const SimpleAlignment& sa);

    // Create minimal json object of chr, start, end, strand
    bj::object to_json();
};

#endif
