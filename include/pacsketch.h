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

#define SAMPLING_RATE 0.0039 // Represents 1 in 256, based on a literature value

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

struct PacsketchSimulateOptions {
    /* struct for simulate sub-command command-line arguments */

    // General values
    std::vector<std::string> input_files; // input datasets
    sketch_type curr_sketch = NOT_CHOSEN; // sketch type we are building
    bool use_minhash = false; // Records whether user uses -M 
    bool use_hll = false; // Records whether user uses -H
    bool input_fasta = false; // input data is a FASTA file (for development)
    data_type input_data_type = PACKET; // input data are packets by default
    size_t num_windows = 0; // number of windows to simulate
    size_t num_records = 0; // number of records to include in each window
    double attack_percent = -1.0; // percentage of records in simulated window that are attack records
    bool test_mode = false; // means the user wants to use a test data set
    std::vector<std::string> test_files; // contains paths to test files <normal, attack>

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

        if (use_minhash && use_hll) {FATAL_WARNING("Both -M and -H cannot be specified at same time, please re-run with a single one of those options.");}
        if (!use_minhash && !use_hll) {FATAL_WARNING("Please specify the type of sketch to build, either MinHash or HLL.");}
    
        if (use_minhash) {curr_sketch=MINHASH;}
        if (use_hll) {curr_sketch=HLL;}

        if (curr_sketch == MINHASH && k_size == 0) {FATAL_WARNING("Please specify a value of k since you requested to build a MinHash sketch.");}
        if (curr_sketch == HLL && bit_prefix == 0) {FATAL_WARNING("Please specify a value for b since you requested to build a HLL.");}
        if (input_fasta) {FATAL_WARNING("The simulation sub-command can only be run with network data.");}

        if (num_records > 50000 || num_records == 0) {FATAL_WARNING("The number of records per window (n) needs to be 0 < x < 50,000");}
        if (num_windows > 10000 || num_windows == 0) {FATAL_WARNING("The number of windows (w) needs to be 0 < w < 10,000");}

        if ((attack_percent < 0.0 || attack_percent > 1.0) && !test_mode) {FATAL_WARNING("Make sure to set the attack ratio (-a). The attack percentage (a) needs to be a number between 0.0 <= a <= 1.0.");}
        if (use_minhash && k_size > num_records) {FATAL_WARNING("The window size is too small, it must be larger than value of k for MinHash sketches.");}
        
        if (test_mode && test_files.size() != 2) {FATAL_WARNING("To use test_mode, you have to provide exactly two file paths: normal, then attack.");}
        if (test_mode && !is_file(test_files[0].data())) {FATAL_WARNING("The first provided test file is not a valid path.");}
        if (test_mode && !is_file(test_files[1].data())) {FATAL_WARNING("The second provided test file is not a valid path.");}

        if (test_mode && num_records > 9700) {FATAL_WARNING("For test mode, you must make sure window size is less than 9,700 records.");}
    }
};


/* Function Declarations */
int pacsketch_build_usage();
int pacsketch_dist_usage();
int pacsketch_simulate_usage();
void parse_build_options(int argc, char** argv, PacsketchBuildOptions* opts);
void parse_dist_options(int argc, char** argv, PacsketchDistOptions* opts);
void parse_simulate_options(int argc, char** argv, PacsketchSimulateOptions* opts);
int build_main(int argc, char** argv); 
int dist_main(int argc, char** argv); 
int simulate_main(int argc, char** argv); 
char* cgets(char* buf, int max, char** data);
std::vector<std::string> sample_records_at_indexes(std::vector<std::string> dataset_vecs, std::vector<size_t> index_list);
std::vector<std::string> sample_mixed_records_at_indexes(std::vector<std::string> dataset_1_vecs, size_t num_dataset_1,
                                                         std::vector<std::string> dataset_2_vecs, size_t num_dataset_2,
                                                         std::vector<size_t> index_list);
inline std::tuple<size_t, size_t> determine_window_breakdown(size_t total_num, double attack_ratio); 
int simulate_test_main(std::vector<std::string> normal_records, std::vector<std::string> attack_records, PacsketchSimulateOptions sim_opts);
std::tuple<double, double> analyze_record_labels_in_window(std::vector<std::string> window_records);
double estimate_attack_ratio_with_sampling(std::vector<std::string> window_records);

#endif /* end of _PACSKETCH_H header */