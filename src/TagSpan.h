#ifndef TAGSPAN_H
#define TAGSPAN_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include "SpanItem.h"
#include "Heap.h"

using namespace std;


class TagSpan{
public:
    int root_type;
	vector<SpanItem> chart; //[][][][][]
    int start,end;
    int K;
	int types_num;
	SpanItem tmp;
	
public:
	TagSpan(int start, int end, int types_num, int K);

	int GetIdx(int s, int type);
	bool Add(int s, int t,  double score,
               FVLink &fvlink,
               SpanItem *p1);

	double GetProb(int s, int t);

	double GetProb(int s, int t, int k);

	void GetProbs(int s, int t, vector<double> &result);
 	vector<SpanItem>::iterator GetItem(int s, int t);

	vector<SpanItem>::iterator GetItem(int s, int t, int k);
	void GetItems(int s, int t, int k, vector<SpanItem *> &sitems);
	void GetBestPath(vector<ResultItem > &d) ;

	void GetBestPaths(vector<ResultItem > &d) ;

	void GetFeatureVector(SpanItem &si, FVLink &fvlink) ;

	string GetDepString(SpanItem &si);

  	void GetKBestPairs(vector<SpanItem *> &items1, vector<SpanItem *> &items2, vector<pair<int, int> > &kbest);
};

#endif
