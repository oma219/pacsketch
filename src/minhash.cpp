/*
 * Name: minhash.cpp
 * Description: Contains the implementation of the MinHash data-structure.
 * Project: pacsketch project.
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
void print_queue(T q) { // NB: pass by value so the print uses a copy
    while(!q.empty()) {
        std::cout << q.top() << ' ';
        q.pop();
    }
    std::cout << '\n';
}

MinHash::MinHash(const char* file_path, size_t k_val, input_type file_type = FASTA) {
    /* Constructs the MinHash data-structure for the scenario where input is a FASTA file */ 
    gzFile fp = gzopen(file_path, "r"); 
    kseq_t* ks = kseq_init(fp);

    // Initialize the max-heap for the k-smallest hashes
    for (size_t i = 0; i < k_val; i++) {
        max_heap_k.push(UINT64_MAX);
    }
    std::cout << max_heap_k.top() << std::endl;
    print_queue(max_heap_k);

    while (kseq_read(ks) >= 0) {
        size_t kmer_length = 21;
        char curr_kmer[21];
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
    print_queue(max_heap_k);

    uint64_t cardinality = (UINT64_MAX/max_heap_k.top());
    cardinality *= k_val;
    std::cout << cardinality << std::endl;
}


int main(int argc, char** argv) {
    input_type file_type = FASTA;

    std::string input_file = "/Users/omarahmed/Downloads/test_fa/test.fa";

    if (!is_file(input_file.data())) {THROW_EXCEPTION(std::runtime_error("The following path provided for MinHash skectch is not valid: " + input_file));}
    MinHash(input_file.c_str(), 400, file_type);
}
