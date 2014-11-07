#include "TagSpan.h"
#include <float.h>
#define MINVALUE -DBL_MAX
TagSpan::TagSpan(int start, int end, int types_num, int K) {
	this->K = K;
	this->start = start;
	this->end = end;
	this->types_num = types_num;
	int size = (end+1)*types_num*K;
	chart.reserve(size);
	int idx = 0;
	FeatureVector fv;
	for(int s1 =0; s1 < end+1; ++s1){
	for(int t1 =0; t1 < types_num; ++t1){
	//cout << size << ":"<<s1 << "=" << t1 << "=" << dir1 << "=" << comp1 << " :" << idx << " vs "<< GetIdx(s1, t1, dir1, comp1) << endl;
	for(int k1 = 0; k1 < K; ++k1){
		SpanItem si(s1, -1, -1, MINVALUE);
		si.t = t1;
		chart.push_back(si);
		++idx;
	}}}
}

int TagSpan::GetIdx(int s, int type){
	return (s*(this->types_num)+type)*K; 
}


bool TagSpan::Add(int s, int type, double score,
		       FVLink &fvlink,
		       SpanItem *p1 ) {
	bool added = false;
	int idx = GetIdx(s, type );
    if(chart.at(idx+K-1).prob > score)
        return false;
	//cout << s << " " << r << " " << t << " "  << dir << " " << comp << " pL:" << p1.s << ":" << p1.t << " pR:" << p2.s << ":" << p2.t << endl;
    for(int i = 0; i < K; i++) {
		vector<SpanItem>::iterator cur_it = chart.begin()+(idx+i);
        if(cur_it->prob < score){
			//cur_it->Reset(s,r,t, type, dir, comp, score, fvlink, p1, p2);
			//return true;

			if(cur_it->prob == MINVALUE){
				cur_it->Reset(s,type, score, fvlink, p1);
			}
			else{
            //SpanItem tmp(MINVALUE);
            cur_it->CopyValuesTo(tmp);
			cur_it->Reset(s,type, score, fvlink, p1);
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

double TagSpan::GetProb(int s, int t) {
	return GetProb(s,t,0);
}

double TagSpan::GetProb(int s, int t, int i) {
	int idx = GetIdx(s,t);
	return chart.at(idx+i).prob;
}

void TagSpan::GetProbs(int s, int t, vector<double> &result) {
	result.clear();
	int idx = GetIdx(s,t);
	for(int i = 0; i < K; i++)
		result.push_back(chart.at(idx+i).prob);
}

vector<SpanItem>::iterator TagSpan::GetItem(int s, int t) {
	return GetItem(s,t,0);
}

vector<SpanItem>::iterator TagSpan::GetItem(int s, int t,  int k) {
	int idx = GetIdx(s,t);
	return chart.begin()+(idx+k);
}

void TagSpan::GetItems(int s, int t,  int k, vector<SpanItem *> &sitems) {
	//vector<SpanItem>::iterator it = GetItem(s, t, dir, comp);
	int idx = GetIdx(s,t);
	for(int i = 0; i < k; ++i){
		SpanItem *p = &chart[idx+i];
		sitems.at(i) = p;
	}
}

void TagSpan::GetBestPath(vector<ResultItem > &d) {
	ResultItem d1;
	int idx = GetIdx(end, 0);
	FVLink fvlink;
	GetFeatureVector(chart.at(idx+0), fvlink);
	fvlink.ToFV( d1.fv);
	d1.parse_tree = GetDepString(chart.at(idx+0));
	d1.score = chart.at(idx+0).prob;
	d.push_back(d1);
}

void TagSpan::GetBestPaths(vector<ResultItem > &d) {
	ResultItem d1;
    int idx = GetIdx(end, 0);
	//cout << end << " " << this->root_type << " " << idx << endl;
	for(int k = 0; k < K; k++) {
		/*cout << k;
		cout << " prob:" << chart.at(idx+k).prob ;
		cout << endl;*/
		if(chart.at(idx+k).prob != MINVALUE){
			FVLink fvlink;
			GetFeatureVector(chart.at(idx+k),fvlink);
			fvlink.ToFV( d1.fv);
			//cout << "Getting Rl" << endl;
    		d1.parse_tree = GetDepString(chart.at(idx+k));
			d1.score = chart.at(idx+k).prob;
    		d.push_back(d1);
		}
	}
}

void TagSpan::GetFeatureVector(SpanItem &si, FVLink &fvlink) {
	//cout << si.s << " " <<" " << si.type << " " << si.prob<< " " << endl;
	
	fvlink.Add(si.fvlink);
	if(si.pleft == NULL){
	//if(si.next.size() == 0){
		return;
	}
	//cout << "LEFT:" << si.next[0].s << " " << si.next[0].t << " " << si.next[0].dir << " " << si.next[0].comp << " " << endl;
	//cout << "RIGHT:" << si.next[1].s << " " << si.next[1].t << " " << si.next[1].dir << " " << si.next[1].comp << " " << endl;
	GetFeatureVector(*si.pleft, fvlink);//left
}

string TagSpan::GetDepString(SpanItem &si) {
	//cout << si.s << " " << si.r <<" " << si.t << " " << si.dir << " " << si.comp << " " << endl;
	stringstream relA;
	string s = "";
	relA << si.type;
	s = relA.str();
	if(si.pleft == NULL)
	    return "";//
	string sL = GetDepString(*si.pleft);
	if(sL == "")
		s = relA.str();
	else
		s = sL+(string)" " + relA.str();
	//cout << si.s << " " << si.r <<" " << si.t << " " << si.dir << " " << si.comp << " " << s<< endl;
	return s;
}
	

	
    // returns pairs of indeces and -1,-1 if < K pairs
void TagSpan::GetKBestPairs(vector<SpanItem*> &items1, vector<SpanItem*> &items2, vector<pair<int, int> > &kbest) {

	kbest.clear();
	
	if(items1.size() == 0 || items2.size() == 0)
		return;

	

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
		if(hi.val < MINVALUE && hi.i1 == -1)
			break;
		onepair.first = hi.i1;
		onepair.second = hi.i2;
		kbest.push_back(onepair);
		n++;
		if(n >= K)
			break;
		if(beenvisited.at((hi.i1+1)*K+hi.i2) != 1){
			beenvisited.at((hi.i1+1)*K+hi.i2) = 1;
			double probA = items1.at(hi.i1+1)->prob + items2.at(hi.i2)->prob;
			hi.i1 += 1; hi.val = probA;
			hp.Add(hi);
		}
		if(beenvisited.at(hi.i1*K+hi.i2+1) != 1){
            beenvisited.at((hi.i1)*K+hi.i2+1) = 1;
			double probB = items1.at(hi.i1)->prob + items2.at(hi.i2+1)->prob;
			hi.i2+=1; hi.val = probB;
            hp.Add(hi);
        }

	}
}
