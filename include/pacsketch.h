/*
 * Name: pacsketch.h
 * Description: Header file for pacsketch.cpp
 * Project: This file is part of pacsketch repo.
 * 
 * Author: Omar Ahmed
 * Date: October 17, 2021
 */

#ifndef  _PACSKETCH_H
#define  _PACSKETCH_H

#include <unistd.h>
#include <fstream>
#include <vector>

/* Useful Macros */
#define NOT_IMPL(x) do { std::fprintf(stderr, "%s() is not implemented: %s\n", __func__, x); std::exit(1);} while (0)
#define FATAL_WARNING(x) do {std::fprintf(stderr, "\nWarning: %s\n\n", x); std::exit(1);} while (0)
#define THROW_EXCEPTION(x) do { std::fprintf(stderr, "\nError: %s\n\n", x); std::exit(1);} while (0)
#define LOG(...) do{std::fprintf(stderr, "[pacsketch] "); std::fprintf(stderr, __VA_ARGS__);\
                    std::fprintf(stderr, "\n");} while(0)

enum sketch_type {MINHASH, HLL, NOT_CHOSEN};
enum data_type {PACKET, FASTA};

/* Function Declarations */
bool is_file(const char* file_path);

struct PacsketchBuildOptions {
    /* struct to build the command-line arguments */

    // General values
    std::string input_file = ""; // input dataset
    sketch_type curr_sketch = NOT_CHOSEN; // sketch type we are building
    bool use_minhash = false; // Records whether user uses -M 
    bool use_hll = false; // Records whether user uses -H
    bool print_cardinality = false; // output cardinality after building
    bool input_fasta = false; // input data is a FASTA file (for development)
    data_type input_data_type = PACKET; // input data are packets by default

    // MinHash specific values
    size_t k_size = 0; // number of hashes to keep

    // HLL specific values
    uint8_t bit_prefix = 0;

public:
    void validate() {    
        /* Validates and finalizes the command-line options */
        if (!is_file(input_file.data())) {THROW_EXCEPTION(("The following path is not valid: " + input_file).data());}

        if (use_minhash && use_hll) {FATAL_WARNING("Both -M and -H cannot be specified at same time, please re-run with a single one of those options.\n");}
        if (!use_minhash && !use_hll) {FATAL_WARNING("Please specify the type of sketch to build, either MinHash or HLL.\n");}
    
        if (use_minhash) {curr_sketch=MINHASH;}
        if (use_hll) {curr_sketch=HLL;}

        if (curr_sketch == MINHASH && k_size == 0) {FATAL_WARNING("Please specify a value of k since you requested to build a MinHash sketch.\n");}
        if (curr_sketch == HLL && bit_prefix == 0) {FATAL_WARNING("Please specify a value for b since you requested to build a HLL.\n");}
        if (input_fasta) {input_data_type=FASTA;}
    }
};

struct PacsketchDistOptions {
    /* struct for dist sub-command command-line arguments */

    // General values
    std::vector<std::string> input_files; // input datasets
    sketch_type curr_sketch = NOT_CHOSEN; // sketch type we are building
    bool use_minhash = false; // Records whether user uses -M 
    bool use_hll = false; // Records whether user uses -H
    bool input_fasta = false; // input data is a FASTA file (for development)
    data_type input_data_type = PACKET; // input data are packets by default

    // MinHash specific values
    size_t k_size = 0; // number of hashes to keep

    // HLL specific values
    uint8_t bit_prefix = 0;

public:
    void validate() {    
        /* Validates and finalizes the command-line options */
        if (input_files.size() != 2) {FATAL_WARNING("Exactly two files needed to be specified.");}
        
        if (!is_file(input_files[0].data())) {THROW_EXCEPTION(("The following path is not valid: " + input_files[0]).data());}
        if (!is_file(input_files[1].data())) {THROW_EXCEPTION(("The following path is not valid: " + input_files[1]).data());}

        if (use_minhash && use_hll) {FATAL_WARNING("Both -M and -H cannot be specified at same time, please re-run with a single one of those options.\n");}
        if (!use_minhash && !use_hll) {FATAL_WARNING("Please specify the type of sketch to build, either MinHash or HLL.\n");}
    
        if (use_minhash) {curr_sketch=MINHASH;}
        if (use_hll) {curr_sketch=HLL;}

        if (curr_sketch == MINHASH && k_size == 0) {FATAL_WARNING("Please specify a value of k since you requested to build a MinHash sketch.\n");}
        if (curr_sketch == HLL && bit_prefix == 0) {FATAL_WARNING("Please specify a value for b since you requested to build a HLL.\n");}
        if (input_fasta) {input_data_type=FASTA;}
    }
};


/* Function Declarations */
int pacsketch_build_usage();
int pacsketch_dist_usage();
void parse_build_options(int argc, char** argv, PacsketchBuildOptions* opts);
void parse_dist_options(int argc, char** argv, PacsketchDistOptions* opts);
int build_main(int argc, char** argv); 
int dist_main(int argc, char** argv); 

#endif /* end of _PACSKETCH_H header */