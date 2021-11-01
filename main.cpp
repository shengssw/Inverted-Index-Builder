#include <iostream>
#include <string>
#include <ctime>

#include "PostingCreator.hpp"
#include "InvertedIndex.hpp"

using namespace std;

char* getDataSource(char ** begin, char ** end, const std::string & option)
{
    char ** itr = find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return find(begin, end, option) != end;
}

int main(int argc, char * args[]) {
	//const string SOURCE = "./Data/fulldocs-new.trec";
	string Source = "";
	if (cmdOptionExists(args, args+argc, "-s")) {
		char * filepath = getDataSource(args, args+argc, "-s");
		if (filepath) {
			Source = string(filepath);
			cout << "The path of the source is " << Source << endl;
		} else {
			cout << "Filepath is empty ..." << endl;
			return 0;
		}
	} else {
		cout << "Missing source ..." << endl;
		return 0;
	}


	clock_t begin = clock();
	// Parse documents, write doc to doctable and create intermediate postings, buffer is about 530Mb
	PostingCreator p = PostingCreator(Source, 1 << 29);
	int numPostings = p.createPosting();
	clock_t milestone1 = clock();
	double time1 = double(milestone1-begin)/ CLOCKS_PER_SEC;
	cout << "The total time to create intermediate postings is " << time1 << " seconds" << endl;

	// Sort and Merge postings using unix sort
	string mergedfile = p.SortAndMerge(numPostings);

	// Create invertlist with varbyte compression
	InvertedIndex in = InvertedIndex(mergedfile);
	string result = in.buildInvertedIndex();
	clock_t milestone2 = clock();
	double time3 = double(milestone2-milestone1)/ CLOCKS_PER_SEC;
	cout << "The total time to create invertedindex and lexicon " << time3 << " seconds" << endl;

	clock_t end = clock();
  	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	cout << "The total time is " << elapsed_secs << " seconds" << endl;

	// Remove mergedPositng
	string cmdRemove = "rm -f " + mergedfile;
    system(cmdRemove.c_str());
   
	return 0;
}