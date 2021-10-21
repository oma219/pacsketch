/* 
 * Name: hash.cpp
 * Description: Contains the code to compute hash values, it is based on the 
 *              MurmurHash3 code (https://github.com/aappleby/smhasher/blob/master/src/MurmurHash3.cpp)
 *              and it uses the function from 
 *              Kraken2 repo (https://github.com/DerrickWood/kraken2/blob/master/src/kv_store.h)
 * Project: This file is part of pacsketch repo.
 * 
 * Author: Omar Ahmed
 * Date: October 17, 2021
 */ 

#include <hash.h>
#include <iostream>
#include <cstring>
#include <map>
#include <stdint.h>


uint64_t MurmurHash3(uint64_t key) {
  uint64_t k = (uint64_t) key;
  k ^= k >> 33;
  k *= 0xff51afd7ed558ccd;
  k ^= k >> 33;
  k *= 0xc4ceb9fe1a85ec53;
  k ^= k >> 33;
  return k;
}

uint64_t encode_string(const char* input_str) {
  /* Takes a string (assuming DNA) and encodes it to a uint64_t */
  std::map<char, uint64_t> encode_dna = {{'A', 0x00}, {'C', 0x01},{'G', 0x02},{'T', 0x03}};
  uint64_t enc_str = 0x0000000000000000;
  size_t i = 0;

  for (; i < strlen(input_str); i++) {
    enc_str = enc_str | (encode_dna[input_str[i]] << (2 * (strlen(input_str) - i - 1)));
  }
  return enc_str;
}




