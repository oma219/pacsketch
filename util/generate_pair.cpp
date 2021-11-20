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
#include <array>
#include <filesystem>
#include <fstream>
#include <tuple>
#include <set>
#include <vector>

/* Makes sure the output prefix has a valid parent path */
bool is_valid_path(const char* output_prefix) {
    std::__fs::filesystem::path out_prefix_path = output_prefix;
    if (output_prefix[strlen(output_prefix)-1] == '/') {FATAL_WARNING("Need a output prefix, not a directory.");}
    if (std::__fs::filesystem::exists(out_prefix_path.parent_path())) {return true;}
    return false;
}

/* Computes the kmer cardinality of the DNA sequence */
size_t compute_single_cardinality(size_t k, std::string curr_seq) {
    std::set<std::string> kmer_set;
    for (size_t pos = 0; pos <= curr_seq.length()-k; pos++) {
        kmer_set.insert(curr_seq.substr(pos, k));
    }
    return kmer_set.size();
}

/* Takes in command-line arguments, and produces a pair of FASTA files */
void produce_pair(size_t k, size_t seq_length, const char* output_file_prefix) {
    // Validate command-line path variable, and open first FASTA
    if (!is_valid_path(output_file_prefix)) {FATAL_WARNING("output prefix given does not exist.");}

    std::array<std::string, 3> file_exts = {"_1.fa", "_2.fa", "_stats.txt"};
    std::array<std::string, 3> full_output_prefixes = {output_file_prefix + file_exts[0], 
                                                       output_file_prefix + file_exts[1],
                                                       output_file_prefix + file_exts[2]};
    std::array<std::ofstream, 3> out_streams = {std::ofstream(full_output_prefixes[0], std::ofstream::out),
                                                std::ofstream(full_output_prefixes[1], std::ofstream::out),
                                                std::ofstream(full_output_prefixes[2], std::ofstream::out)};

    // Initialize variables to be used for building FASTA files
    std::array<char, 4> alphabet = {'A', 'C', 'G', 'T'};
    srand(time(NULL));

    size_t num_seqs = 2;
    std::set<std::string> union_kmer_set;
    std::vector<size_t> cardinalities;

    // Creates each of FASTA files while computing cardinality of each individuallly
    for (size_t i = 0; i < num_seqs; i++) {
        out_streams[i] << ">seq" << i << std::endl;
        std::string total_seq = "";
        std::string curr_str = "";

        for (size_t j = 0; j < seq_length; j++) {
            if (j % FASTA_WIDTH == 0 && j) {
                out_streams[i] << curr_str.data() << std::endl;
                total_seq += curr_str;
                curr_str = ""; 
            }
            curr_str += alphabet[GET_RANDOM_INDEX(4)];
        }
        if (curr_str.length()) {out_streams[i] << curr_str.data() << std::endl; total_seq += curr_str;}
        
        size_t cardinality = compute_single_cardinality(k, total_seq);
        cardinalities.push_back(cardinality);

        // Inserted current FASTA into union sketch
        for (size_t pos = 0; pos <= total_seq.length()-k; pos++) {
            union_kmer_set.insert(total_seq.substr(pos, k));
        }
    }
    out_streams[0].close();
    out_streams[1].close();

    // Gather stats into stats output file
    cardinalities.push_back(union_kmer_set.size());
    auto jaccard = (cardinalities[0] + cardinalities[1] - cardinalities[2] + 0.0)/(cardinalities[2]);

    out_streams[2] << std::right << std::setw(10) << "|SET(A)|" <<
                      std::right << std::setw(10) << "|SET(B)|" <<
                      std::right << std::setw(15) << "|SET(AUB)|"  <<
                      std::right << std::setw(10) << "J(A,B)" << std::endl;
    out_streams[2] << std::right << std::setw(10) << cardinalities[0] <<
                      std::right << std::setw(10) << cardinalities[1] <<
                      std::right << std::setw(15) << cardinalities[2] <<
                      std::right << std::setw(10) << std::setprecision(4) << jaccard << std::endl;
    out_streams[2].close();
}

void parse_generate_pair_run_options(int argc, char** argv, GeneratePairOptions* opts) {
    /* Parses the command-line arguments */
    for (int c; (c = getopt(argc, argv, "hk:l:o:")) >= 0;){
        switch (c) {
            case 'h': generate_pair_usage(); std::exit(1);
            case 'k': opts->k = std::max(std::atoi(optarg), 0); break;
            case 'o': opts->output_file_prefix.assign(optarg); break;
            case 'l': opts->seq_length = std::max(std::atoi(optarg), 0); break;
            default:  FATAL_WARNING("Use -h option to see valid command-line options");
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

