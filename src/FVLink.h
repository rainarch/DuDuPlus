#ifndef FVLINK_H
#define FVLINK_H

#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include "Parameters.h"
#include "FeatureVector.h"

using namespace std;

class FVLink{

public:
	vector<pair<FeatureVector *, double> > pfvs;//fv+neg_flag
public:
	FVLink();
	void Reserve(int size);
	void Add(FVLink &newfvlink);
	void Add(FVLink &newfvlink, double neg_flag);

	void Add(FeatureVector *fv);
	void Add(FeatureVector *fv, double neg_flag);
	void Add(FeatureVector &fv);
	void Add(FeatureVector &fv, double neg_flag);
	void ToFV(FeatureVector &fv);
	void ToString(string &s);
};
#endif
