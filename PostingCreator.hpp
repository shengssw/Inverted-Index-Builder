/*
 *  PostingCreator.hpp
 *  
 *  Author: Sheng Wang 
 *  Date: 10/02/2021
 */

#ifndef PostingCreator_hpp
#define PostingCreator_hpp

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include <stack>
#include <string.h>
#include <algorithm>
#include <locale>
#include <algorithm>
#include <cctype>
#include <string>
#include <utility>  

#include "DocTable.hpp"

using namespace std;

struct Posting {  
    string term;
    size_t docId;
    size_t frequency;

    Posting(string s, int id, int f){
        term = s;
        docId = id;
        frequency = f;
    }
};

class PostingCreator {
    private: 
        /* Private Data fields */
        string sourceName;
        size_t bufferSize;
        size_t bufferPosition;
        char* buffer;
        int fileNum;
        static const string DELIMITERS;

        // Parse readin chunck
        void parseDocument(vector<char>::iterator begin, vector<char>::iterator end, int docId);

        // See if a char is delimiter
        inline bool isDelimiter(char c);

        // write out the intermediate positng
        void flushBuffer();

            

    public:
        // Constructor: find the source dirctory and initialize the buffer 
        PostingCreator(const string& source, size_t size);

        // Destructor 
        ~PostingCreator();

        // Parse documents, generate intermidiate postings and write postings to buffer
        int createPosting();

        // Sort and merge posings files use Unix Sort, return the final merged filename
        string SortAndMerge(int numfiles);

};

#endif