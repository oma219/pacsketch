/*
 * Name: hll.h
 * Description: Header file for hll.cpp
 * Project: This file is part of pacsketch repo.
 * 
 * Author: Omar Ahmed
 * Date: October 19, 2021
 */

#ifndef _HLL_H
#define _HLL_H

#include <math.h>
#include <cstring>
#include <stdint.h>
#include <pacsketch.h>

#define BITS_PER_BYTE 8
#define HASH_SIZE 64
#define REGISTER_BITS int(std::ceil(std::log2(HASH_SIZE)))

#define TOTAL_REGISTER_SPACE(x) int(std::ceil((x * REGISTER_BITS)/BITS_PER_BYTE))

#define GRAB_REGISTER_MASK(x) uint64_t (std::pow(2, x) - 1) << (HASH_SIZE-x)
#define GRAB_REGISTER_NUM(x, y, z) (x & y) >> (HASH_SIZE - z)

#define GRAB_REMAINING_BITS(x, y, z) (x & y) << (z)
#define DETERMINE_LZC(x)  ({ \ 
                             uint64_t probe = ((uint64_t) 0x1) << (HASH_SIZE - 1); \
                             uint8_t lzc = 1; \
                             for (size_t i = 0; i < HASH_SIZE; i++) { \
                                if (probe & x) {break;} \
                                probe >>= 1; lzc += 1;\
                             } \
                            lzc; \
                            }) 

#define GENERATE_LSB_BYTE(x, y) (x >> (REGISTER_BITS - y)) 
#define GENERATE_MSB_BYTE(x, y) (x << (BITS_PER_BYTE - y)) 

#define PUSH_TO_LEFT(x, y) (x << y)
#define PUSH_TO_RIGHT(x, y) (x >> y)

#define GRAB_LSB_REGISTER(x, y) (uint8_t)(x & (uint16_t) (std::pow(2, y) - 1))
#define GRAB_MSB_REGISTER(x, y) x >> (BITS_PER_BYTE - y)
#define COMBINE_LSB_MSB(x, y, z) (uint8_t) ((((uint16_t)x) << z) | y)

#define CLEAR_LSB_REGISTER(x, y) ((x >> y) << y)
#define CLEAR_MSB_REGISTER(x, y)  x & ~((uint16_t)(std::pow(2, y)-1) << (BITS_PER_BYTE-y))
#define CLEAR_WITHIN_REGISTER(x, y) x & ~((uint8_t)(std::pow(2, REGISTER_BITS) - 1) << y)


class HyperLogLog {

private:
    std::string ref_file; // path to input data
    uint8_t prefix_bits = 0; // number of bits to use for bucket determination
    uint64_t num_registers = 0; // number of registers in HLL
    uint64_t total_bytes_allocated = 0; // actual bytes allocated for registers
    char* registers; // pointers to dynamically allocated memory of registers

public:
    HyperLogLog(std::string input_path, uint8_t b, data_type file_type);
    ~HyperLogLog();
    uint64_t compute_cardinality();

private:
    void buildFromFASTA(std::string input_path, uint8_t m);
    void buildFromPackets(std::string input_path, uint8_t m);
    void initialize_registers();
    uint8_t grab_register(uint64_t register_num);
    void clear_register(uint64_t register_num);
    void set_register(uint64_t register_num, uint8_t new_val);

}; // end of HLL class


#endif /* end of _HLL_H */