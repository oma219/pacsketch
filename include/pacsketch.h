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

/* Useful Macros */
#define NOT_IMPL(x) do { std::fprintf(stderr, "%s() is not implemented: %s\n", __func__, x); std::exit(1);} while (0)
#define FATAL_WARNING(x) do {std::fprintf(stderr, "Warning: %s\n", x); std::exit(1);} while (0)
#define THROW_EXCEPTION(x) do { throw x;} while (0)
#define LOG(...) do{std::fprintf(stderr, "[pacsketch] "); std::fprintf(stderr, __VA_ARGS__);\
                    std::fprintf(stderr, "\n");} while(0)


/* Function Declarations */
bool is_file(const char* file_path);

enum sketch_type {MINHASH, HLL, NOT_CHOSEN};
enum data_type {PACKET, FASTA};

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
        if (!is_file(input_file.data())) {THROW_EXCEPTION(std::runtime_error("The following path is not valid: " + input_file));}

        if (use_minhash && use_hll) {FATAL_WARNING("Both -M and -H cannot be specified at same time, please re-run with a single one of those options.\n");}
        if (!use_minhash && !use_hll) {FATAL_WARNING("Please specify the type of sketch to build, either MinHash or HLL.\n");}
    
        if (use_minhash) {curr_sketch=MINHASH;}
        if (use_hll) {curr_sketch=HLL;}

        if (curr_sketch == MINHASH && k_size == 0) {FATAL_WARNING("Please specify a value of k since you requested to build a MinHash sketch.\n");}
        if (curr_sketch == HLL && bit_prefix == 0) {FATAL_WARNING("Please specify a value for b since you requested to build a HLL.\n");}
        if (input_fasta) {input_data_type=FASTA;}

    }

};


#endif /* end of _PACSKETCH_H header */