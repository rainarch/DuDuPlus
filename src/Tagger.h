#ifndef TAGGER_H
#define TAGGER_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include "MyHashTable.h"
#include "SymOptions.h"
#include "FeatureVector.h"
#include "Parameters.h"

#include "POSPipe.h"
#include "TagDecoder.h"
#include "BaseWriter.h"
using namespace std;

class Tagger{

public:
	POSPipe *ppipe;
	SymOptions *poptions;
	TagDecoder decoder;
	Parameters params;
public:
	Tagger(POSPipe *, SymOptions *);
	void Train(string &trainfile);
	void Test(string &testfile);
	void TestOneSent(Instance &instance, vector<ResultItem > &d);
	void TestOneSent(Instance &instance);
	void SaveModel(string &model_name);
	void SaveModelNew(string &model_name);
	int LoadModel(string &model_name);
	int LoadModelNew(string &model_name);
	void TrainingIter(string &trainfile, int iter);
};

#endif
