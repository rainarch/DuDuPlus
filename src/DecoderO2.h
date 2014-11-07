#ifndef DECODERO2_H
#define DECODERO2_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include "MyHashTable.h"
#include "SymOptions.h"
#include "FeatureVector.h"
#include "PipeO2.h"
#include "Decoder.h"
using namespace std;

class DecoderO2: public Decoder{

public:
public:
	void ResetBC(vector<double> &b1p, vector<double> &c1p);
	void DecodeProjective(Instance &instance,vector<FeatureVector> &fvs,  vector<double> &probs,
                            vector<FeatureVector> &fvs_trip,vector<double> &probs_trip,
                            vector<FeatureVector> &fvs_sib,vector<double> &probs_sib,
                            vector<FeatureVector> &fvs_gc,vector<double> &probs_gc,
							vector<int> &trip_ids,vector<int> &sib_ids,vector<int> &gc_ids,
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
