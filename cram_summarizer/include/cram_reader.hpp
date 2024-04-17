#ifndef CRAM_READER
#define CRAM_READER

#include <string>

class CramReader {
  public:
    CramReader(const std::string& in_path, const bool silent = true);
    ~CramReader();
};
#endif
