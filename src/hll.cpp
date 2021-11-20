/*
 * Name: hll.cpp
 * Description: Contains the implementation for the HyperLogLog datastructure.
 * Project: This file is part of pacsketch repo.
 * 
 * Author: Omar Ahmed
 * Date: October 19, 2021
 */

#include <iostream>
#include <hll.h>
#include <kseq.h>
#include <zlib.h>
#include <hash.h>
#include <pacsketch.h>
#include <cmath>

KSEQ_INIT(gzFile, gzread)

HyperLogLog::HyperLogLog(std::string input_path, uint8_t b, data_type file_type) {
    /* Constructor for HLL data-structure */
    
    // Initialize attributes
    ref_file.assign(input_path);
    prefix_bits = b;
    num_registers = std::pow(2, prefix_bits);
    
    total_bytes_allocated = TOTAL_REGISTER_SPACE(num_registers);
    registers = new char[TOTAL_REGISTER_SPACE(num_registers)];
    initialize_registers();

    // Build actual data-structure based on input file
    switch(file_type) {
        case FASTA: buildFromFASTA(ref_file, prefix_bits); break;
        case PACKET: buildFromPackets(ref_file, prefix_bits); break;
        default: FATAL_WARNING("There appears to be a bug in the code in HLL constructor.\n"); std::exit(1);
    }
}

HyperLogLog::~HyperLogLog() {
    /* Deconstructor for HyperLogLog - frees space for HLL */
    delete registers;
}

void HyperLogLog::initialize_registers() {
    /* Initializes all the registers to zero */
    for (size_t i = 0; i < total_bytes_allocated; i++) {
        registers[i] |= 0x00;
    }
}

uint8_t HyperLogLog::grab_register(uint64_t register_num) {
    /* Grabs the value in a specific register, the complexity comes from the
       fact the register can span byte boundaries */
    
    // Determine where in the memory registers we need to check
    uint64_t start_bit = register_num * REGISTER_BITS;
    uint64_t end_bit = (register_num+1) * REGISTER_BITS;

    uint64_t start_byte = start_bit/BITS_PER_BYTE;
    uint64_t end_byte = end_bit/BITS_PER_BYTE;

    // Extract register from memory, approach depends if it spans byte
    // boundary or not.
    if (start_byte != end_byte && (end_bit % BITS_PER_BYTE != 0)) { 

        // I added second condition, because it technically spaces byte
        // boundaries, but it completely fits in one byte, it is just the
        // fact that the end_bit is not-inclusive is why it appears that
        // it spans boundaries.

        uint8_t curr_lsb_byte = registers[start_byte];
        uint8_t curr_msb_byte = registers[end_byte];

        uint8_t num_lsb = 0;
        for (size_t i = start_bit; i < end_bit; i++) {
            if (i % BITS_PER_BYTE == 0) {break;}
            num_lsb++;
        }
        uint8_t num_msb = REGISTER_BITS - num_lsb;
        
        uint8_t grabbed_lsb = GRAB_LSB_REGISTER(curr_lsb_byte, num_lsb);
        uint8_t grabbed_msb = GRAB_MSB_REGISTER(curr_msb_byte, num_msb);
        uint8_t curr_register = COMBINE_LSB_MSB(grabbed_lsb, grabbed_msb, num_msb);
        return curr_register;
        
    } else {
        uint8_t curr_full_byte = registers[start_byte];
        uint8_t bits_to_right = 0;
        size_t i = end_bit;

        while (i % BITS_PER_BYTE != 0) {i++; bits_to_right++;}
        uint8_t bits_to_left = (BITS_PER_BYTE - bits_to_right - REGISTER_BITS);

        // Move to left to drop MSB, and then shift to right to align to left.
        uint8_t curr_register = PUSH_TO_LEFT(curr_full_byte, bits_to_left);
        curr_register = PUSH_TO_RIGHT(curr_register, bits_to_left+bits_to_right);
        return curr_register;
    }
}

void HyperLogLog::clear_register(uint64_t register_num) {
    /* Clear the register to all zeros to be able to set the value */

    // Determine where in the memory registers we need to check
    uint64_t start_bit = register_num * REGISTER_BITS;
    uint64_t end_bit = (register_num+1) * REGISTER_BITS;

    uint64_t start_byte = start_bit/BITS_PER_BYTE;
    uint64_t end_byte = end_bit/BITS_PER_BYTE;

    // Extract register from memory, approach depends if it spans byte
    // boundary or not.
    if (start_byte != end_byte && (end_bit % BITS_PER_BYTE != 0)) { 

        // I added second condition, because it technically spaces byte
        // boundaries, but it completely fits in one byte, it is just the
        // fact that the end_bit is not-inclusive is why it appears that
        // it spans boundaries.

        uint8_t curr_lsb_byte = registers[start_byte];
        uint8_t curr_msb_byte = registers[end_byte];

        uint8_t num_lsb = 0;
        for (size_t i = start_bit; i < end_bit; i++) {
            if (i % BITS_PER_BYTE == 0) {break;}
            num_lsb++;
        }
        uint8_t num_msb = REGISTER_BITS - num_lsb;
        
        uint8_t new_lsb_byte = CLEAR_LSB_REGISTER(curr_lsb_byte, num_lsb);
        uint8_t new_msb_byte = CLEAR_MSB_REGISTER(curr_msb_byte, num_msb);

        registers[start_byte] = new_lsb_byte;
        registers[end_byte] = new_msb_byte;     
    } else {
        uint8_t curr_full_byte = registers[start_byte];
        uint8_t bits_to_right = 0;
        size_t i = end_bit;

        while (i % BITS_PER_BYTE != 0) {i++; bits_to_right++;}
        uint8_t bits_to_left = (BITS_PER_BYTE - bits_to_right - REGISTER_BITS);

        // Clear the register within this byte, and replace it
        uint8_t new_curr_register = CLEAR_WITHIN_REGISTER(curr_full_byte, bits_to_right);
        registers[start_byte] = new_curr_register;
    }

}

void HyperLogLog::set_register(uint64_t register_num, uint8_t lzc) {
    /* Replaces register with value of new largest LZC for that bucket */

    // Determine where in the memory registers we need to check
    uint64_t start_bit = register_num * REGISTER_BITS;
    uint64_t end_bit = (register_num+1) * REGISTER_BITS;

    uint64_t start_byte = start_bit/BITS_PER_BYTE;
    uint64_t end_byte = end_bit/BITS_PER_BYTE;

    if (start_byte != end_byte && (end_bit % BITS_PER_BYTE != 0)) { // register traverses byte boundaries
        // I added second condition, because it technically spaces byte
        // boundaries, but it completely fits in one byte, it is just the
        // fact that the end_bit is not-inclusive is why it appears that
        // it spans boundaries.

        uint8_t num_lsb = 0;
        for (size_t i = start_bit; i < end_bit; i++) {
            if (i % BITS_PER_BYTE == 0) {break;}
            num_lsb++;
        }
        uint8_t num_msb = REGISTER_BITS - num_lsb;

        uint8_t lsb_byte = GENERATE_LSB_BYTE(lzc, num_lsb);
        uint8_t msb_byte = GENERATE_MSB_BYTE(lzc, num_msb);

        registers[start_byte] |= lsb_byte;
        registers[end_byte] |= msb_byte;        
    } else {
        uint8_t bits_to_right = 0;
        size_t i = end_bit;
        while (i % BITS_PER_BYTE != 0) {i++; bits_to_right++;}

        uint8_t byte_val = PUSH_TO_LEFT(lzc, bits_to_right);
        registers[start_byte] |= byte_val;
    }
}

void HyperLogLog::buildFromFASTA(std::string input_path, uint8_t m) {
    /* Builds the HLL from a FASTA file */
    gzFile fp = gzopen(input_path.data(), "r"); 
    kseq_t* ks = kseq_init(fp);
    
    
    while (kseq_read(ks) >= 0) {
        size_t kmer_length = 11;
        char curr_kmer[kmer_length + 1];
        for (size_t i = 0; i <= ks->seq.l-kmer_length; i++) {
            // Grab and encode kmer, and hash it
            memcpy(curr_kmer, &ks->seq.s[i], kmer_length);
            uint64_t encoded_kmer = encode_string(curr_kmer);
            uint64_t curr_kmer_hash = MurmurHash3(encoded_kmer);

            // Parse the hash value to get register num, and lead zero count (lzc)
            uint64_t mask = GRAB_REGISTER_MASK(prefix_bits);
            uint64_t register_num = GRAB_REGISTER_NUM(mask, curr_kmer_hash, prefix_bits);

            uint64_t remaining_mask = ~mask;
            uint64_t remaining_bits = GRAB_REMAINING_BITS(curr_kmer_hash, remaining_mask, prefix_bits);
            uint8_t  lzc = DETERMINE_LZC(remaining_bits);

            // Update register number if the current LZC is larger than register
            uint8_t register_value = grab_register(register_num);
            if (lzc > register_value) {
                clear_register(register_num);
                set_register(register_num, lzc);
            }
        }
    }
}

uint64_t HyperLogLog::compute_cardinality() {
    /* Computes cardinality of HLL sketch and returns it */

    // Start by calculating the Z value from the HLL paper
    double z = 0.0;
    size_t num_zero = 0;
    for (size_t i = 0; i < num_registers; i++) {
        uint8_t curr_lzc = grab_register(i);
        if (curr_lzc == 0) {num_zero++;}
        z += (1.0/std::pow(2, curr_lzc));
    }

    // Determine the bias factor (alpha) based on m
    size_t m = std::pow(2, prefix_bits);
    double alpha = 0.0;
    if (m <= 16) {alpha = 0.673;}
    else if (m == 32) {alpha = 0.697;}
    else if (m == 64) {alpha = 0.709;}
    else {alpha = (0.7213/(1 + (1.079/m)));}

    // Put together the final factors
    size_t cardinality = alpha * m * m * (1/z);

    // Now, lets check if any of the corrections apply ...
    if (cardinality <= 2.5 * m && num_zero) {cardinality = m * std::log2(m/num_zero);}
    if (cardinality > (1/30.0) * std::pow(2, 32)) {cardinality = -1 * std::pow(2, 32) * std::log2(1 - (cardinality/std::pow(2, 32)));}
    return cardinality;
}

void HyperLogLog::buildFromPackets(std::string input_path, uint8_t m) {
    /* Builds the HLL from a Packet Data */
    NOT_IMPL("still working on this ...");
}

/*
int main (int argc, char** argv) {

    std::string input_file = "/Users/omarahmed/Downloads/test_fa/test.fa";
    data_type curr_data = FASTA;

    HyperLogLog curr_sketch (input_file, 11, curr_data);
    std::cout << curr_sketch.compute_cardinality() << std::endl;
}
*/

