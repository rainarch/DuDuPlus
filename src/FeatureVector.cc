#include "FeatureVector.h"
#include <map>

FeatureVector::~FeatureVector(){
}

void FeatureVector::Reserve(int fv_size){
	feats.reserve(fv_size);
	vals.reserve(fv_size);
}

void FeatureVector::Add(int feat_id, double v){
	feats.push_back(feat_id);
	vals.push_back(v);
	//cout << "Added" << feat_id << endl;
}

double FeatureVector::GetScore(Parameters &params){
	double score = 0.0;
	FeatList_IT it = feats.begin();
	ValList_IT vit = vals.begin();
    while(it != feats.end()){
		int idx = *it;
		double v = *vit;
		score += params.parameters[idx]* (v);
        ++it;
		++vit;
    }
	return score;
}

int FeatureVector::GetFeatSize(){
	return feats.size();
}
/*
void FeatureVector::CopyFrom(FeatList &newfeats){
	//this->feats = newfeats;
	this->feats.clear();
	this->feats.reserve(newfeats.size());
	copy(newfeats.begin(), newfeats.end(), back_inserter(this->feats));
}
*/
void FeatureVector::CopyFrom(FeatureVector &fv){
	this->feats.clear();
    this->feats.reserve(fv.feats.size());
    this->vals.clear();
    this->vals.reserve(fv.vals.size());
	copy(fv.feats.begin(), fv.feats.end(), back_inserter(this->feats));
	copy(fv.vals.begin(), fv.vals.end(), back_inserter(this->vals));
}


void FeatureVector::GetDistVector(FeatureVector &fv){
	Add(fv, -1.0);
}

void FeatureVector::Add(FeatureVector &fv){
	Add(fv, 1.0);
}

void FeatureVector::Clear(){
	this->feats.clear();
	this->vals.clear();
}


void FeatureVector::Add(FeatureVector &fv, double neg_flag){
    if(fv.feats.size() == 0)
		return;
	this->feats.reserve(fv.feats.size()+this->feats.size());
	this->vals.reserve(fv.vals.size()+this->vals.size());
	copy(fv.feats.begin(), fv.feats.end(), back_inserter(this->feats));

	if(neg_flag == 1.0){
		copy(fv.vals.begin(), fv.vals.end(), back_inserter(this->vals));
	}
	else{//
		ValList_IT it = fv.vals.begin();
		while(it != fv.vals.end()){
			double d = *it * neg_flag;
			this->vals.push_back(d);
			++it;
		}
	}
/*	cout << this->feats.size()  << endl;
	it = this->feats.begin();
    while(it != this->feats.end()){
		cout << "Having " << it->first << endl;
        ++it;
    }
*/
	return;
}
/*
void FeatureVector::Add(int feat_ids[], int size){
	if(size <= 0) return;
	//this->feats.reserve(size+this->feats.size());
	for(int i = 0; i < size; ++i)
		Add(feat_ids[i], 1.0);
}
*/
/*
void FeatureVector::Add(list<int> &feat_ids){
	//this->feats.reserve(feat_ids.size()+this->feats.size());
	list<int>::iterator it = feat_ids.begin();
	while(it != feat_ids.end()){
		Add(*it, 1.0);
		++it;
	}
}
*/
/*
void FeatureVector::Add(FeatList &newfeats){
    //this->feats = newfeats;
    //this->feats.reserve(this->feats.size() + newfeats.size());
    copy(newfeats.begin(), newfeats.end(), back_inserter(this->feats));
}
*/
/*
void FeatureVector::Add(FeatList_IT itb, FeatList_IT ite){
    //this->feats = newfeats;
    //this->feats.reserve(this->feats.size() + newfeats.size());
    copy(itb, ite, back_inserter(this->feats));
}*/
void FeatureVector::Add(vector<int> &newkeys, int off_idx, int off_size){
	FeatList_IT itb = newkeys.begin();
	itb +=off_idx;
	FeatList_IT ite = newkeys.begin()+off_idx+off_size;
	copy(itb, ite, back_inserter(this->feats));
	this->vals.insert(this->vals.end(), off_size, 1.0);
}




void FeatureVector::AddFeats2Map(map<int, double> &m){
	FeatList_IT it = this->feats.begin();
	ValList_IT vit = this->vals.begin();
	//cout << "SIZE:" << myfeats.size() << endl;
    while(it != this->feats.end()){
		int idx = *it;
		double v = *vit;
		map<int, double>::iterator myit = m.find(idx);
		if(myit != m.end()){
			m[idx] += v;
		}
		else{
			m[idx] = v;
		}
	//	cout << it->first << "=" << m[it->first]  << endl;
        ++it;
		++vit;
    }

}

double FeatureVector::DotProduct(FeatureVector &fv1){
	//to map first
	//cout << "dot" << endl;
	map<int, double> m0;
	this->AddFeats2Map(m0);
	map<int, double> m1;
	fv1.AddFeats2Map(m1);	
	//dot product
	map<int, double>::iterator it = m0.begin();
	double v = 0.0;
	while(it != m0.end()){
		v += m1[it->first] * it->second;
		//cout << it->first << "=" << m0[it->first]  << endl;
		++it;
	}
	return v;
}

void FeatureVector::GetKeys(vector<int> &keys){
	keys.push_back(feats.size());
	copy(feats.begin(), feats.end(), back_inserter(keys));    
}
void FeatureVector::UpdateParams(Parameters &params, double a, double upd, int neg_flag){
	if(this->feats.size() == 0)
        return;
	
    FeatList_IT it = this->feats.begin();
   	ValList_IT vit = this->vals.begin();
    while(it != this->feats.end()){
		int idx = *it;
		double v = *vit;
        params.parameters[idx] += (neg_flag*a*v) ;
        params.total[idx] += (upd* neg_flag*a*v) ;
        ++it;
		++vit;
    }

}
