#ifndef CONLLREADER_H
#define CONLLREADER_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include "Instance.h"

using namespace std;

class CONLLReader{

private:
	ifstream input_rd;

public:
	CONLLReader(string filename);
	CONLLReader();
	~CONLLReader();
	int InitFile(string filename);
	void CloseFile();
	void GetNext(Instance &);
};

#endif
