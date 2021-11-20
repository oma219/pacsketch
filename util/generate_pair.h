/* 
 * Name: generate_pair.h
 * Description: Header file for generate_pair.cpp
 * Project: This file is part of pacsketch repo.
 * 
 * Author: Omar Ahmed
 * Date: November 18, 2021
 */ 

#ifndef _GEN_FASTA_PAIR
#define _GEN_FASTA_PAIR

#define FASTA_WIDTH 60
#define GET_RANDOM_INDEX(x) (rand()%(x))
#define FATAL_WARNING(x) do {std::fprintf(stderr, "\nWarning: %s\n\n", x); std::exit(1);} while (0)
#define THROW_EXCEPTION(x) do { throw x;} while (0)

/* Function Declarations */
bool is_valid_path(const char* output_prefix);
size_t compute_single_cardinality(size_t k, std::string curr_seq); 

struct GeneratePairOptions {
    size_t k = 0; // kmer to check cardinality
    std::string output_file_prefix = ""; // output file prefix is valid
    size_t seq_length = 0; // length of FASTA sequence

public:
    void validate() {
        if (k <= 0 || k > 32) {FATAL_WARNING("The kmer length (-k) needs to be <=32");}
        if (seq_length <= 0) {FATAL_WARNING("The length of the sequence (-l) needs to be set as a positive number.");}
        if (k > seq_length) {FATAL_WARNING("The length of the text is too small for the given kmer value.");}
        if (output_file_prefix == "") {FATAL_WARNING("Need to specify an output file prefix.");}
    }
};

/* Function Declarations */
void parse_generate_pair_run_options(int argc, char** argv, GeneratePairOptions* opts);
int generate_pair_usage ();
void produce_pair(size_t k, size_t seq_length, const char* output_file_prefix);


#endif /* end of _GEN_FASTA_PAIR include */