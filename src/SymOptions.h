#ifndef PARSEROPTIONS_H
#define PARSEROPTIONS_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include <stdio.h>
using namespace std;

class SymOptions{

public:
    string trainfile;
    string testfile;
	string featfile;
    bool train;
    bool eval;
    bool test;
    string model_name;//dp
	string posmodel_name;//pos
	string segmodel_name;//seg
    string loss_type;
    string decode_type;
	bool labeled_parsing;
    int num_iters;
    string outfile;
    string goldfile;
    int trainK;
    int testK;
	int order;
	bool beamfeat;// to use higher-order feats in a low-order model
	string format;//sent or conll	

	bool usefeat; 
	bool feat_memo;
	string lang;
	string task;

	bool flag_coarsepos;
	bool flag_subtree;
	bool flag_cluster;
	string subtreefile;//subtree
	string clusterfile;

	string runtype;//BASIC; ST; FULL; LBBasic; LBST; LBFULL
	int  max_sentlen;	

	int debug_level;
	bool DEBUG;
	bool help;
public:
	SymOptions(int argc, char* argv[]);
	void SetRunType(string &runtype);
	void PrintOptions();
	void PrintHelp();
};

#endif
