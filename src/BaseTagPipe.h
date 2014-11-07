#ifndef BaseTagPIPE_H
#define BaseTagPIPE_H

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

using namespace std;

class BaseTagPipe{

public:
	MyHashTable feat_list;
	MyHashTable type_list;
	MyHashTable word_list;
	MyHashTable typepair_list;
	int instance_num;
	BaseReader rd_data;	
	string *types;
	int types_num;

	string start_tag;
	string end_tag;

	string loss_type;
	int root_type;
	SymOptions *poptions;

	MyDict mydict;
	vector<vector<FeatureVector> > allFVs;//to store the features of all the instances.
public:
	BaseTagPipe();
	BaseTagPipe(SymOptions *options);
	~BaseTagPipe();

	virtual int InitFile (string file){};
	void CloseFile();
	void NextInstance(Instance &inst);
	void GetNext(Instance &instance);
	void CloseAlphabets();
	void CreateAlphabet(string &file);
	int  Add(string &feat, FeatureVector &fv); 
	int  AddTwo(string &feat, string &suffix, FeatureVector &fv);
	int  Add(string &feat, string &suffix, FeatureVector &fv);
	void CreateFeatureVector(Instance &instance, FeatureVector &fv);
	void CreateInstances(string &file,  string &feat_filename);
	void WriteInstance(Instance &instance, ofstream &feat_wd);
	void ReadInstance(ifstream &feat_rd, Instance &instance,
                            vector<FeatureVector> &fvs,  vector<double> &probs,
                             Parameters &params);
	virtual void PreprocessInstance(Instance &instance){};
	virtual void AddCoreFeatures(Instance &instance,
                int curI,
                string curT,
                FeatureVector &fv){};
	virtual void AddTagFeatures(Instance &instance,
                int curI,
                string preT,
                string curT,
                FeatureVector &fv){};

	void CreateTagFeatureVector(Instance &instance, string &tag, FeatureVector &fv);
	void FillFeatureVectors(Instance &instance,
                            vector<FeatureVector> &fvs,  vector<double> &probs,
                             Parameters &params);

	void FillFeatureVectors(Instance &instance,
                            vector<FeatureVector> &fvs);

	void ScoreFeatureVectors(Instance &instance,
                            vector<FeatureVector> &fvs,  vector<double> &probs,
                             Parameters &params);
    double GetNumErrors(Instance &instance, string &tag, string &gold);
    double GetNumErrors2(Instance &instance, string &tag, string &gold);

	void WordType(string &w0, string &type);
	double TypeID2Str(string &s);
};

#endif
