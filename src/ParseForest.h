#ifndef PARSEFOREST_H
#define PARSEFOREST_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include "SpanItem.h"
#include "Heap.h"
using namespace std;


class ParseForest{
public:
    int root_type;
	vector<SpanItem> chart; //[][][][][]
    int start,end;
    int K;
	SpanItem tmp;
public:
	ParseForest(int start, int end, int K);

	int GetIdx(int s, int t, int dir, int comp);
	bool Add(int s, int type, int dir, double score);
	bool Add(int s, int type, int dir, double score, FVLink &fvlink);
	bool Add(int s, int r, int t, int type,
               int dir, int comp, double score,
               FVLink &fvlink,
               SpanItem *p1, SpanItem *p2);

	double GetProb(int s, int t, int dir, int comp);

	double GetProb(int s, int t, int dir, int comp, int i);

	void GetProbs(int s, int t, int dir, int comp, vector<double> &result);
 	vector<SpanItem>::iterator GetItem(int s, int t, int dir, int comp);

	vector<SpanItem>::iterator GetItem(int s, int t, int dir, int comp, int k);
	void GetItems(int s, int t, int dir, int comp, int k, vector<SpanItem *> &sitems);
	void GetBestParse(vector<ResultItem > &d) ;

	void GetBestParses(vector<ResultItem > &d) ;

	void GetFeatureVector(SpanItem &si, FVLink &fvlink) ;

	string GetDepString(SpanItem &si);

  	void GetKBestPairs(vector<SpanItem *> &items1, vector<SpanItem *> &items2, vector<pair<int, int> > &kbest);
};

#endif
