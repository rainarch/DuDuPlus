#ifndef SegPIPE_H
#define SegPIPE_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include "MyHashTable.h"
#include "SymOptions.h"
#include "BaseReader.h"
#include "FeatureVector.h"
#include "Parameters.h"
#include "MyDict.h"
#include "BaseTagPipe.h"
using namespace std;

class SegPipe: public BaseTagPipe{

public:
	//SegPipe();
	~SegPipe();
	SegPipe(SymOptions *options);
	int InitFile (string file);

	void AddCoreFeatures(Instance &instance,
                int curI,
                string curT,
                FeatureVector &fv);
    void AddTagFeatures(Instance &instance,
                int curI,
                string preT,
                string curT,
                FeatureVector &fv);
	void PreprocessInstance(Instance &instance);
	void CodeMap(string &w, string &code);
	bool SeqTagCheck(string &s);
};

#endif
