#include "FVLink.h"

FVLink::FVLink(){
	//pfvs.reserve(10);
}
void FVLink::Reserve(int size){
	if(size > 1)
    	pfvs.reserve(size);
}

void FVLink::Add(FVLink &newfvlink){
	//this->pfvs.insert(this->pfvs.end(), newfvlink.pfvs.begin(), newfvlink.pfvs.end());
	Add(newfvlink, 1.0);
}

void FVLink::Add(FVLink &newfvlink, double neg_flag){
	for(int i =0; i < newfvlink.pfvs.size();++i){
		newfvlink.pfvs[i].second *= neg_flag;
		this->pfvs.push_back(newfvlink.pfvs[i]);
	}
//	string s;
//	ToString(s);
//	cout << "New:" << s << endl;
}

void FVLink::Add(FeatureVector &fv){
	Add(fv, 1.0);
}
void FVLink::Add(FeatureVector &fv, double neg_flag){
	pair<FeatureVector *, double> p;
	p.first = &fv;
	p.second = neg_flag;
	this->pfvs.push_back(p);
}

void FVLink::Add(FeatureVector *pfv){
    Add(pfv, 1.0);
}
void FVLink::Add(FeatureVector *pfv, double neg_flag){
    pair<FeatureVector *, double> p;
    p.first = pfv;
    p.second = neg_flag;
    this->pfvs.push_back(p);
}


void FVLink::ToFV(FeatureVector &fv){
//	cout << "TOFV" << endl;
	for(int i =0; i < pfvs.size();++i){
//		cout << pfvs[i].first->feats.size() << " " << pfvs[i].second << endl;	
		fv.Add(*pfvs[i].first, pfvs[i].second);
    }

}

void FVLink::ToString(string &s){
	ostringstream ss (ostringstream::out);
	for(int i =0; i < pfvs.size();++i){
		for(int j = 0; j < pfvs[i].first->feats.size();j++){
			//ss  << pfvs[i].first->feats[j].first  << "==" << pfvs[i].first->feats[j].second << " ";
		}
    }
	s = ss.str();
}
