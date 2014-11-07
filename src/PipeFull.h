#ifndef PIPEFULL_H
#define PIPEFULL_H

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
#include "PipeO2.h"
using namespace std;

class PipeFull: public PipeO2{

public:
public:

	void AddHigherFeatures(Instance &instance,FeatureVector &fv);
	void AddGSibFeatures(Instance &instance,int par, int ch, int gc, int gc1, FeatureVector &fv);
	void AddTriSibFeatures(Instance &instance,int par, int ch1, int ch2, int ch3, FeatureVector &fv);
	int  GetGSibSize(int sent_len);
	int  GetGSibIdx(int sent_len, int par, int ch, int gc, int gc1);
	int  GetTriSibSize(int sent_len);
	int  GetTriSibIdx(int sent_len, int par, int ch1, int ch2, int ch3);
};

#endif
