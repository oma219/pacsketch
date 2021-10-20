/*
 * Name: pacsketch.cpp
 * Description: Main file for pacsketch project.
 * Project: pacsketch project.
 * 
 * Author: Omar Ahmed
 * Date: October 17, 2021
 */

#include <iostream>
#include <cstring>
#include <fstream>
#include <unistd.h>
#include <pacsketch.h>
#include <hash.h>
#include <minhash.h>

bool is_file(const char* file_path) {
    /* Checks if the path is a valid file-path */
    std::ifstream test_file(file_path, std::ifstream::in);
    if (test_file.fail()) {return 0;}
    
    test_file.close();
    return 1;   
}

int pacsketch_build_usage() {
    /* Prints out the usage information for pacsketch build sub-command */
    std::fprintf(stderr, "\npacsketch build - builds the skeches for a given input dataset.\n");
    std::fprintf(stderr, "Usage: pacsketch build [options]\n\n");

    std::fprintf(stderr, "Options:\n");
    std::fprintf(stderr, "\t%-10sprints this usage message\n", "-h");
    std::fprintf(stderr, "\t%-10spath to input file that has index built for it\n", "-i [FILE]");
    std::fprintf(stderr, "\t%-10sinput data is in FASTA format (used for dev)\n", "-f");
    std::fprintf(stderr, "\t%-10sbuild a MinHash sketch from input data\n", "-M");
    std::fprintf(stderr, "\t%-10sbuild a HyperLogLog sketch from input data\n", "-H");
    std::fprintf(stderr, "\t%-10soutput the cardinality of the sketch after building\n\n", "-c");

    std::fprintf(stderr, "MinHash specific options:\n");
    std::fprintf(stderr, "\t%-10snumber of hashes to keep in sketch\n\n", "-k [arg]");
    return 1;
}

void parse_build_options(int argc, char** argv, PacsketchBuildOptions* opts) {
    /* Parses the command-line options for build sub-command */
    for (int c; (c=getopt(argc, argv, "hi:fMHck:")) >= 0;) {
        switch (c) {
            case 'h': pacsketch_build_usage(); std::exit(1);
            case 'i': opts->input_file.assign(optarg); break;
            case 'f': opts->input_fasta = true; break;
            case 'M': opts->use_minhash = true; break;
            case 'H': opts->use_hll = true; break;
            case 'c': opts->print_cardinality = true; break;
            case 'k': opts->k_size = std::max(std::atoi(optarg), 0); break;
            default: pacsketch_build_usage(); std::exit(1);
        }
    }
}

int build_main(int argc, char** argv) {
    /* main method for build sub-command */
    if (argc == 1) {return pacsketch_build_usage();}

    // Grab the build options, and validate they are not missing/don't make sense 
    PacsketchBuildOptions build_opts;
    parse_build_options(argc, argv, &build_opts);
    build_opts.validate();

    // Build the sketch
    if (build_opts.curr_sketch == MINHASH) {
        MinHash data_sketch (build_opts.input_file, build_opts.k_size, build_opts.input_data_type);
        if (build_opts.print_cardinality) {
            std::fprintf(stdout, "Estimated Cardinality of Data: %lld\n\n", data_sketch.get_cardinality());
        }
    }
    return 1;
}

int dist_main(int argc, char** argv) {
    /* main method for dist sub-command */
    NOT_IMPL("still working on this ...");
    return 1;
}

int pacsketch_usage() {
    /* Prints out the usage for pacsketch (if no sub-command, or incorrect sub-command is used) */
    std::fprintf(stderr, "\npacsketch has different sub-commands to run which can be used as follows:\n");
    std::fprintf(stderr, "Usage: pacsketch <command> [options]\n\n");

    std::fprintf(stderr, "Commands:\n");
    std::fprintf(stderr, "\tbuild\tbuilds the sketches for packet traces (different sketches can be used)\n");
    std::fprintf(stderr, "\tdist\tcompares sketches and computes similarity measures between them\n\n");
    return 1;
}

int main(int argc, char** argv){
    /* main method for pacsketch package */
    
    if (argc > 1) {
        if (std::strcmp(argv[1], "build") == 0)
            return build_main(argc-1, argv+1);
        if (std::strcmp(argv[1], "dist") == 0)
            return dist_main(argc-1, argv+1);
    }
    return pacsketch_usage();
}
