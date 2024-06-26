#ifndef APP_CTL_DATA
#define APP_CTL_DATA

#include <string>
#include <vector>
#include <random>

/**
 * Application flow control data
 * Data class to store the configuration that determines how the program behaves.
 * Determined from parsing and interpretting the command line arguments.
 * */
struct AppControlData {
  /**
   * Path to file(s) on disk.  Defaults to empty which reads from stdin.
   */
  std::string input_path{};

  /**
   * Path to reference fasta on disk required for reading cram files.
   */
  std::string ref_path{};

  /**
   * Should version string be printed to stdout.
   * Should program exit without reading or processing data.
   */
  bool print_version{false};
  bool just_exit{false};
};

#endif
