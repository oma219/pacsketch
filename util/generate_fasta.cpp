/* 
 * Name: generate_fasta.cpp
 * Description: Contains code to generate random fasta files to be used
 *              as test input for pacsketch code. It will compute the kmer
 *              cardinality with each FASTA file in order to check the 
 *              accuracy of cardinality estimation methods.
 * Project: This file is part of pacsketch repo.
 * 
 * Author: Omar Ahmed
 * Date: October 17, 2021
 */ 

#include <iostream>
#include <generate_fasta.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>  
#include <array>
#include <set>

size_t compute_cardinality(size_t k, std::string curr_seq) {
    /* Computes the kmer cardinality of the DNA sequence */
    std::set<std::string> kmer_set;
    for (size_t pos = 0; pos <= curr_seq.length()-k; pos++) {
        kmer_set.insert(curr_seq.substr(pos, k));
    }
    return kmer_set.size();
}

void produce_fasta(size_t k, size_t num_seqs, size_t seq_length) {
    /* Produces the FASTA file, and cardinality measurements to stdout */
    std::array<char, 4> alphabet = {'A', 'C', 'G', 'T'};
    srand(time(NULL));

    for (size_t i = 0; i < num_seqs; i++) {
        std::fprintf(stdout, ">seq%ld\n", i);
        std::string total_seq = "";
        std::string curr_str = "";

        for (size_t j = 0; j < seq_length; j++) {
            if (j % FASTA_WIDTH == 0 && j) {
                std::fprintf(stdout, "%s\n", curr_str.data()); 
                total_seq += curr_str;
                curr_str = ""; 
            }
            curr_str += alphabet[GET_RANDOM_INDEX(4)];
        }
        if (curr_str.length()) {std::fprintf(stdout, "%s\n", curr_str.data()); total_seq += curr_str;}
        
        size_t cardinality = compute_cardinality(k, total_seq);
        std::fprintf(stderr, "CARDINALITY_seq%ld = %ld\n", i, cardinality);
    }
}

void parse_generate_fasta_run_options(int argc, char** argv, GenerateFastaOptions* opts) {
    /* Parses the command-line arguments */
    for (int c; (c = getopt(argc, argv, "hk:n:l:")) >= 0;){
        switch (c) {
            case 'h': generate_fasta_usage(); std::exit(1);
            case 'k': opts->k = std::max(std::atoi(optarg), 0); break;
            case 'n': opts->num_seqs = std::max(std::atoi(optarg), 0); break;
            case 'l': opts->seq_length = std::max(std::atoi(optarg), 0); break;
            default: generate_fasta_usage(); std::exit(1);
        }
    }
}

int generate_fasta_usage () {
    /* prints out the usage information for generate fasta utility */
    std::fprintf(stderr, "generate_fasta - outputs a FASTA file based on desired parameters\n");
    std::fprintf(stderr, "                 along with the exact kmer cardinality. The output\n");
    std::fprintf(stderr, "                 FASTA will go to stdout by default.\n");
    std::fprintf(stderr, "Usage: generate_fasta [options]\n\n");
    
    std::fprintf(stderr, "Options:\n");
    std::fprintf(stderr, "\t%-10sprints this usage message\n", "-h");
    std::fprintf(stderr, "\t%-10slength of k-mer to use for cardinality\n", "-k [arg]");
    std::fprintf(stderr, "\t%-10snumber of sequences in output FASTA\n", "-n [arg]");
    std::fprintf(stderr, "\t%-10slength of each sequence.\n", "-l [arg]");
    return 0;
}

int main (int argc, char ** argv) {
    /* main method of generate_fasta utility */
    if (argc > 1) {
        GenerateFastaOptions run_opts;
        parse_generate_fasta_run_options(argc, argv, &run_opts);
        run_opts.validate();

        produce_fasta(run_opts.k, run_opts.num_seqs, run_opts.seq_length);
        return 0;
    } 
    else {return generate_fasta_usage();}
}