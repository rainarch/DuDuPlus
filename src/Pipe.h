#ifndef PIPE_H
#define PIPE_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include "MyHashTable.h"
#include "SymOptions.h"
#include "BaseReader.h"
#include "FeatureVector.h"
#include "Parameters.h"
#include "FVLink.h"
#include "MyDict.h"
using namespace std;

class Pipe{

public:
	MyHashTable feat_list;
	MyHashTable type_list;
	MyHashTable pospair_type_list;
	MyDict mydict;

	unordered_map<string, string> subtree_list;
	unordered_map<string, string> wordclu_list;
	//To speed up
	unordered_map<string, string> wordzerohd_list;//most freq words(Top 100) that have never been a head in training data

	int instance_num;
	BaseReader rd_data;	
	string *types;
	int types_num;

	string loss_type;
	SymOptions *poptions;
public:
	Pipe();
	~Pipe();
	Pipe(SymOptions *options);
	void Init(SymOptions *options);

	int InitFile (string file);
	void CloseFile();
	void NextInstance(Instance &inst);
	void CloseAlphabets();
	void CreateAlphabet(string &file);
	int Add(string &feat, FeatureVector &fv); 
	int AddTwo(string &feat, string &suffix, FeatureVector &fv);
	int Add(string &feat, string &suffix, FeatureVector &fv);
	void CreateFeatureVector(Instance &instance, FeatureVector &fv);
	void CreateInstances(string &file,  string &feat_filename);
	void WriteInstance(Instance &instance, ofstream &feat_wd);
	void ReadInstance(ifstream &feat_rd, Instance &instance,
                            vector<FeatureVector> &fvs,  vector<double> &probs,
                            vector<FeatureVector> &nt_fvs,vector<double> &pnt_probs,
                             Parameters &params);

	void CreateTagFeatureVector(Instance &instance, string &tag, FeatureVector &fv);

	void AddCoreFeatures(Instance &instance,
                int small,
                int large,
                bool attR,
                FeatureVector &fv);
	void CreateClusterItems(Instance&);
	void FillFeatureVectors(Instance &instance,
                            vector<FeatureVector> &fvs,  vector<double> &probs,
                            vector<FeatureVector> &nt_fvs,vector<double> &pnt_probs,
                             Parameters &params);

	void AddLabeledFeatures1(Instance &instance,
                   int wid,
                   string &label,
                   bool attR,
                   bool isChild,
                   FeatureVector &fv); 
	void AddLabeledFeatures(Instance &instance,
                int small,
                int large,
                string &label,
                bool attR,
                FeatureVector &fv) ;
	void AddLinearFeatures(string prefix, Instance &instance,
                             int small, int large, string &att_dist, FeatureVector &fv);

	void AddCorePosFeatures(string prefix, string p1L, string p1, string p1R,
                             string p2L, string p2, string p2R, string &att_dist, FeatureVector &fv);

	void AddTwoObsFeatures(string prefix, Instance &instance, int wid1, int wid2,
                             string &att_dist, FeatureVector &fv);
	void AddTwoObsFeatures(string prefix, Instance &instance, string &w1, string &w2,
                            string &p1, string &p2, bool wcopy, bool pcopy,
                             string &att_dist, FeatureVector &fv);
	void TypeID2Str(string &s);
	void ParseTree2Array(string tag, vector<int> &tag_heads);
	//Other features
	void AddSFeatures(string prefix, Instance &instance,
                            int hd, int ch , string &att_dist, FeatureVector &fv);

	void AddSubtreeFeatures(string &prefix, Instance &instance,
                             int small, int large, bool attR, FeatureVector &fv);
	void AddSubtreeFeaturesItems(string &prefix, bool attR, string &val,
                                   string &w1, string &w2, string &p1, string &p2,
                                    FeatureVector &fv);
	void AddSubtreeFeaturesItems(string &prefix, bool attR, string &val,
                                   string &w1, string &p1,
                                    FeatureVector &fv);
	//End
	void TagFeatureVector(Instance &instance, vector<int> &tag_heads, vector<FeatureVector> &fvs,  vector<double> &probs,
                            vector<FeatureVector> &nt_fvs,vector<double> &pnt_probs,
                            FVLink &fvlink);
	virtual void TagFeatureVector2(Instance &instance, vector<int> &tag_heads, 
							vector<FeatureVector> &fvs,  vector<double> &probs,
                            vector<FeatureVector> &fvs_trip,  vector<double> &probs_trip,
                            vector<FeatureVector> &fvs_sib,  vector<double> &probs_sib,
                            vector<FeatureVector> &fvs_gc,  vector<double> &probs_gc,
                            vector<int> &trip_ids,vector<int> &sib_ids,vector<int> &gc_ids,
                            vector<FeatureVector> &nt_fvs,vector<double> &pnt_probs,
                            FVLink &fvlink){};
	bool   IsGoodHead(string &w);
    double GetNumErrors(Instance &instance, string &tag, string &gold);
    double GetNumErrorsDEPNonpunc(Instance &instance, string &tag, string &gold);
    double GetNumErrorsDEP(Instance &instance, string &tag, string &gold);


	int GetFVIdx(int instance_length, int s, int t);
	int GetFVIdx2(int instance_length, int s, int t);
	//int GetLBFVIdx(int instance_length, int wid, int t);
	//int GetLBFVIdx2(int instance_length, int wid, int t);
	int GetLBFVIdx(int instance_length, int hid, int wid, int t);
    int GetLBFVIdx2(int instance_length, int hid, int wid, int t);
	void TestIdx();
	//
	virtual void AddExtendedFeatures(Instance &instance,FeatureVector &fv){};
    virtual void WriteExtendedFeatures(Instance &instance, ofstream &feat_wd){};
	virtual void FillFeatureVectors2(Instance &instance, vector<FeatureVector> &fvs,  vector<double> &probs,
                            vector<FeatureVector> &fvs_trip,  vector<double> &probs_trip,
                            vector<FeatureVector> &fvs_sib,  vector<double> &probs_sib,
                            vector<FeatureVector> &fvs_gc,  vector<double> &probs_gc,
							vector<int> &trip_ids,vector<int> &sib_ids,vector<int> &gc_ids,
                            vector<FeatureVector> &nt_fvs,vector<double> &nt_probs,
                             Parameters &params){};
	virtual void ReadInstance2(ifstream &feat_rd, Instance &instance,
                            vector<FeatureVector> &fvs,  vector<double> &probs,
                            vector<FeatureVector> &fvs_trip,  vector<double> &probs_trip,
                            vector<FeatureVector> &fvs_sib,  vector<double> &probs_sib,
                            vector<FeatureVector> &fvs_gc,  vector<double> &probs_gc,
							vector<int> &trip_ids,vector<int> &sib_ids,vector<int> &gc_ids,
                            vector<FeatureVector> &nt_fvs,vector<double> &nt_probs,
                             Parameters &params){};
	virtual int GetFVTripIdx(int instance_length, int s, int r, int t){};
    virtual int GetFVSIBIdx(int instance_length, int r, int t, int type){};
	virtual int GetFVGCIdx(int instance_length, int k1, int k2, int k3){};
	virtual int GetFVGCIdx2(int instance_length, int k1, int k2, int k3){};
	virtual void AddGSibFeatures(Instance &instance,int par, int ch, int gc, int gc1, FeatureVector &fv){};
    virtual void AddTriSibFeatures(Instance &instance,int par, int ch1, int ch2, int ch3, FeatureVector &fv){};
	virtual int  GetGSibSize(int sent_len){};
    virtual int  GetGSibIdx(int sent_len, int par, int ch, int gc, int gc1){};
    virtual int  GetTriSibSize(int sent_len){};
    virtual int  GetTriSibIdx(int sent_len, int par, int ch1, int ch2, int ch3){};
};

#endif
