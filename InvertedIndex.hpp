/*
 *  InvertedIndex.hpp
 *  
 *  Author: Sheng Wang 
 *  Date: 10/03/2021
 */

#ifndef InvertedIndex_hpp
#define InvertedIndex_hpp

#include<vector>
#include<string>
#include<iostream>
#include<fstream>

#include "VarByteCompress.hpp"

using namespace std;

struct lexiconEntry
{
    string term;
    size_t startPos;
    size_t endPos;
    size_t numPosting;

    lexiconEntry(string t, size_t sp, size_t ep, size_t num) {
        term = t;
        startPos = sp;
        endPos = ep;
        numPosting = num;
    }
};


class InvertedIndex {
    private:
        string inputFile;
        vector<lexiconEntry> lexicon;

        // write lexcion to disk
        void writeLexicon();

    public:
        // Constructor 
        InvertedIndex(string infile);

        // build inverted index
        string buildInvertedIndex();

};

#endif