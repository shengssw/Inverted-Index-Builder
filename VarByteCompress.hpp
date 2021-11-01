/*
 *  VarByteCompress.hpp
 *  
 *  Author: Sheng Wang 
 *  Date: 10/03/2021
 */

#ifndef VarByteCompress_hpp
#define VarByteCompress_hpp

#include <vector>
#include <bitset>
#include <cstdint>

using namespace std;

// Functions for varbyte compress of integer

// Write a number < 256 to a byte
void writeByte(vector<unsigned char> &bytes, size_t n);

// Encode a number
vector<unsigned char> encodeNum(size_t num);

// Encode an array of numbers
vector<unsigned char> encode(vector<size_t> &numbers);

// Decode numbers
vector<size_t> decode(vector<unsigned char> &compressedArray);

#endif

