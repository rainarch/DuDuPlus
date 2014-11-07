#ifndef BASEWRITER_H
#define BASEWRITER_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include "Instance.h"
#include "FeatureVector.h"
using namespace std;

class BaseWriter{

protected:
	ofstream output_wd;
	string filename;
    string format;//CONLL or SENT
    string type;//SEG/POS/DP
    bool   isstdIO;//stander IO
 
public:
	BaseWriter(string &filename, string &format, string &type);
	BaseWriter();
	~BaseWriter();
	void Init();
	void CloseFile();
	bool IsOpen();
	int InitFile(string &filename);
	void WriteBuf(string s);
	void OutputInstance(Instance &instance, vector<ResultItem > &d, int K);
	void OutputDP(Instance &instance, vector<ResultItem > &d, int K);
	void OutputPOS(Instance &instance, vector<ResultItem > &d, int K);
	void OutputSEG(Instance &instance, vector<ResultItem > &d, int K);
	void String2DPCONLL(Instance &instance, string &tag, string &sc);
	void String2POSCONLL(Instance &instance, string &tag, string &sc);
	void String2SEGCONLL(Instance &instance, string &tag, string &sc);
	void String2POSSent(Instance &instance, string &tag, string &sc);
    void String2SEGSent(Instance &instance, string &tag, string &sc);
};

#endif
