#ifndef SENTREADER_H
#define SENTREADER_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include "Instance.h"

using namespace std;

class SentReader{

private:
	ifstream input_rd;

public:
	SentReader(string filename);
	SentReader();
	~SentReader();
	int InitFile(string filename);
	void CloseFile();
	void GetNext(Instance &);
	void WordType(string &w0, string &type);
};

#endif
