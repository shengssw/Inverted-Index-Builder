/*
 * PostingCreator.cpp
 * 
 * Author: Sheng Wang
 * Date: 10/03/2021
 */
#include "PostingCreator.hpp"

// Constant
const string PostingCreator::DELIMITERS = ".,:; &()^#@“<>+-?!'/·*©\"=\n";

// Constructor & Destructor
PostingCreator::PostingCreator(const string& source, size_t size) {
    sourceName = source;
    bufferSize = size;
    bufferPosition = 0;
    buffer = new char[bufferSize];
    fileNum = 0;
}

PostingCreator::~PostingCreator() {
    delete[] buffer;
}

// Create Posting from the source
int PostingCreator::createPosting() {
    string line;
    int docId = 0;
    const string tokenLeft = "<TEXT>";
    const string tokenRight = "</TEXT>";
    stack<int> docTracker;
    vector<Posting> Postings;
    bool getUrl = false;
    string currentdocUrl;
    size_t docSize = 0;
    DocTable table = DocTable();

    ifstream myfile(sourceName, ios::in|ios::binary);
    if (myfile.is_open()) {

        while (getline(myfile, line)) {
         
            if(!line.compare(tokenLeft) && docTracker.empty()) {
                // Get the begining of the doc
                docTracker.push(1);
                docId ++;
                continue;
                //cout << "parsing document " << docId << "........" << endl;
            } 

            if(!line.compare(tokenRight) && !docTracker.empty()) {
                // Get the end of the doc text
                docTracker.pop();
                table.addEntry(docId, currentdocUrl, docSize);
                docSize = 0;
                getUrl = false;   

                // sort term
                sort(Postings.begin(), Postings.end(),
                []( Posting& lhs, Posting& rhs) {
                    return lhs.term < rhs.term;
                });

                // Merge term with the same term
                auto comp = [] ( Posting& lhs, Posting& rhs ) {
                    if(lhs.term == rhs.term) {
                        lhs.frequency += 1;
                    }
                    return lhs.term == rhs.term;};
                auto last = unique(Postings.begin(), Postings.end(), comp);
                Postings.erase(last, Postings.end());
            
                // write to buffer
                for(auto it = Postings.begin(); it != Postings.end(); it++) {
                    string posting = it->term + ' ' + to_string(it->docId) + ' ' + to_string(it->frequency) + '\n';

                    if (posting.size()+bufferPosition > bufferSize) {
                        flushBuffer();
                    }

                    int byteAdded = sprintf(buffer+bufferPosition, "%s", posting.c_str());
                    if ( byteAdded != posting.size()) {
                        cout << "Failed to add posing to buffer!" << endl;
                    } else { 
                        bufferPosition += byteAdded; 
                    }
                }

                Postings.clear();
                //cout << "After clean, the size of Postings is " << Postings.size() << endl;
            }

            if(!docTracker.empty()) {
                // Update the size of the doc
                docSize += line.size();

                // Parse docu line by line
                if (!getUrl) {
                    // Check if the line is site url
                    currentdocUrl = line;
                    getUrl = true;
                } else {
                    // Create intermediate positng
                    //istringstream iss(line);
                    auto it = line.begin();
                    string word;
                    char c;
                    bool allChar = true;
                    bool isDigit = false;
                    locale loc;

                    while (it != line.end())
                    {
                        c = *it;
                        if(!isDelimiter(c)){
                            isDigit = isdigit(c);
                            allChar =isalpha(c,loc);
                            if(isDigit || allChar) {
                                // only push number or english char
                                word.push_back(c);
                            }
                        } else {
                            if(!word.empty()){
                                if(allChar) {
                                    // Lower the character
                                    transform(word.begin(), word.end(), word.begin(),[](char c){ return tolower(c); });
                                }
                                // Add intermediate posting
                                Posting p = Posting(word, docId, 1);
                                Postings.push_back(p);
                                word.clear();
                            }
                        }
                        it++;
                    }                    
                }
            } 

        }

        // Flush the rest of data to a file
        if(bufferPosition != 0)
            flushBuffer();

        myfile.close();

    } else {
        cout << "Error opening source file\n";
    }

    table.writeTable();
    return fileNum;
}

// See if input char is a delimiter
inline bool PostingCreator::isDelimiter(char c) {
    for (int i = 0; i < DELIMITERS.size(); i++) {
        if(c == DELIMITERS[i]){
            return true;
        }
    }
    return false;
}


// Write out intermediate positng and reset the buffer
void PostingCreator::flushBuffer() {
    fileNum++;
    cout << "Flushing Buffer and write out to file" << fileNum << " ......." << endl;

    ofstream myfile ("Postings"+to_string(fileNum), ofstream::binary);
    myfile.write(&buffer[0], bufferPosition);
    myfile.close();

    memset(buffer, 0, bufferSize);
    bufferPosition = 0;
}

// Sort and Merge Postings files using unix sort and unix merge command
string PostingCreator::SortAndMerge(int numfiles) {
    vector<string> filesToSort;
    string initialFiles = "";
    for (int i = 0; i < numfiles; i++) {
        int index = i+1;
        filesToSort.push_back("Postings"+to_string(index));
        initialFiles += ("Postings"+to_string(index)+' ');
    }
    string cmd = "sort -k1,1 -k2,2n "; // Sort by term, breaking ties by docId
    string filestoMerge = "";
    int index = 0;
    for (auto it = filesToSort.begin(); it != filesToSort.end(); it++) {
        cout << "Start Unix Sort " << *it << " ......." << endl;
        string fullcmd = cmd + *it + " -o Sorted"+to_string(++index);
        system(fullcmd.c_str());
        cout << "Finish Unix Sort " << *it << " ......." <<endl;
        filestoMerge += ("Sorted"+to_string(index)+' ');
        // Remove original posting file
        string cmdRemove = "rm -f " + *it;
        system(cmdRemove.c_str());
    }

    // Merge presorted files to one file
    //cout << "The files to get merge are " << filestoMerge << endl;

    cout << "Start merging files ...." << endl;
    string fullcmdmerge = "sort -m " + filestoMerge + "-o mergedPostings";
    system(fullcmdmerge.c_str());
    cout << "Finish merging files ...." << endl; 

    // Remove all the intermediate Postings
    string cmdRemoveSorted = "rm -f " + filestoMerge;
    system(cmdRemoveSorted.c_str());
    cout << "Finish removing Sorted Postings ........." << endl; 

    return "mergedPostings";
}