#ifndef PARSEFORESTO2_H
#define PARSEFORESTO2_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include "SpanItem.h"
#include "Heap.h"
using namespace std;


class ParseForestO2{
public:
    int root_type;
	vector<SpanItem> chartO; //incomplete items
	vector<SpanItem> chartC; //complete items, need r
	vector<SpanItem> chartCK;  //complete items 
    int start,end;
    int K;
	SpanItem tmp;
public:
	ParseForestO2(int start, int end, int K);

	int GetIdxO(int s, int t, int dir);
	int GetIdxC(int s, int t, int r, int dir);
	bool Add(int s, int type, int dir, double score);
	void GetItemsC(int s, int t, int r, int dir, vector<SpanItem *> &sitems);
	void GetItemsO(int s, int t,  int dir, vector<SpanItem *> &sitems);
	bool AddToKItems(SpanItem *p1, double prob, vector<SpanItem *> &sitems , vector<double> &b1p);
  	void GetKBestPairs2(vector<SpanItem *> &items1, vector<SpanItem *> &items2, vector<double> &probs1, vector<double> &probs2, vector<pair<int, int> > &kbest);

	bool AddO(int s, int r, int t, int type,
               int dir, int comp, double score,
               FVLink &fvlink,
               SpanItem *p1, SpanItem *p2);
	bool AddC(int s, int r, int t, int type,
               int dir, int comp, double score,
               FVLink &fvlink,
               SpanItem *p1, SpanItem *p2);
	/*bool Add(int s, int type, int dir, double score, FVLink &fvlink);
	*/
	//double GetProb(int s, int t, int dir, int comp);

	//double GetProb(int s, int t, int dir, int comp, int i);

	//void GetProbs(int s, int t, int dir, int comp, vector<double> &result);
 	//vector<SpanItem>::iterator GetItem(int s, int t, int dir, int comp);
	//vector<SpanItem>::iterator GetItem(int s, int t, int dir, int comp, int k);
	//void GetBestParse(vector<pair<FeatureVector, string> > &d) ;

	void GetBestParses(vector<ResultItem > &d) ;

	void GetFeatureVector(SpanItem &si, FVLink &fvlink) ;

	string GetDepString(SpanItem &si);

  	//void GetKBestPairs(vector<SpanItem *> &items1, vector<SpanItem *> &items2, vector<pair<int, int> > &kbest);
 
};

#endif
