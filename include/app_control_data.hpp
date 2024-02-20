#include <string>

#ifndef APP_CTL_DATA
#define APP_CTL_DATA
/**
 * Application flow control data
 * Data class to store the configuration that determines how the program behaves.  
 * Determined from parsing and interpretting the command line arguments.
 * */
class AppControlData {
  public:
    /**
     * Path to file on disk.  Defaults to '-' which reads from stdin.
     */
    std::string input_path{"-"};

    /**
     * Bool indiciting to print the version string to stdout.
     */
    bool print_version{false};

    /**
     * Bool indication to exit the program without reading or processing data.
     */
    bool just_exit{false};
};

#endif
