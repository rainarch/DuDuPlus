#ifndef PIPEO2_H
#define PIPEO2_H

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
#include "Pipe.h"
using namespace std;

class PipeO2: public Pipe{

public:
public:

	void AddExtendedFeatures(Instance &instance,FeatureVector &fv);
	void AddTripFeatures(Instance &instance,
                       int par,
                       int ch1, int ch2,
                       FeatureVector &fv);
	void AddSiblingFeatures(Instance &instance,
                      int ch1, int ch2,
                      bool isST,
                      FeatureVector &fv);
	void AddGCFeatures(Instance &instance,
                      int par, int ch,
                      int gc,
                      FeatureVector &fv);
	//other features
	void AddTripSubtreeFeatures(Instance &instance,
                       int par,
                       int ch1, int ch2,
                       FeatureVector &fv);
	void AddGCSubtreeFeatures(Instance &instance,
                      int par, int ch,
                      int gc,
                      FeatureVector &fv);
	void WriteExtendedFeatures(Instance &instance, ofstream &feat_wd);
	void FillFeatureVectors2(Instance &instance, vector<FeatureVector> &fvs,  vector<double> &probs,
                            vector<FeatureVector> &fvs_trip,  vector<double> &probs_trip,
                            vector<FeatureVector> &fvs_sib,  vector<double> &probs_sib,
                            vector<FeatureVector> &fvs_gc,  vector<double> &probs_gc,
							vector<int> &trip_ids,vector<int> &sib_ids,vector<int> &gc_ids,
                            vector<FeatureVector> &nt_fvs,vector<double> &pnt_probs,
                             Parameters &params);
	void ReadInstance2(ifstream &feat_rd, Instance &instance,
                            vector<FeatureVector> &fvs,  vector<double> &probs,
                            vector<FeatureVector> &fvs_trip,  vector<double> &probs_trip,
                            vector<FeatureVector> &fvs_sib,  vector<double> &probs_sib,
                            vector<FeatureVector> &fvs_gc,  vector<double> &probs_gc,
							vector<int> &trip_ids,vector<int> &sib_ids,vector<int> &gc_ids,
                            vector<FeatureVector> &nt_fvs,vector<double> &pnt_probs,
                             Parameters &params);
	void TagFeatureVector2(Instance &instance, vector<int> &tag_heads, vector<FeatureVector> &fvs,  vector<double> &probs,
                            vector<FeatureVector> &fvs_trip,  vector<double> &probs_trip,
                            vector<FeatureVector> &fvs_sib,  vector<double> &probs_sib,
                            vector<FeatureVector> &fvs_gc,  vector<double> &probs_gc,
                            vector<int> &trip_ids,vector<int> &sib_ids,vector<int> &gc_ids,
                            vector<FeatureVector> &nt_fvs,vector<double> &pnt_probs,
                            FVLink &fvlink);
	bool IsSIB(int ch1, int ch2);
	bool IsTrip(int par, int ch1, int ch2);

	bool IsGC(int par, int ch, int gc);

	virtual void AddHigherFeatures(Instance &instance,FeatureVector &fv){};
	virtual void AddGSibFeatures(Instance &instance,int par, int ch, int gc, int gc1, FeatureVector &fv){};
    virtual void AddTriSibFeatures(Instance &instance,int par, int ch1, int ch2, int ch3, FeatureVector &fv){};
	int GetFVTripIdx(int instance_length, int s, int r, int t);
	int GetFVSIBIdx(int instance_length, int r, int t, int type);
	int GetFVTripIdx2(int instance_length, int s, int r, int t);
	int GetFVSIBIdx2(int instance_length, int r, int t, int type);
	int GetFVGCIdx(int instance_length, int k1, int k2, int k3);
	int GetFVGCIdx2(int instance_length, int k1, int k2, int k3);
	void TestIdx();
 	int  GetGSibSize(int sent_len){};
    int  GetGSibIdx(int sent_len, int par, int ch, int gc, int gc1){};
    int  GetTriSibSize(int sent_len){};
    int  GetTriSibIdx(int sent_len, int par, int ch1, int ch2, int ch3){};
};

#endif
