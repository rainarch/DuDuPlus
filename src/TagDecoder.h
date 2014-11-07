#ifndef TAGDECODER_H
#define TAGDECODER_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include "MyHashTable.h"
#include "FeatureVector.h"
#include "Instance.h"

using namespace std;

class TagDecoder{

public:
	int types_num;
	int root_type;
public:
	void DecodeProjective(Instance &instance,vector<FeatureVector> &fvs,  vector<double> &probs,
							int K, vector<ResultItem> &d);
};

#endif
