#ifndef FEATUREVECTOR_H
#define FEATUREVECTOR_H

#include <string>
#include <vector>
#include <list>
#include <utility>
#include <iostream>
#include "Parameters.h"
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <map>
//#include <boost/serialization/>
//#include <boost/serialization/string.hpp>
//#include <boost/serialization/version.hpp>

typedef vector<int > FeatList;
typedef vector<int >::iterator FeatList_IT;
typedef vector<double > ValList;
typedef vector<double >::iterator ValList_IT;

using namespace std;

class FeatureVector{

public:
	FeatList feats;
	ValList  vals;
public:
	
	~FeatureVector();

	void Add(int feat_id, double v);
	void Add(vector<int> &newkeys, int off_idx, int off_size);
	void Add(FeatureVector &fv);
	void Add(FeatureVector &fv, double neg_flag);
	
	double GetScore(Parameters &params);
	int GetFeatSize();	

	void GetKeys(vector<int> &keys);
	void CopyFrom(FeatureVector &fv);
	void GetDistVector(FeatureVector &fv);
	void UpdateParams(Parameters &params, double a, double upd, int neg_flag);
	void Clear();
	void Reserve(int fv_size);
	double DotProduct(FeatureVector &fv1);
	void AddFeats2Map( map<int, double> &m);
	///
	//void CopyFrom(FeatList &newfeats);

	//void Add(int feat_ids[], int size);
	//void Add(list<int> &feat_ids);
	//void Add(FeatList &newfeats);
	//void Add(FeatList::iterator itb, FeatList::iterator ite);
};
class ResultItem{
public:
FeatureVector fv;
string parse_tree;
double score;
};

namespace boost {
namespace serialization {

template<class Archive>
void serialize(Archive & ar, FeatureVector &fv, const unsigned int version)
{
//    ar & fv.b;
//	ar & fv.aa;
	ar & fv.feats;
}
template<class Archive>
void serialize(Archive & ar, pair<int, double> &p, const unsigned int version)
{
//    ar & fv.b;
    ar & p.first;
    ar & p.second;
}

} // namespace serialization
} // namespace boost
#endif
