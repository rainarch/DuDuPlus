#ifndef POSWRITER_H
#define POSWRITER_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include "Instance.h"
#include "FeatureVector.h"
using namespace std;

class POSWriter{

private:
	ofstream output_wd;

public:
	POSWriter(string filename);
	POSWriter();
	~POSWriter();
	void CloseFile();
	bool IsOpen();
	int InitFile(string filename);
	void OutputTags(Instance &instance, vector<ResultItem > &d);

	void String2POS(Instance &instance, string &tag, string &sc);
};

#endif
