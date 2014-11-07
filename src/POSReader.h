#ifndef POSREADER_H
#define POSREADER_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include "Instance.h"

using namespace std;

class POSReader{

private:
	ifstream input_rd;

public:
	POSReader(string filename);
	POSReader();
	~POSReader();
	int InitFile(string filename);
	void CloseFile();
	void GetNext(Instance &);
	void GetNextSent(Instance &);
	void WordType(string &w0, string &type);
};

#endif
