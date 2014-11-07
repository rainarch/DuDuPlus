#include "ParseForestO2.h"
#include <float.h>
#define MINVALUE -DBL_MAX
ParseForestO2::ParseForestO2(int start, int end, int K) {
	this->K = K;
	this->start = start;
	this->end = end;
	
	{
	int size = (end+1)*(end+1)*2*K;
	chartO.reserve(size);
	int idx = 0;
	FeatureVector fv;
	for(int s1 =0; s1 < end+1; ++s1){
	for(int t1 =0; t1 < end+1; ++t1){
	for(int dir1 =0; dir1 < 2; ++dir1){
	int idxO = GetIdxO(s1, t1,  dir1);
	//cout << idx << " vs " << idxO << endl;
	for(int k1 = 0; k1 < K; ++k1){
		SpanItem si(s1, -1, dir1, MINVALUE);
		si.t = t1;
		si.comp = 0;
		si.fvlink.Reserve(t1-s1);
		chartO.push_back(si);
		++idx;
	}}}}
	}
	//for complete
	{
    int size = (end+1)*(end+1)*(end+1)*2*K;
    chartC.reserve(size);
    int idx = 0;
    FeatureVector fv;
    for(int s1 =0; s1 < end+1; ++s1){
    for(int t1 =0; t1 < end+1; ++t1){
    for(int r1 =0; r1 < end+1; ++r1){
    for(int dir1 =0; dir1 < 2; ++dir1){
	//cout << s1 << " " << t1 << " " << r1 << " "<< dir1 << endl;
	//int idxC = GetIdxC(s1, t1, r1, dir1);
	//cout << idx << " vs " << idxC << endl;
    for(int k1 = 0; k1 < K; ++k1){
        SpanItem si(s1, -1, dir1, MINVALUE);
        si.t = t1;
        si.r = r1;
        si.comp = 0;
        si.fvlink.Reserve(t1-s1);
        chartC.push_back(si);
        ++idx;
    }}}}}
    }
	{
	chartCK.reserve(K);
	for(int k1 = 0; k1 < K; ++k1){
		SpanItem si(0, -1, 0, MINVALUE);
        si.t = end;
        si.r = 0;
        si.comp = 0;
        si.fvlink.Reserve(end);
        chartCK.push_back(si);
	}
	}
}


int ParseForestO2::GetIdxO(int s, int t, int dir){
	return (s*(this->end+1)*2+ t*2+ dir)*K; 
}
int ParseForestO2::GetIdxC(int s, int t, int r, int dir){
	return (s*(this->end+1)*(this->end+1)*2+ t*(this->end+1)*2+r*2+ dir)*K; 
}

bool ParseForestO2::Add(int s, int type, int dir, double score) {
    bool added = false;
    int idx = GetIdxC(s, s, s, dir);
    if(chartC.at(idx+K-1).prob > score)
        return false;
    //cout << s << " " << type << " " << dir << " " << score << "Adding"<< endl;
    for(int i = 0; i < K; i++) {
        if(chartC.at(idx+i).prob < score){
            if(chartC.at(idx+i).prob == MINVALUE){
                chartC.at(idx+i).Reset(s, type, dir, score);
            }
            else{
            //SpanItem tmp(MINVALUE);
            chartC.at(idx+i).CopyValuesTo(tmp);
            chartC.at(idx+i).Reset(s, type, dir, score);
            for(int j = i+1; j < K && tmp.prob != MINVALUE; j++){
                SpanItem tmp1;
                chartC.at(idx+j).CopyValuesTo(tmp1);
                tmp.CopyValuesTo(chartC.at(idx+j));
                tmp1.CopyValuesTo(tmp);
            }
            }
            added = true;
            break;
        }
    }

    return added;
}
bool ParseForestO2::AddToKItems(SpanItem *p1, double prob, vector<SpanItem *> &sitems , vector<double> &probs){
	//bool added = true;
	bool added = false;
	int csize = probs.size();
/*	if(csize == 0){
		sitems.push_back(p1);
		probs.push_back(prob);
		return true;
	}
	if(csize == K && probs.at(csize-1) > prob)
		return added;//false
	if(csize < K && probs.at(csize-1) > prob){
		sitems.push_back(p1);
        probs.push_back(prob);
        return true;
	}
*/	
	//cout << "Newadded:" << prob << endl;
	for(int k = 0; k < K && k < csize ;k++){
		if(probs.at(k) < prob){
			if(probs.at(k) == MINVALUE){
				probs.at(k) = prob;
				sitems.at(k) = p1;
			}
			else{
				SpanItem *pT = sitems.at(k);
				double   probT = probs.at(k);
				probs.at(k) = prob;
                sitems.at(k) = p1;
				for(int j = k+1; j < K && probT != MINVALUE;j++){
					SpanItem *pT1 = sitems.at(j);
	                double   probT1 = probs.at(j);
					probs.at(j) = probT;
	                sitems.at(j) = pT;			
					pT = pT1;
					probT = probT1;
				}		
			}
			added = true;
		//	cout << "added at " << k << endl;
			break;
		}
	}
	for(int k = 0; k < K && k < csize ;k++){
		if(probs.at(k) == MINVALUE) break;
		//cout << "NK:"<< k << " "<< probs.at(k) << " " << sitems.at(k)->s << " " << sitems.at(k)->r << " " << sitems.at(k)->t << endl;
	}
	return added;
}
/*

bool ParseForestO2::Add(int s, int type, int dir, double score, FVLink &fvlink) {


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
*/

bool ParseForestO2::AddO(int s, int r, int t, int type,
		       int dir, int comp, double score,
		       FVLink &fvlink,
		       SpanItem *p1, SpanItem *p2) {
	//cout << "AddO:" << s << " "<< r << " " << t << " " << score << endl;
	bool added = false;
	int idx = GetIdxO(s, t, dir);
    if(chartO.at(idx+K-1).prob > score)
        return false;
    for(int i = 0; i < K; i++) {
		vector<SpanItem>::iterator cur_it = chartO.begin()+(idx+i);
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
                chartO.at(idx+j).CopyValuesTo(tmp1);
                tmp.CopyValuesTo(chartO.at(idx+j));
                tmp1.CopyValuesTo(tmp);
            }
			}
            added = true;
            break;
			
        }
    }

    return added;	
}
bool ParseForestO2::AddC(int s, int r, int t, int type,
               int dir, int comp, double score,
               FVLink &fvlink,
               SpanItem *p1, SpanItem *p2) {
	//cout << "AddC:" << s << " "<< r << " " << t << " "<< dir<< " " << score << endl;
    bool added = false;
    int idx = GetIdxC(s, t,r, dir);
    if(chartC.at(idx+K-1).prob > score)
        return false;
    for(int i = 0; i < K; i++) {
        vector<SpanItem>::iterator cur_it = chartC.begin()+(idx+i);
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
                chartC.at(idx+j).CopyValuesTo(tmp1);
                tmp.CopyValuesTo(chartC.at(idx+j));
                tmp1.CopyValuesTo(tmp);
            }
            }
            added = true;
            break;

        }
    }
	//cout << "IsFINAL?:" << s << " " << t << " " << score << endl;
	if(added && s == 0 && t == end && dir == 0){//only for 0-end
		//cout << "FINAL:" << s << " " << t << " " << score << endl;
		if(chartCK.at(K-1).prob > score)
			return added;
		for(int i = 0; i < K; i++) {
	        vector<SpanItem>::iterator cur_it = chartCK.begin()+(i);
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
	                chartCK.at(j).CopyValuesTo(tmp1);
	                tmp.CopyValuesTo(chartCK.at(j));
	                tmp1.CopyValuesTo(tmp);
	            }
	            }
	            added = true;
	            break;
	
	        }
	    }
	
	}
    return added;
}

/*
double ParseForestO2::GetProb(int s, int t, int dir, int comp) {
	return GetProb(s,t,dir,comp,0);
}

double ParseForestO2::GetProb(int s, int t, int dir, int comp, int i) {
	int idx = GetIdx(s,t,dir, comp);
	return chart.at(idx+i).prob;
}

void ParseForestO2::GetProbs(int s, int t, int dir, int comp, vector<double> &result) {
	result.clear();
	int idx = GetIdx(s,t,dir, comp);
	for(int i = 0; i < K; i++)
		result.push_back(chart.at(idx+i).prob);
}

vector<SpanItem>::iterator ParseForestO2::GetItem(int s, int t, int dir, int comp) {
	return GetItem(s,t,dir,comp,0);
}

vector<SpanItem>::iterator ParseForestO2::GetItem(int s, int t, int dir, int comp, int k) {
	int idx = GetIdx(s,t,dir, comp);
	return chart.begin()+(idx+k);
}
*/
void ParseForestO2::GetItemsC(int s, int t, int r,  int dir, vector<SpanItem *> &sitems) {
	//vector<SpanItem>::iterator it = GetItem(s, t, dir, comp);
	int idx = GetIdxC(s,t,r, dir);
	//sitems.clear();
	//sitems.reserve(K);
	for(int i = 0; i < K; ++i){
		SpanItem *p = &chartC[idx+i];
		sitems.at(i) = p;
		//if(p->prob == MINVALUE)
		//	break;
	}
	if(sitems.size() == 0)
		cerr << "Err: getItemsC 0" << endl;
}
void ParseForestO2::GetItemsO(int s, int t,  int dir, vector<SpanItem *> &sitems) {
    //vector<SpanItem>::iterator it = GetItem(s, t, dir, comp);
    int idx = GetIdxO(s,t, dir);
    //sitems.clear();
    //sitems.reserve(K);
    for(int i = 0; i < K; ++i){
        SpanItem *p = &chartO[idx+i];
        sitems.at(i) = (p);
		//if(p->prob == MINVALUE)
		//	break;
    }
	if(sitems.size() == 0)
		cerr << "Err: getItemsO 0" << endl;
}

/*
void ParseForestO2::GetBestParse(vector<pair<FeatureVector, string> > &d) {
	pair<FeatureVector, string> d1;
	int idx = GetIdx(0, end, 0,0);
	FVLink fvlink;
	GetFeatureVector(chart.at(idx+0), fvlink);
	fvlink.ToFV( d1.first);
	d1.second = GetDepString(chart.at(idx+0));
	d.push_back(d1);
}
*/
void ParseForestO2::GetBestParses(vector<ResultItem > &d) {

    int idx = 0;//chartCK
	//cout << idx << endl;
	for(int k = 0; k < K; k++) {
	//	cout << k;
	//	cout << " prob:" << chartCK.at(idx+k).prob << " ";
	//	cout << endl;
		if(chartCK.at(idx+k).prob != MINVALUE){
			ResultItem d1;
			//FVLink fvlink;
			//GetFeatureVector(chartCK.at(idx+k),fvlink);
			//fvlink.ToFV( d1.fv);
			//cout << "Getting Rl" << endl;
    		d1.parse_tree = GetDepString(chartCK.at(idx+k));
			d1.score = chartCK.at(idx+k).prob;
    		d.push_back(d1);
		}
	}
}

void ParseForestO2::GetFeatureVector(SpanItem &si, FVLink &fvlink) {
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

string ParseForestO2::GetDepString(SpanItem &si) {
	//cout << si.s << " " << si.r <<" " << si.t << " " << si.dir << " " << si.comp << " " << si.prob<< endl;
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
void ParseForestO2::GetKBestPairs2(vector<SpanItem*> &items1, vector<SpanItem*> &items2, 
                                   vector<double> &probs1,  vector<double> &probs2, vector<pair<int, int> > &kbest) {

	//kbest.clear();
	
	if(items1.size() == 0 || items2.size() == 0)
		return;
	if(K == 1){
		kbest[0].first = 0;
		kbest[0].second = 0;
		return;
	}
	

	vector<int> beenvisited;
	//pair<int, int > onepair;

	beenvisited.resize(K*K);
	Heap hp(K+1);
	HeapItem hi(probs1.at(0) + probs2.at(0), 0,0);
	//hi.i1 = 0; hi.i2 =0; hi.val = items1.at(0).prob + items2.at(0).prob;
	hp.Add(hi);
 	beenvisited.at(0*K+0) = 1;
	int n = 0;
	
	while(n < K){
		hp.RemoveMax(hi);
		if(hi.i1 == -1 || hi.val <= MINVALUE)
			break;
		//onepair.first = hi.i1;
		//onepair.second = hi.i2;
		//cout << hi.i1 << " " << hi.i2 << " " << hi.val << endl;
		//kbest.push_back(onepair);
		kbest[n].first = hi.i1;
		kbest[n].second = hi.i2;
		n++;
		if(n >= K)
			break;
		if(beenvisited.at((hi.i1+1)*K+hi.i2) != 1 && (hi.i1+1) < probs1.size() && probs1.at(hi.i1+1) != MINVALUE){
			beenvisited.at((hi.i1+1)*K+hi.i2) = 1;
			double probA = probs1.at(hi.i1+1) + probs2.at(hi.i2);
			//cout << probA << " i1+1" << endl;
			HeapItem hiT(probA, hi.i1+1,hi.i2);
			hp.Add(hiT);
		}
		if(beenvisited.at(hi.i1*K+hi.i2+1) != 1 && (hi.i2+1) < probs2.size() && probs2.at(hi.i2+1) != MINVALUE){
            beenvisited.at((hi.i1)*K+hi.i2+1) = 1;
			double probB = probs1.at(hi.i1) + probs2.at(hi.i2+1);
			//cout << probB << " i2+1 " << endl;
			HeapItem hiT(probB, hi.i1,hi.i2+1);
            hp.Add(hiT);
        }

	}
	for(int k = 0; k < K;++k){
		//cout << k << " bestPair: " << kbest[k].first << " " << kbest[k].second << endl;
	}
}

