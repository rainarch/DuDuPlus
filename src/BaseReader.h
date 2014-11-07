#ifndef BaseREADER_H
#define BaseREADER_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include "Instance.h"

using namespace std;

class BaseReader{

protected:
	ifstream input_rd;
	string filename;
	string format;//CONLL or SENT
	string type;//SEG/POS/DP
	bool   isstdIO;//stander IO
	int ReadNextLine(string &);
public:
	BaseReader(string &filename, string &format, string &type);
	BaseReader();
	~BaseReader();
	void Init();
	void SetIO(string filename);
	void SetFormat(string format);
	void SetType(string type);
	int InitFile(string &filename);
	void CloseFile();
	void GetNext(Instance &);
	void GetNextCONLL(Instance &);
	void GetNextSent(Instance &);
	void GenPOSInstance(vector<string> &lines, Instance &inst);
	void GenSEGInstance(vector<string> &lines, Instance &inst);
	void GenDPInstance(vector<string> &lines, Instance &inst);
	void GenPOSInstance(string &line, Instance &inst);
	void GenSEGInstance(string &line, Instance &inst);
	void GenDPInstance(string &line, Instance &inst);
};

#endif
