/*
 *  VarByteCompress.cpp
 *  
 *  Author: Sheng Wang 
 *  Date: 10/03/2021
 */

#include "VarByteCompress.hpp"

// Write a number < 256 to a byte
void writeByte(vector<unsigned char> &bytes, size_t n) {
    uint8_t byte = n;
    bytes.push_back(byte);
}

// Encode a number
vector<unsigned char> encodeNum(size_t num) {
    vector<unsigned char> bytes;
    while (num > 127) {
        writeByte( bytes, num & 127);
        num = num >> 7;
    }
    writeByte( bytes, 128 + num);
    return bytes;
}

// Encode an array of numbers
vector<unsigned char> encode(vector<size_t> &array) {
    vector<unsigned char> compressedArray;
    for (size_t num : array) {
        vector<unsigned char> bytes = encodeNum(num);
        for(auto it = bytes.begin(); it != bytes.end(); it++) {
            compressedArray.push_back(*it);
        }
    }
    return compressedArray;
}

// Decode numbers
vector<size_t> decode(vector<unsigned char> &compressedArray) {
    vector<size_t> array;
    size_t val = 0;
    size_t shift = 0;
    for (unsigned char byte : compressedArray) {
        uint8_t num = (uint8_t)byte;
        if( num < 128) {
            val = val + (num << shift);
            shift = shift + 7;
        } else {
            val = val + ((num-128) << shift);
            array.push_back(val);
            val = 0;
            shift = 0;
        }
    }
    return array;
}