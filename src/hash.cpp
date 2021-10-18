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

uint64_t MurmurHash3(uint64_t key) {
  uint64_t k = (uint64_t) key;
  k ^= k >> 33;
  k *= 0xff51afd7ed558ccd;
  k ^= k >> 33;
  k *= 0xc4ceb9fe1a85ec53;
  k ^= k >> 33;
  return k;
}


