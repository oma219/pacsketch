/*
 * Name: minhash.h
 * Description: Header file for minhash.cpp
 * Project: This file is part of pacsketch repo.
 * 
 * Author: Omar Ahmed
 * Date: October 17, 2021
 */

#ifndef _MINHASH_H
#define _MINHASH_H

#include <queue>
#include <vector>
#include <limits>
#include <stdint.h>
#include <pacsketch.h>

#define MAX_HASH std::numeric_limits<uint64_t>::max()

class MinHash {

private:
    std::string ref_file; // path to input data
    data_type file_type; // tells us how to parse input
    std::priority_queue<uint64_t, std::vector<uint64_t>> max_heap_k; // holds lowest k values, in max heap
    size_t k; // number of items kept

public:
    MinHash(std::string file_path, size_t k_val, data_type file_type);
    uint64_t get_cardinality();

private:
    void buildFromFASTA(std::string file_path, size_t k_val);
    void buildFromPackets(std::string file_path, size_t k_val);

}; // end of MinHash class

#endif /* end of _MINHASH_H */