#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include "MyHashTable.h"
#include "SymOptions.h"
#include "FeatureVector.h"
#include "Pipe.h"
#include "Parameters.h"
#include "Decoder.h"
#include "DecoderO2.h"
#include "BaseWriter.h"
#include "MyTool.h"
using namespace std;

class Parser{

public:
	Pipe *ppipe;
	SymOptions *poptions;
	Decoder decoder;
	DecoderO2 decoder2;
	Parameters params;
public:
	Parser(Pipe *, SymOptions *);
	void Train(string &trainfile);
	void Test(string &testfile);
	void TestOneSent(Instance &instance, vector<ResultItem > &d);
	void SaveModel(string &model_name);
	void SaveModelNew(string &model_name);
	int LoadModel(string &model_name);
	int LoadModelNew(string &model_name);
	void TrainingIter(string &trainfile, int iter);
};

#endif
