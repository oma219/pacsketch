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
#include <set>
#include <vector>


KSEQ_INIT(gzFile, gzread)

template<typename T>
void print_queue(T q) { 
    // From C++ website for priority queues
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
    elements_in_queue.push_back(MAX_HASH);

    while (kseq_read(ks) >= 0) {
        size_t kmer_length = 11;
        char curr_kmer[kmer_length + 1];
        for (size_t i = 0; i <= ks->seq.l-kmer_length; i++) {
            // Grab and encode kmer, and hash it
            memcpy(curr_kmer, &ks->seq.s[i], kmer_length);
            uint64_t encoded_kmer = encode_string(curr_kmer);
            uint64_t curr_kmer_hash = MurmurHash3(encoded_kmer);

            // Check if it is one of the min-hashes, and it is unique
            if (curr_kmer_hash < max_heap_k.top() && !std::count(elements_in_queue.begin(), elements_in_queue.end(), curr_kmer_hash)) {
                auto removed_value = max_heap_k.top();
                max_heap_k.pop();
                max_heap_k.push(curr_kmer_hash);
                
                // Adds new value, and removes old value
                elements_in_queue.push_back(curr_kmer_hash);
                elements_in_queue.erase(std::remove(elements_in_queue.begin(), elements_in_queue.end(), removed_value), elements_in_queue.end());
            }
        }
    }
} 

void MinHash::buildFromPackets(std::string file_path, size_t k_val) {
    /* Builds the MinHash sketch from a Packet Trace */
    NOT_IMPL("still working on this ...\n");
}

MinHash::MinHash(std::string file_path, size_t k_val, data_type input_type) {
    /* constructor for MinHash class, it builds based on data_type*/
    ref_file.assign(file_path);
    k = k_val;
    file_type = input_type;

    switch(file_type) {
        case FASTA: buildFromFASTA(file_path, k_val); break;
        case PACKET: buildFromPackets(file_path, k_val); break;
        default: FATAL_WARNING("There appears to be a bug in the code in MinHash constructor.\n"); std::exit(1);
    }
}

MinHash::MinHash(size_t k_val, data_type input_type) {
    /* constructor for Minhash - used when unioning two MinHash sketch */
    ref_file.assign("");
    k = k_val;
    file_type = input_type;

    // Initialize the max-heap for the k-smallest hashes
    for (size_t i = 0; i < k_val; i++) {
        max_heap_k.push(MAX_HASH); // Could also use UINT64_MAX
    }
}

uint64_t MinHash::get_cardinality() {
    /* Computes the cardinality based the MinHash sketch */
    uint64_t k_min_hash = max_heap_k.top();
    if (k_min_hash == 0) {k_min_hash = 1000000;} // Just to avoid an error

    uint64_t cardinality = (MAX_HASH/k_min_hash);
    cardinality *= k;
    return cardinality;
}

MinHash MinHash::operator +(MinHash& operand) {
    /* Creates the union minhash from two minhashes */
    MinHash union_sketch (this->k, this->file_type);

    auto add_to_max_heap = [&] (uint64_t curr_kmer_hash) {
        // Check if it is one of the min-hashes
        if (curr_kmer_hash < union_sketch.max_heap_k.top()) {
            union_sketch.max_heap_k.pop();
            union_sketch.max_heap_k.push(curr_kmer_hash);
        }
    };

    while (!this->max_heap_k.empty()) { // Add operand1 hashes
        add_to_max_heap(this->max_heap_k.top());
        this->max_heap_k.pop();
    }

    while (!operand.max_heap_k.empty()) { // Add operand2 hashes
        add_to_max_heap(operand.max_heap_k.top());
        operand.max_heap_k.pop();
    }
    uint64_t union_card = union_sketch.get_cardinality();
    return union_sketch;
}

double MinHash::compute_jaccard(MinHash& op1, MinHash& op2) {
    /* Computes jaccard between two MinHash sketches */
    std::set<uint64_t> hash_set;
    std::vector<uint64_t> op2_vec;

    while (!op1.max_heap_k.empty()) { 
        hash_set.insert(op1.max_heap_k.top()); // Add operand1 hashes to set
        op1.max_heap_k.pop();

        op2_vec.push_back(op2.max_heap_k.top()); // Add operand2 hashes to vector
        op2.max_heap_k.pop();
    }

    // Find number of overlapping hashes
    size_t intersection_count = 0;
    for(auto itr = std::begin(op2_vec); itr != std::end(op2_vec); ++itr) {
        if (hash_set.find(*itr) != hash_set.end()) {intersection_count++;}
    }

    // Add operand2 hashes to set
    std::for_each(std::begin(op2_vec), std::end(op2_vec), [&](uint64_t& hash_value) {hash_set.insert(hash_value);});

    auto jaccard = (intersection_count + 0.0)/(hash_set.size());
    return jaccard;
}
