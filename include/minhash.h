/*
 * Name: minhash.h
 * Description: Header file for minhash.cpp
 * Project: pacsketch project.
 * 
 * Author: Omar Ahmed
 * Date: October 17, 2021
 */

#include <queue>
#include <vector>
#include <limits>
#include <stdint.h>

#define MAX_HASH std::numeric_limits<uint64_t>::max()

enum input_type {FASTA};

class MinHash {

private:
    const char* ref_file; // path to input data
    input_type file_type; // tells us how to parse input
    std::priority_queue<uint64_t, std::vector<uint64_t>> max_heap_k; // holds lowest k values, in max heap
    size_t k; // number of items kept

public:
    MinHash(const char* file_path, size_t k_val, input_type file_type);
};