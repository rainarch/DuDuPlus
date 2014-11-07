#ifndef DECODER_H
#define DECODER_H

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
using namespace std;


class Decoder{

public:
//	Pipe *ppipe;
public:
	void ResetPairs(vector<pair<int, int> > &pairs);
	void GetDepLabels(vector<double> &nt_probs, int len, Pipe &pipe,
                           vector<int> &static_labels, vector<double> &static_scores);
	void DecodeProjective(Instance &instance,vector<FeatureVector> &fvs,  vector<double> &probs,
                            vector<FeatureVector> &nt_fvs,vector<double> &pnt_probs,
							int K, vector<ResultItem > &d, Pipe &pipe);
	void DecodeBeam(Instance &instance,vector<FeatureVector> &fvs,  vector<double> &probs,
                            vector<FeatureVector> &fvs_trip,vector<double> &probs_trip,
                            vector<FeatureVector> &fvs_sib,vector<double> &probs_sib,
                            vector<FeatureVector> &fvs_gc,vector<double> &probs_gc,
                            vector<int> &trip_ids,vector<int> &sib_ids,vector<int> &gc_ids,
                            vector<FeatureVector> &nt_fvs,vector<double> &pnt_probs,
                            int K, vector<ResultItem > &d, Pipe &pipe, Parameters &params);
};

#endif
