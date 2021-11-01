/*
 * DocTable.hpp
 * 
 * Author: Sheng Wang
 * Date: 10/03/2021
 */

#include "DocTable.hpp"

DocTable::DocTable() {
    docNum = 0;
}

void DocTable::addEntry(int docId, string &link, size_t size) {
    docEntry e = docEntry(docId, link, size);
    table.push_back(e);
}

void DocTable::writeTable() {
    string output = "docTable";

    ofstream myfile(output, ios::binary);
    if(myfile.is_open()) {
        for(auto it = table.begin(); it != table.end(); it++) {
            myfile << it->docId << ' ' << it->url << ' ' << it->size << '\n';
        }
    } else {
        cout << "Error: Can Not Open docTable" << endl;
    }
    myfile.close();
}