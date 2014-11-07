#ifndef CONLLWRITER_H
#define CONLLWRITER_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include "Instance.h"
#include "FeatureVector.h"
using namespace std;

class CONLLWriter{

private:
	ofstream output_wd;

public:
	CONLLWriter(string filename);
	CONLLWriter();
	~CONLLWriter();
	void CloseFile();
	bool IsOpen();
	int InitFile(string filename);
	void OutputParseTree(Instance &instance, vector<ResultItem > &d, int K);
	void String2CONLL(Instance &instance, string &tag, string &sc);
};

#endif
