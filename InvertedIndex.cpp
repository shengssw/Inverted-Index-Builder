/*
 *  InvertedIndex.cpp
 *  
 *  Author: Sheng Wang 
 *  Date: 10/03/2021
 */

#include "InvertedIndex.hpp"

// Constructor
InvertedIndex::InvertedIndex(string infile) {
    inputFile = infile;
}

// Build the InvertedIndex
string InvertedIndex::buildInvertedIndex() {
    string outfile = "InvertedIndex";

    // Open merged Posting
    ifstream myfile(inputFile);
    if(myfile.is_open()) {
        //cout << "Start reading mergedPosting file ....." << endl;
        // Open output file to wirte
        ofstream out(outfile, ios::binary);
        size_t startPos = 0;
        size_t endPos = 0;
        size_t numPosting = 0;
        
        // Set up array and get first term
        vector<size_t> allPostingIds;
        vector<size_t> allPostingFrequency;
        string term;
        size_t docId;
        size_t frequency;
        myfile >> term >> docId >> frequency;
        allPostingIds.push_back(docId);
        allPostingFrequency.push_back(frequency);
        numPosting ++;
       
        //Loop to get same terms
        string currTerm;
        size_t currId;
        size_t currFreq;

        while(myfile>> currTerm >> currId >> currFreq) {

            if (currTerm == term) {
                numPosting ++;
                allPostingIds.push_back(currId);
                allPostingFrequency.push_back(frequency);
            } else {
                // create inverted list for the previous term
                vector<unsigned char> invertedlist;
                vector<unsigned char> blocks;
                // Set up metadata, we group 64 docid + 64 frequency as a block
                vector<size_t> metaBlockSize;
                vector<size_t> metaLastId;
                
                size_t counter = 0;
                size_t numId = 0;
                vector<size_t> idBlock;
                vector<size_t> frequencyBlock;

                while (counter < allPostingIds.size()) {
                    idBlock.push_back(allPostingIds[counter]);
                    frequencyBlock.push_back(allPostingFrequency[counter]);
                    numId ++;
                    if( numId == 64 || counter == allPostingIds.size()-1) {
                        // Store doc id as difference
                        for (auto rit = idBlock.rbegin(); rit != idBlock.rend(); rit++ ) {
                            if (*rit != idBlock.front()) {
                                *rit = *rit - *(rit+1); 
                            } else {
                                if (!metaLastId.empty()) {
                                    *rit = *rit - metaLastId.back();
                                }
                            }
                        }

                        // Compress
                        vector<unsigned char> encodeIds = encode(idBlock);
                        vector<unsigned char> encodeFreqs = encode(frequencyBlock);

                        // Update metadata
                        metaLastId.push_back(allPostingIds[counter]);
                        metaBlockSize.push_back(encodeIds.size()+encodeFreqs.size());

                        // Add blocks
                        blocks.reserve(blocks.size()+encodeIds.size()+encodeFreqs.size());
                        blocks.insert(blocks.end(), encodeIds.begin(), encodeIds.end());
                        blocks.insert(blocks.end(), encodeFreqs.begin(), encodeFreqs.end());

                        // Reset vector for next block
                        idBlock.clear();
                        frequencyBlock.clear();
                        numId = 0;
                    }
                    counter++;
                }

                // Compress metadata
                //cout << "The total posting for current term is " << numPosting << endl;
                //cout << "The total number of blocks for this term is " << metaBlockSize.size() << endl;
                vector<unsigned char> encodeBlockSize = encode(metaBlockSize);
                vector<unsigned char> encodeLastIds = encode(metaLastId);

                // Add metadata to the front of the list
                invertedlist.reserve(invertedlist.size()+encodeBlockSize.size()+ encodeLastIds.size());
                invertedlist.insert(invertedlist.end(), encodeBlockSize.begin(), encodeBlockSize.end());
                invertedlist.insert(invertedlist.end(), encodeLastIds.begin(), encodeLastIds.end());

                // Append blocks to the list
                invertedlist.reserve(invertedlist.size() + blocks.size());
                invertedlist.insert(invertedlist.end(), blocks.begin(), blocks.end());

                // Write invertlist to file
                out.write((const char*)&invertedlist[0], invertedlist.size());
                endPos = out.tellp();

                // Update lexicon with the term
                //cout << "The startpos is " << startPos << " and the endpos is " << endPos << endl; 
                lexiconEntry l = lexiconEntry(term, startPos, endPos, numPosting);
                lexicon.push_back(l);

                // Reset for next term
                numPosting = 1;
                startPos = endPos + 1;
                term = currTerm;
                allPostingIds.clear();
                allPostingFrequency.clear();
                allPostingIds.push_back(currId);
                allPostingFrequency.push_back(currFreq);         
            }
        }

        out.close();
        myfile.close();
        writeLexicon();

    } else {
        cout << "Error: Can Not Open mergedPostings" << endl;
    }
    myfile.close();
    return outfile;
}

// Write lexicon to disk
void InvertedIndex::writeLexicon() {
    string output = "Lexicon";

    ofstream myfile(output, ios::binary);
    if (myfile.is_open()) {
        for (auto it = lexicon.begin(); it != lexicon.end(); it++) {
            myfile << it->term << ' ' << it->startPos << ' ' << it->endPos << ' ' << it->numPosting << '\n';
        }
    } else {
        cout << "Error: Can Not Open Lexicon" << endl;
    }
    myfile.close();
}