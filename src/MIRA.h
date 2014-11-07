#ifndef MIRA_H
#define MIRA_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include "FeatureVector.h"
#include "Parameters.h"

using namespace std;

class MIRA{
public:
	void UpdateParamsMIRA(FeatureVector &act_fv,
                                 vector<ResultItem > &d, double upd,
                                 string gold_string, Parameters &params);
	void UpdateParamsMIRA(FeatureVector &act_feats,
                                 vector<ResultItem > &d, double upd,
                                 vector<double> &num_errs, Parameters &params);
	void Hildreth(vector<FeatureVector> & dist, vector<double> &b, vector<double> & alpha);

	void Update(Parameters &params, double a, double upd, FeatureVector &fv);

	void Update(Parameters &params, double a, double upd, FeatureVector &fv, int neg_flag);
 
};

#endif
