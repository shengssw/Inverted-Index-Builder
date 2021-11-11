/*
 * DocTable.hpp
 * 
 * Author: Sheng Wang
 * Date: 10/03/2021
 */

#ifndef DocTable_hpp
#define DocTable_hpp

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

struct docEntry {
    size_t docId;
    string url;
    size_t size;
    size_t offset;

    docEntry(int id, string link, size_t s, size_t o) {
        docId = id;
        url = link;
        size = s;
        offset = o;
    }
};

class DocTable {
    private:
        /* Private data fields */
        int docNum;
        vector<docEntry> table;

    public:
        // Constructor: create a table
        DocTable();

        // add a doc to the table
        void addEntry( int docId, string &link, size_t size, size_t offset );

        // write table file to the disk
        void writeTable();  

};
#endif 