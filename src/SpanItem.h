#ifndef SPANITEM_H
#define SPANITEM_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include "FeatureVector.h"
#include "FVLink.h"
#include <float.h>

#define MINVALUE -DBL_MAX

using namespace std;

class SpanItem{
public:
    int s,r,t,dir,comp,length,type;
    double prob;
    FVLink fvlink;
	//vector<SpanItem> next;//have two
	SpanItem *pleft, *pright;
public:
	SpanItem(int s, int type, int dir, double prob);
	SpanItem(int i, int k, int j, int type, int dir, int comp,double prob, FVLink &fvlink, SpanItem *pleft, SpanItem *pright);
	SpanItem(int s, int type, int dir, double prob, FVLink &fvlink);
	SpanItem(double prob);
	SpanItem();
	void Init(int s, int type, int dir, double prob);
	void CopyValuesTo(SpanItem &p);
	bool Equals(SpanItem &p);
	bool IsPre();
	
	void Reset(int s, int type, int dir, double prob);
	void Reset(int s, int type, int dir, double prob, FVLink &fvlink);

	void Reset(int i, int k, int j, int type,
               int dir, int comp,
               double prob, FVLink &fvlink,
               SpanItem *pleft, SpanItem *pright);
	//for TAgging
	void Reset(int i, int type, double prob, FVLink &fvlink,
               SpanItem *pleft);
};

#endif
