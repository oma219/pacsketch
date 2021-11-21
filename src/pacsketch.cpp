/*
 * Name: pacsketch.cpp
 * Description: Main file for pacsketch project.
 * Project: This file is part of pacsketch repo.
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
#include <hll.h>

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

    std::fprintf(stderr, "HyperLogLog specific options:\n");
    std::fprintf(stderr, "\t%-10snumber of bits to use for choosing registers\n\n", "-b [arg]");
    return 1;
}

int pacsketch_dist_usage() {
    /* Prints out the usage information for pacsketch build sub-command */
    std::fprintf(stderr, "\npacsketch dist - computes the jaccard between two sketches.\n");
    std::fprintf(stderr, "Usage: pacsketch dist -i file1 -i file2 [options]\n\n");

    std::fprintf(stderr, "Options:\n");
    std::fprintf(stderr, "\t%-10sprints this usage message\n", "-h");
    std::fprintf(stderr, "\t%-10spath to input file that has index built for it\n", "-i [FILE]");
    std::fprintf(stderr, "\t%-10sinput data is in FASTA format (used for dev)\n", "-f");
    std::fprintf(stderr, "\t%-10sbuild a MinHash sketch from input data\n", "-M");
    std::fprintf(stderr, "\t%-10sbuild a HyperLogLog sketch from input data\n", "-H");

    std::fprintf(stderr, "MinHash specific options:\n");
    std::fprintf(stderr, "\t%-10snumber of hashes to keep in sketch\n\n", "-k [arg]");

    std::fprintf(stderr, "HyperLogLog specific options:\n");
    std::fprintf(stderr, "\t%-10snumber of bits to use for choosing registers\n\n", "-b [arg]");
    return 1;
}


void parse_build_options(int argc, char** argv, PacsketchBuildOptions* opts) {
    /* Parses the command-line options for build sub-command */
    for (int c; (c=getopt(argc, argv, "hi:fMHck:b:")) >= 0;) {
        switch (c) {
            case 'h': pacsketch_build_usage(); std::exit(1);
            case 'i': opts->input_file.assign(optarg); break;
            case 'f': opts->input_fasta = true; break;
            case 'M': opts->use_minhash = true; break;
            case 'H': opts->use_hll = true; break;
            case 'c': opts->print_cardinality = true; break;
            case 'k': opts->k_size = std::max(std::atoi(optarg), 0); break;
            case 'b': opts->bit_prefix = std::max(std::atoi(optarg), 0); break;
            default:  std::exit(1);
        }
    }
}

void parse_dist_options(int argc, char** argv, PacsketchDistOptions* opts) {
    /* Parses the command-line options for dist sub-command */
    for (int c; (c=getopt(argc, argv, "hi:fMHk:b:")) >= 0;) {
        switch (c) {
            case 'h': pacsketch_build_usage(); std::exit(1);
            case 'i': opts->input_files.push_back(optarg); break;
            case 'f': opts->input_fasta = true; break;
            case 'M': opts->use_minhash = true; break;
            case 'H': opts->use_hll = true; break;
            case 'k': opts->k_size = std::max(std::atoi(optarg), 0); break;
            case 'b': opts->bit_prefix = std::max(std::atoi(optarg), 0); break;
            default:  std::exit(1);
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
            std::fprintf(stdout, "Estimated_Cardinality: %lld\n", data_sketch.get_cardinality());
        }
    } else if (build_opts.curr_sketch == HLL) {
        HyperLogLog data_sketch (build_opts.input_file, build_opts.bit_prefix, build_opts.input_data_type);
        if (build_opts.print_cardinality) {
            std::fprintf(stdout, "Estimated_Cardinality: %lld\n", data_sketch.compute_cardinality());
        }
    }
    return 1;
}

int dist_main(int argc, char** argv) {
    /* main method for dist sub-command */
    if (argc == 1) {return pacsketch_dist_usage();}

    // Grab the dist options, and validate they are not missing/don't make sense 
    PacsketchDistOptions dist_opts;
    parse_dist_options(argc, argv, &dist_opts);
    dist_opts.validate();

    // Build the sketches for each input file
    if (dist_opts.curr_sketch == MINHASH) {
        MinHash data_sketch_1 (dist_opts.input_files[0], dist_opts.k_size, dist_opts.input_data_type);
        MinHash data_sketch_2 (dist_opts.input_files[1], dist_opts.k_size, dist_opts.input_data_type);

        uint64_t card_a = data_sketch_1.get_cardinality();
        uint64_t card_b = data_sketch_2.get_cardinality();

        // Generate union sketch, and estimate jaccard
        MinHash union_sketch = data_sketch_1 + data_sketch_2;
        uint64_t card_union = union_sketch.get_cardinality();
        auto jaccard = (card_a + card_b - card_union + 0.0)/(card_union);

        std::cout << "Estimated values based on MinHash sketches ...\n";
        std::cout << std::right << std::setw(10) << "|SET(A)|" <<
                     std::right << std::setw(10) << "|SET(B)|" <<
                     std::right << std::setw(15) << "|SET(AUB)|"  <<
                     std::right << std::setw(10) << "J(A,B)" << std::endl;
        std::cout << std::right << std::setw(10) << card_a <<
                     std::right << std::setw(10) << card_b <<
                     std::right << std::setw(15) << card_union <<
                     std::right << std::setw(10) << std::setprecision(4) << jaccard << std::endl;

    } else if (dist_opts.curr_sketch == HLL) {
        HyperLogLog data_sketch_1 (dist_opts.input_files[0], dist_opts.bit_prefix, dist_opts.input_data_type);
        HyperLogLog data_sketch_2 (dist_opts.input_files[1], dist_opts.bit_prefix, dist_opts.input_data_type);

        uint64_t card_a = data_sketch_1.compute_cardinality();
        uint64_t card_b = data_sketch_2.compute_cardinality();
                
        // Generate union sketch, and estimate jaccard
        HyperLogLog union_sketch = data_sketch_1 + data_sketch_2;
        uint64_t card_union = union_sketch.compute_cardinality();
        auto jaccard = (card_a + card_b - card_union + 0.0)/(card_union);

        std::cout << "Estimated values based on HyperLogLog sketches ...\n";
        std::cout << std::right << std::setw(10) << "|SET(A)|" <<
                     std::right << std::setw(10) << "|SET(B)|" <<
                     std::right << std::setw(15) << "|SET(AUB)|"  <<
                     std::right << std::setw(10) << "J(A,B)" << std::endl;
        std::cout << std::right << std::setw(10) << card_a <<
                     std::right << std::setw(10) << card_b <<
                     std::right << std::setw(15) << card_union <<
                     std::right << std::setw(10) << std::setprecision(4) << jaccard << std::endl;
    }

    
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
