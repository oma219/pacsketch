/* 
 * Name: generate_fasta.h
 * Description: Header file for generate_fasta.cpp
 * Project: This file is part of pacsketch repo.
 * 
 * Author: Omar Ahmed
 * Date: October 17, 2021
 */ 

#ifndef _GEN_FASTA
#define _GEN_FASTA

#include <stdlib.h>

// Define method to randomly choose index from array of length x
#define GET_RANDOM_INDEX(x) (rand()%(x))

#define FASTA_WIDTH 60

#define FATAL_WARNING(x) do {std::fprintf(stderr, "Warning: %s\n", x); std::exit(1);} while (0)
#define THROW_EXCEPTION(x) do { throw x;} while (0)

struct GenerateFastaOptions {
    size_t k = 0; // kmer to check cardinality
    size_t num_seqs = 0; // number of sequences in output file
    size_t seq_length = 0; // length of FASTA sequence
public:
    void validate() {
        if (k <= 0 || k > 32) {FATAL_WARNING("The kmer length (-k) needs to be <=32");}
        if (num_seqs <= 0) {FATAL_WARNING("The number of sequences (-n) needs to be set as a positive number.");}
        if (seq_length <= 0) {FATAL_WARNING("The length of the sequence (-l) needs to be set as a positive number.");}
        if (k > seq_length) {FATAL_WARNING("The length of the text is too small for the given kmer value.");}
    }
};

/* Function Declarations */
void parse_generate_fasta_run_options(int argc, char** argv, GenerateFastaOptions* opts);
int generate_fasta_usage ();
void produce_fasta(size_t k, size_t num_seqs, size_t seq_length);
size_t compute_cardinality(size_t k, std::string curr_seq);

#endif /* end of _GEN_FASTA include */