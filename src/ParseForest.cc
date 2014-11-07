#include "ParseForest.h"
#include <float.h>
#define MINVALUE -DBL_MAX
ParseForest::ParseForest(int start, int end, int K) {
	this->K = K;
	this->start = start;
	this->end = end;
	int size = (end+1)*(end+1)*2*2*K;
	chart.reserve(size);
	int idx = 0;
	FeatureVector fv;
	for(int s1 =0; s1 < end+1; ++s1){
	for(int t1 =0; t1 < end+1; ++t1){
	for(int dir1 =0; dir1 < 2; ++dir1){
	for(int comp1 =0; comp1 < 2; ++comp1){
	//cout << size << ":"<<s1 << "=" << t1 << "=" << dir1 << "=" << comp1 << " :" << idx << " vs "<< GetIdx(s1, t1, dir1, comp1) << endl;
	for(int k1 = 0; k1 < K; ++k1){
		SpanItem si(s1, -1, dir1, MINVALUE);
		si.t = t1;
		si.comp = comp1;
		si.fvlink.Reserve(t1-s1);
		chart.push_back(si);
		++idx;
	}}}}}
}

int ParseForest::GetIdx(int s, int t, int dir, int comp){
	return (s*(this->end+1)*2*2+ t*2*2+ dir*2 +comp)*K; 
	//return ((  (s*(this->end+1)+t) * (this->end+1) +dir)*2+comp)*2;
}
bool ParseForest::Add(int s, int type, int dir, double score) {
    bool added = false;
    int idx = GetIdx(s, s, dir, 0);
    if(chart.at(idx+K-1).prob > score)
        return false;
    //cout << s << " " << type << " " << dir << " " << score << "Adding"<< endl;
    for(int i = 0; i < K; i++) {
        if(chart.at(idx+i).prob < score){
            if(chart.at(idx+i).prob == MINVALUE){
                chart.at(idx+i).Reset(s, type, dir, score);
            }
            else{
            //SpanItem tmp(MINVALUE);
            chart.at(idx+i).CopyValuesTo(tmp);
            chart.at(idx+i).Reset(s, type, dir, score);
            for(int j = i+1; j < K && tmp.prob != MINVALUE; j++){
                SpanItem tmp1;
                chart.at(idx+j).CopyValuesTo(tmp1);
                tmp.CopyValuesTo(chart.at(idx+j));
                tmp1.CopyValuesTo(tmp);
            }
            }
            added = true;
            break;
        }
    }

    return added;
}



bool ParseForest::Add(int s, int type, int dir, double score, FVLink &fvlink) {


	bool added = false;
	int idx = GetIdx(s, s, dir, 0);	
	if(chart.at(idx+K-1).prob > score)
		return false;	
	for(int i = 0; i < K; i++) {
		if(chart.at(idx+i).prob < score){
			if(chart.at(idx+i).prob == MINVALUE){
				chart.at(idx+i).Reset(s, type, dir, score, fvlink);
			}
			else{
			//SpanItem tmp(MINVALUE);
			
			chart.at(idx+i).CopyValuesTo(tmp);
			chart.at(idx+i).Reset(s, type, dir, score, fvlink);
			for(int j = i+1; j < K && tmp.prob != MINVALUE; j++){
				SpanItem tmp1;
				chart.at(idx+j).CopyValuesTo(tmp1);
				tmp.CopyValuesTo(chart.at(idx+j));
				tmp1.CopyValuesTo(tmp);
			}
			}
			added = true;
			break;
		}
	}

	return added;
}

bool ParseForest::Add(int s, int r, int t, int type,
		       int dir, int comp, double score,
		       FVLink &fvlink,
		       SpanItem *p1, SpanItem *p2) {
	bool added = false;
	int idx = GetIdx(s, t, dir, comp);
//	string sfv;
//	fvlink.ToString(sfv);
	//cout << s << " " << r << " " << t << " "  << dir << " " << comp << " pL:" << p1.s << ":" << p1.t << " pR:" << p2.s << ":" << p2.t << endl;
	//cout << "FVLInk=" << sfv << endl;
	//cout << "Scores:" << chart.at(idx+K-1).prob << " " << score << "K=" << K<< endl;
    if(chart.at(idx+K-1).prob > score)
        return false;
	//cout << s << " " << r << " " << t << " "  << dir << " " << comp << " pL:" << p1.s << ":" << p1.t << " pR:" << p2.s << ":" << p2.t << endl;
    for(int i = 0; i < K; i++) {
		vector<SpanItem>::iterator cur_it = chart.begin()+(idx+i);
        if(cur_it->prob < score){
			//cur_it->Reset(s,r,t, type, dir, comp, score, fvlink, p1, p2);
			//return true;

			if(cur_it->prob == MINVALUE){
				cur_it->Reset(s,r,t, type, dir, comp, score, fvlink, p1, p2);
			}
			else{
            //SpanItem tmp(MINVALUE);
            cur_it->CopyValuesTo(tmp);
			cur_it->Reset(s,r,t, type, dir, comp, score, fvlink, p1, p2);
            for(int j = i+1; j < K && tmp.prob != MINVALUE; j++){
                SpanItem tmp1;
                chart.at(idx+j).CopyValuesTo(tmp1);
                tmp.CopyValuesTo(chart.at(idx+j));
                tmp1.CopyValuesTo(tmp);
            }
			}
            added = true;
            break;
			
        }
    }

    return added;	
}

double ParseForest::GetProb(int s, int t, int dir, int comp) {
	return GetProb(s,t,dir,comp,0);
}

double ParseForest::GetProb(int s, int t, int dir, int comp, int i) {
	int idx = GetIdx(s,t,dir, comp);
	return chart.at(idx+i).prob;
}

void ParseForest::GetProbs(int s, int t, int dir, int comp, vector<double> &result) {
	result.clear();
	int idx = GetIdx(s,t,dir, comp);
	for(int i = 0; i < K; i++)
		result.push_back(chart.at(idx+i).prob);
}

vector<SpanItem>::iterator ParseForest::GetItem(int s, int t, int dir, int comp) {
	return GetItem(s,t,dir,comp,0);
}

vector<SpanItem>::iterator ParseForest::GetItem(int s, int t, int dir, int comp, int k) {
	int idx = GetIdx(s,t,dir, comp);
	return chart.begin()+(idx+k);
}

void ParseForest::GetItems(int s, int t, int dir, int comp, int k, vector<SpanItem *> &sitems) {
	//vector<SpanItem>::iterator it = GetItem(s, t, dir, comp);
	int idx = GetIdx(s,t,dir, comp);
	for(int i = 0; i < k; ++i){
		SpanItem *p = &chart[idx+i];
		sitems.at(i) = p;
	}
}

void ParseForest::GetBestParse(vector<ResultItem > &d) {
	ResultItem d1;
	int idx = GetIdx(0, end, 0,0);
	FVLink fvlink;
	GetFeatureVector(chart.at(idx+0), fvlink);
	fvlink.ToFV( d1.fv);
	d1.parse_tree = GetDepString(chart.at(idx+0));
	d1.score = chart.at(idx+0).prob;
	d.push_back(d1);
}

void ParseForest::GetBestParses(vector<ResultItem > &d) {
    int idx = GetIdx(0, end, 0,0);
	//cout << idx << endl;
	for(int k = 0; k < K; k++) {
		double ddd = 0.943463273714;
	//	cout << k;
	//	cout << " prob:" << chart.at(idx+k).prob << " "<< ddd;
	//	cout << endl;
		if(chart.at(idx+k).prob != MINVALUE){
			ResultItem d1;
			//FVLink fvlink;
			//GetFeatureVector(chart.at(idx+k),fvlink);
			//fvlink.ToFV( d1.fv);
			//cout << "Getting Rl" << endl;
    		d1.parse_tree = GetDepString(chart.at(idx+k));
			d1.score = chart.at(idx+k).prob;
    		d.push_back(d1);
		}
	}
}

void ParseForest::GetFeatureVector(SpanItem &si, FVLink &fvlink) {
	//cout << si.s << " " << si.r <<" " << si.t << " " << si.dir << " " << si.comp << " " << endl;
	
	fvlink.Add(si.fvlink);
	if(si.pleft == NULL){
	//if(si.next.size() == 0){
		return;
	}
	//cout << "LEFT:" << si.next[0].s << " " << si.next[0].t << " " << si.next[0].dir << " " << si.next[0].comp << " " << endl;
	//cout << "RIGHT:" << si.next[1].s << " " << si.next[1].t << " " << si.next[1].dir << " " << si.next[1].comp << " " << endl;
	GetFeatureVector(*si.pleft, fvlink);//left
	GetFeatureVector(*si.pright, fvlink);//right
}

string ParseForest::GetDepString(SpanItem &si) {
	//cout << si.s << " " << si.r <<" " << si.t << " " << si.dir << " " << si.comp << " " << endl;
	if(si.pleft == NULL)
	    return "";
	string s = "";
	string sL = GetDepString(*si.pleft);
	string sR = GetDepString(*si.pright);
	if(sL == "")
		s = sR;
	else if(sR == "")
		s = sL;
	else 
		s = sL + " "+ sR;
	if(si.comp == 0) {
	}
	else{
		stringstream relA;
		relA << si.s << "|" << si.t << ":" << si.type;//s is the head
		stringstream relB;
		relB << si.t << "|" << si.s << ":" << si.type;//t is the head
		if(si.dir == 0) {
			if(s != "")
				s += (string)" "+relA.str();
			else
				s = relA.str();
		}
		else {
			if(s != ""){
            	string s1 = relB.str()+" "+s;
				s = s1;
			}
			else
				s = relB.str();
		}	
	}
	//cout << si.s << " " << si.r <<" " << si.t << " " << si.dir << " " << si.comp << " " << s<< endl;
	return s;
}
	

	
    // returns pairs of indeces and -1,-1 if < K pairs
void ParseForest::GetKBestPairs(vector<SpanItem*> &items1, vector<SpanItem*> &items2, vector<pair<int, int> > &kbest) {

	//kbest.clear();
	
	if(items1.size() == 0 || items2.size() == 0)
		return;
	if(K == 1){
		kbest[0].first = 0;
        kbest[0].second = 0;
		return;
	}
	

	vector<int> beenvisited;
	pair<int, int > onepair;

	beenvisited.resize(K*K);
	Heap hp(K+1);
	HeapItem hi(items1.at(0)->prob + items2.at(0)->prob, 0,0);
	//hi.i1 = 0; hi.i2 =0; hi.val = items1.at(0).prob + items2.at(0).prob;
	hp.Add(hi);
 	beenvisited.at(0*K+0) = 1;
	int n = 0;
	while(n < K){
		hp.RemoveMax(hi);
		if(hi.i1 == -1 || hi.val <= MINVALUE)
            break;
//		onepair.first = hi.i1;
//		onepair.second = hi.i2;
//		kbest.push_back(onepair);
		kbest[n].first = hi.i1;
        kbest[n].second = hi.i2;
		n++;
		if(n >= K)
			break;
		if(beenvisited.at((hi.i1+1)*K+hi.i2) != 1 && (hi.i1+1) < items1.size() && items1.at(hi.i1+1)->prob != MINVALUE ){
			beenvisited.at((hi.i1+1)*K+hi.i2) = 1;
			double probA = items1.at(hi.i1+1)->prob + items2.at(hi.i2)->prob;
			HeapItem hiT(probA, hi.i1+1,hi.i2);
			hp.Add(hiT);
		}
		if(beenvisited.at(hi.i1*K+hi.i2+1) != 1 && (hi.i2+1) < items2.size() && items2.at(hi.i2+1)->prob != MINVALUE){
            beenvisited.at((hi.i1)*K+hi.i2+1) = 1;
			double probB = items1.at(hi.i1)->prob + items2.at(hi.i2+1)->prob;
			HeapItem hiT(probB, hi.i1,hi.i2+1);
            hp.Add(hiT);
        }

	}
}
