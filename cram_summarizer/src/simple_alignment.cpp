#include "simple_alignment.hpp"

SimpleAlignment::SimpleAlignment(
    const std::string& qname, const std::string& chr, const int start, const int end, const bool strand) :
  qname(qname), chr(chr), start(start), end(end), strand(strand) {}

SimpleAlignment::SimpleAlignment(
    const std::string& chr, const int start, const int end, const bool strand) :
    SimpleAlignment("*", chr, start, end, strand) {}

SimpleAlignment::SimpleAlignment() : SimpleAlignment("*", "zzzz", 0, 1, true) {}

SimpleAlignment::~SimpleAlignment(){}

std::ostream& operator<<(std::ostream& os, const SimpleAlignment& sa){
  os << sa.chr << sa.start << sa.end << sa.strand;
  return os;
}

boost::json::object SimpleAlignment::to_json(){
  boost::json::object obj;

  obj["chr"] = chr;
  obj["start"] = start;
  obj["end"] = end;
  obj["is_reverse"] = !strand;

  return obj;
}

