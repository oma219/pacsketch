/* 
 * Name: generate_pair.cpp
 * Description: Contains code to generate pairs of FASTA files and then
 *              we will calculate the exact jaccard value between these
 *              files. We then can test the accuracy of our data-structures
 *              in terms of their ability to approximate similarity 
 *              between sketches.
 * Project: This file is part of pacsketch repo.
 * 
 * Author: Omar Ahmed
 * Date: November 18, 2021
 */

#include <iostream>
#include <generate_pair.h>
#include <unistd.h>

void produce_pair(size_t k, size_t seq_length, const char* output_file_prefix) {
    /* Takes in command-line arguments, and produces a pair of FASTA files */
    std::cout << "hello from produce_pair\n";
}

void parse_generate_pair_run_options(int argc, char** argv, GeneratePairOptions* opts) {
    /* Parses the command-line arguments */
    for (int c; (c = getopt(argc, argv, "hk:l:o:")) >= 0;){
        switch (c) {
            case 'h': generate_pair_usage(); std::exit(1);
            case 'k': opts->k = std::max(std::atoi(optarg), 0); break;
            case 'o': opts->output_file_prefix.assign(optarg); break;
            case 'l': opts->seq_length = std::max(std::atoi(optarg), 0); break;
            default: generate_pair_usage(); std::exit(1);
        }
    }
}

int generate_pair_usage () {
    /* prints out the usage information for generate pair utility */
    std::fprintf(stderr, "generate_pair - outputs a pair of FASTA files based on desired parameters\n");
    std::fprintf(stderr, "                 along with the exact jaccard similarity score. The output\n");
    std::fprintf(stderr, "                 FASTA will go the two files using the provided output prefix.\n");
    std::fprintf(stderr, "Usage: generate_pair [options]\n\n");
    
    std::fprintf(stderr, "Options:\n");
    std::fprintf(stderr, "\t%-10sprints this usage message\n", "-h");
    std::fprintf(stderr, "\t%-10slength of k-mer to use for sketch\n", "-k [arg]");
    std::fprintf(stderr, "\t%-10slength of each sequence.\n", "-l [arg]");
    std::fprintf(stderr, "\t%-10soutput file prefix for FASTA files\n", "-o [arg]");
    return 0;
}

int main (int argc, char ** argv) {
    /* main method of generate_pair utility */
    if (argc > 1) {
        GeneratePairOptions run_opts;
        parse_generate_pair_run_options(argc, argv, &run_opts);
        run_opts.validate();

        produce_pair(run_opts.k, run_opts.seq_length, run_opts.output_file_prefix.data());
        return 0;
    } 
    else {return generate_pair_usage();}
}

