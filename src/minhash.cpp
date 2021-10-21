/*
 * Name: minhash.cpp
 * Description: Contains the implementation of the MinHash data-structure.
 * Project: This file is part of pacsketch repo.
 * 
 * Author: Omar Ahmed
 * Date: October 17, 2021
 */

#include <minhash.h>
#include <iostream>
#include <kseq.h>
#include <zlib.h>
#include <hash.h>
#include <pacsketch.h>


KSEQ_INIT(gzFile, gzread)

template<typename T>
void print_queue(T q) { 
    // From c++ website for priority queues
    while(!q.empty()) {
        std::cout << q.top() << ' ';
        q.pop();
    }
    std::cout << '\n';
}

void MinHash::buildFromFASTA(std::string file_path, size_t k_val) {
    /* Constructs the MinHash data-structure for the scenario where input is a FASTA file */
    gzFile fp = gzopen(file_path.data(), "r"); 
    kseq_t* ks = kseq_init(fp);

    // Initialize the max-heap for the k-smallest hashes
    for (size_t i = 0; i < k_val; i++) {
        max_heap_k.push(MAX_HASH); // Could also use UINT64_MAX
    }

    while (kseq_read(ks) >= 0) {
        size_t kmer_length = 21;
        char curr_kmer[kmer_length + 1];
        for (size_t i = 0; i <= ks->seq.l-kmer_length; i++) {
            // Grab and encode kmer, and hash it
            memcpy(curr_kmer, &ks->seq.s[i], kmer_length);
            uint64_t encoded_kmer = encode_string(curr_kmer);
            uint64_t curr_kmer_hash = MurmurHash3(encoded_kmer);

            // Check if it is one of the min-hashes
            if (curr_kmer_hash < max_heap_k.top()) {
                max_heap_k.pop();
                max_heap_k.push(curr_kmer_hash);
            }
        }
    }
} 

void MinHash::buildFromPackets(std::string file_path, size_t k_val) {
    /* Builds the MinHash sketch from a Packet Trace */
    NOT_IMPL("still working on this ...\n");
}

MinHash::MinHash(std::string file_path, size_t k_val, data_type file_type) {
    /* constructor for MinHash class, it builds based on data_type*/
    ref_file.assign(file_path);
    k = k_val;

    switch(file_type) {
        case FASTA: buildFromFASTA(file_path, k_val); break;
        case PACKET: buildFromPackets(file_path, k_val); break;
        default: FATAL_WARNING("There appears to be a bug in the code in MinHash constructor.\n"); std::exit(1);
    }
}

uint64_t MinHash::get_cardinality() {
    /* Computes the cardinality based the MinHash sketch */
    uint64_t cardinality = (MAX_HASH/max_heap_k.top());
    cardinality *= k;
    return cardinality;
}
