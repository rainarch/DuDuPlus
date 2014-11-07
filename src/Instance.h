#ifndef INSTANCE_H
#define INSTANCE_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include "FeatureVector.h"
#include "WordItem.h"

using namespace std;

class Instance{

public:
	string act_parsetree;
	//vector<pair<int, double> > act_feats;
	FeatureVector act_fv;
	vector<WordItem > word_items;
	int word_num;
public:
	Instance();
	~Instance();
	void Clear();
	void PrintInst();
};

#endif

