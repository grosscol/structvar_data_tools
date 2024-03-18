#include <string>
#include <random>

#ifndef APP_CTL_DATA
#define APP_CTL_DATA
/**
 * Application flow control data
 * Data class to store the configuration that determines how the program behaves.
 * Determined from parsing and interpretting the command line arguments.
 * */
struct AppControlData {
    /**
     * Path to file on disk.  Defaults to '-' which reads from stdin.
     */
    std::string input_path{"-"};

    /**
     * Number of random samples to take of each het and hom set.
     */
    int num_rnd_samples{5};


    /**
     * Seed for PRNG used for sampling.
     */
    unsigned int rnd_seed{std::random_device{}()};

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
