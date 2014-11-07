#include "Decoder.h"
#include "ParseForest.h"
#include "SpanItem.h"
void Decoder::ResetPairs(vector<pair<int, int> > &pairs){
    for(int k = 0; k < pairs.size(); k++){
        pairs[k].first = -1;
        pairs[k].second = -1;
    }
}
//labeled parsing
void Decoder::GetDepLabels(vector<double> &nt_probs, int len, Pipe &pipe, 
                           vector<int> &static_labels, vector<double> &static_scores) {
	static_labels.clear();
	static_scores.clear();
	static_labels.reserve(len*len);
	static_scores.reserve(len*len);
	int types_num= pipe.types_num;
	for(int i = 0; i < len; i++) {
	    for(int j = 0; j < len; j++) {
		//i-ch, j-hd
		if(i == j) {
			static_labels.push_back(0); 
			static_scores.push_back(0.0);
			continue; 
		}
		int wh = -1;
		double best = MINVALUE;
		for(int t = 0; t < types_num; t++) {
		    double score = 0.0;
			int vidx_i = pipe.GetLBFVIdx(len, j, i, t);
			score = nt_probs[vidx_i];
		    if(score > best) { wh = t; best = score; }
		}
		static_labels.push_back( wh);
		static_scores.push_back(best);
	    }
	}
}

void Decoder::DecodeProjective(Instance &instance,vector<FeatureVector> &fvs,  vector<double> &probs,
                            vector<FeatureVector> &nt_fvs,vector<double> &nt_probs,
                            int K, vector<ResultItem > &d, Pipe &pipe){
	int instance_length = instance.word_num;
	//labeled parsing
	vector<int> static_labels;
	vector<double> static_scores;
	bool labeled_parsing = pipe.poptions->labeled_parsing;
	if(labeled_parsing)
		GetDepLabels(nt_probs, instance_length, pipe, static_labels, static_scores);	


	//cout << "ILen=" << instance_length << endl;
	ParseForest pf(0, instance_length-1, K);
	FeatureVector tmpfv;
	for(int s = 0; s < instance_length; s++) {
	    pf.Add(s,-1,0,0.0);
	    pf.Add(s,-1,1,0.0);
	}
	vector<SpanItem*> b1;
	b1.reserve(K);
	vector<SpanItem*> c1;
	c1.reserve(K);
	vector<pair<int, int> > pairs;
	pairs.reserve(K);
    for(int k = 0; k < K; k++){
        double val = MINVALUE;
        b1.push_back(NULL);
        c1.push_back(NULL);
        pair<int, int> p;
        p.first = -1;
        p.second = -1;
        pairs.push_back(p);
    }
	FVLink fv_fin;
	//cout << "Endof" << endl;
	for(int j = 1; j < instance_length; ++j) {
	  	for(int s = 0; s < instance_length && s+j < instance_length; s++) {
		
		int t = s+j;
		//int vect_idx = 2*((s*instance_length - (s+1)*s/2) + j-1);
		int vect_idx = pipe.GetFVIdx(instance_length, s,  t);
		//cout << s <<" " << t << " "<< 0 <<" idx:" <<vect_idx <<endl;
		//FeatureVector *prodFV_st = &fvs.at(vect_idx + 0);
		//FeatureVector *prodFV_ts = &fvs.at(vect_idx + 1);
		double prodProb_st = probs.at(vect_idx + 0);
		double prodProb_ts = probs.at(vect_idx + 1);
		//cout << s <<" " << t << " "<< 0 <<" idx:" <<vect_idx << " st:" << prodProb_st << " ts:"<< prodProb_ts <<endl;
		int labelst = 0;
		int labelts = 0;
		double scorest = 0.0;
		double scorets = 0.0;		
		if(labeled_parsing){
			labelst = static_labels[t*instance_length+s];//t is a child
			scorest = static_scores[t*instance_length+s];
			labelts = static_labels[s*instance_length+t];//s is a child
			scorets = static_scores[s*instance_length+t];
		}
		
					
		double prodProb = 0.0;
		//for incomplete items		
		for(int r = s; r <= t; r++) {
					
		    /** first is direction, second is complete=0*/
		    /** _s means s is the parent*/
		    if(r != t) {
			pf.GetItems(s,r,0,0,K, b1);
			pf.GetItems(r+1,t,1,0,K, c1);
	//		cout << s <<" "<< r << " " << t << " "<< b1.size() << "=" << c1.size() << endl; 			
			if(b1.size() != 0 && c1.size() != 0) {
				//vector<pair<int, int> > pairs;
				ResetPairs(pairs);
			    pf.GetKBestPairs(b1,c1, pairs);
			    for(int k = 0; k < pairs.size(); k++) {
	//			cout << s <<" "<< r << " " << t << " "<< b1.size() << "a=K=" << c1.size() << endl; 			
								
				int comp1 = pairs.at(k).first; int comp2 = pairs.at(k).second;
				if(comp1 <0 || comp2 < 0)
                    break;				
				double bc = b1[comp1]->prob+c1[comp2]->prob;
				// ->				
				double prob_fin = bc+prodProb_st;
				//FVLink fv_fin;
				//fv_fin.Add(prodFV_st);
				if(labeled_parsing) {
				    prob_fin += scorest;
				}
				pf.Add(s,r,t,labelst,0,1,prob_fin,fv_fin,b1[comp1],c1[comp2]);
				// <-
				double prob_fin2 = bc+prodProb_ts;
				//FVLink fv_fin2;
				//fv_fin2.Add(prodFV_ts);
				if(labeled_parsing) {
                    prob_fin2 += scorets;
                }

				pf.Add(s,r,t,labelts,1,1,prob_fin2,fv_fin,b1[comp1],c1[comp2]);

			    }
			}						
		    }					
		}
				
				
		for(int r = s; r <= t; r++) {
					
		    if(r != s) {
            pf.GetItems(s,r,0,1,K, b1);
            pf.GetItems(r,t,0,0,K, c1);

			if(b1.size() != 0 && c1.size() != 0) {
                //vector<pair<int, int> > pairs;
				ResetPairs(pairs);
                pf.GetKBestPairs(b1,c1, pairs);
                for(int k = 0; k < pairs.size(); k++) {

                int comp1 = pairs.at(k).first; int comp2 = pairs.at(k).second;
				if(comp1 <0 || comp2 < 0)
                    break;
                double bc = b1[comp1]->prob+c1[comp2]->prob;
 
									
				//FVLink fv;					
				if(!pf.Add(s,r,t,-1,0,0,bc,
					   fv_fin,
					   b1[comp1],c1[comp2]))
				    break;
			    }
			}
		    }
						
		    if(r != t) {
            pf.GetItems(s,r,1,0,K, b1);
            pf.GetItems(r,t,1,1,K, c1);
			if(b1.size() != 0 && c1.size() != 0) {
                //vector<pair<int, int> > pairs;
				ResetPairs(pairs);
                pf.GetKBestPairs(b1,c1, pairs);
                for(int k = 0; k < pairs.size(); k++) {

                int comp1 = pairs.at(k).first; int comp2 = pairs.at(k).second;
				if(comp1 <0 || comp2 < 0)
                    break;
                double bc = b1[comp1]->prob+c1[comp2]->prob;


                //FVLink fv;
									
				if(!pf.Add(s,r,t,-1,1,0,bc,
					   fv_fin,b1[comp1],c1[comp2]))
				    break;
			    }
			}
		    }
		}
				
	    }
	}
	//return;
	pf.GetBestParses(d);
	return;
}

void Decoder::DecodeBeam(Instance &instance,vector<FeatureVector> &fvs,  vector<double> &probs,
                            vector<FeatureVector> &fvs_trip,vector<double> &probs_trip,
                            vector<FeatureVector> &fvs_sib,vector<double> &probs_sib,
                            vector<FeatureVector> &fvs_gc,vector<double> &probs_gc,
                            vector<int> &trip_ids,vector<int> &sib_ids,vector<int> &gc_ids,
                            vector<FeatureVector> &nt_fvs,vector<double> &nt_probs,
                            int K, vector<ResultItem > &d, Pipe &pipe, Parameters &params){
	int instance_length = instance.word_num;
	//labeled parsing
    vector<int> static_labels;
    vector<double> static_scores;
    bool labeled_parsing = pipe.poptions->labeled_parsing;
    if(labeled_parsing)
        GetDepLabels(nt_probs, instance_length, pipe, static_labels, static_scores);

	//cout << "ILen=" << instance_length << endl;
	ParseForest pf(0, instance_length-1, K);
	FeatureVector tmpfv;
	for(int s = 0; s < instance_length; s++) {
	    pf.Add(s,-1,0,0.0);
	    pf.Add(s,-1,1,0.0);
	}
	vector<SpanItem*> b1;
	b1.reserve(K);
	vector<SpanItem*> c1;
	c1.reserve(K);
	vector<pair<int, int> > pairs;
	pairs.reserve(K);
    for(int k = 0; k < K; k++){
        double val = MINVALUE;
        b1.push_back(NULL);
        c1.push_back(NULL);
        pair<int, int> p;
        p.first = -1;
        p.second = -1;
        pairs.push_back(p);
    }
	//reuse
	int   size_gsib = pipe.GetGSibSize(instance_length);
	int   size_3sib = pipe.GetTriSibSize(instance_length);
	bool  flag_reuse = true;//if too large, giveup
	if(instance_length > 80){
		size_gsib = 100;
		size_3sib = 100;	
		flag_reuse = false;
	}
	double *probs_gsib = new double[size_gsib];
	double *probs_3sib = new double[size_3sib]; 
	for(int i = 0; i < size_gsib; i++)
		*(probs_gsib+i)	= MINVALUE;
	for(int i = 0; i < size_3sib; i++)
        *(probs_3sib+i) = MINVALUE;
	
	FVLink fv_fin;
    FeatureVector fv;//for reuse
	//cout << "Endof" << endl;
	for(int j = 1; j < instance_length; ++j) {
	  	for(int s = 0; s < instance_length && s+j < instance_length; s++) {
		
		int t = s+j;
		//int vect_idx = 2*((s*instance_length - (s+1)*s/2) + j-1);
		int vect_idx = pipe.GetFVIdx(instance_length, s,  t);
		//cout << s <<" " << t << " "<< 0 <<" idx:" <<vect_idx <<endl;
		//FeatureVector *prodFV_st = &fvs.at(vect_idx + 0);
		//FeatureVector *prodFV_ts = &fvs.at(vect_idx + 1);
		double prodProb_st = probs.at(vect_idx + 0);
		double prodProb_ts = probs.at(vect_idx + 1);
		//cout << s <<" " << t << " "<< 0 <<" idx:" <<vect_idx << " st:" << prodProb_st << " ts:"<< prodProb_ts <<endl;
		int labelst = 0;
        int labelts = 0;
        double scorest = 0.0;
        double scorets = 0.0;
        if(labeled_parsing){
            labelst = static_labels[t*instance_length+s];//t is a child
            scorest = static_scores[t*instance_length+s];
            labelts = static_labels[s*instance_length+t];//s is a child
            scorets = static_scores[s*instance_length+t];
        }

		double prodProb = 0.0;
		//for incomplete items		
		for(int r = s; r <= t; r++) {
					
		    /** first is direction, second is complete=0*/
		    /** _s means s is the parent*/
		    if(r != t) {
			pf.GetItems(s,r,0,0,K, b1);
			pf.GetItems(r+1,t,1,0,K, c1);
	//		cout << s <<" "<< r << " " << t << " "<< b1.size() << "=" << c1.size() << endl; 			
			if(b1.size() != 0 && c1.size() != 0) {
				//vector<pair<int, int> > pairs;
				ResetPairs(pairs);
			    pf.GetKBestPairs(b1,c1, pairs);
			    for(int k = 0; k < pairs.size(); k++) {
	//			cout << s <<" "<< r << " " << t << " "<< b1.size() << "a=K=" << c1.size() << endl; 			
								
				int comp1 = pairs.at(k).first; int comp2 = pairs.at(k).second;
				if(comp1 <0 || comp2 < 0)
                    break;				
				double bc = b1[comp1]->prob+c1[comp2]->prob;
				// ->				
				double prob_fin = bc+prodProb_st;
				//*add higher-order feats
				int b1r = b1[comp1]->r;
				int c1r = c1[comp2]->r;
				{
				//sib
				int sib = b1r;
				int idx3_1 = pipe.GetFVTripIdx(instance_length,  s, sib, t);
                int idx2_1 = pipe.GetFVSIBIdx(instance_length,   sib, t, 0);
                int idx3 = trip_ids[idx3_1];
                int idx2 = sib_ids[idx2_1];
                double prob_trip = probs_trip.at(idx3);
                double prob_sib = sib == s? probs_sib.at(idx2):probs_sib.at(idx2+1);
				prob_fin += prob_trip + prob_sib;
				}
				{
				//gc
				int gc = c1r;
				if(gc == t)
					gc = -1;
				int idx3_1 = pipe.GetFVGCIdx(instance_length,  s, t, gc);
                int idx3 = gc_ids[idx3_1];
                double prob_gc = probs_gc.at(idx3);
				prob_fin += prob_gc;
				}
				{//higher-order features
				int gc = -1;
				int gc1 = -1;
				if(c1r != t){
				gc = c1r;
				vector<SpanItem>::iterator p =  pf.GetItem(c1r,t,1,1);
				int pr = p->r;
				p =  pf.GetItem(pr+1,t,1,0);
				if(p->r != t){
					gc1 = gc;
					gc = p->r;
				}
				}
				double prob_gsib = 0.0;
				int idx_gsib = pipe.GetGSibIdx(instance_length, s, t, gc, gc1);
				if(!flag_reuse || *(probs_gsib+idx_gsib) == MINVALUE){
				//FeatureVector fv;
				fv.Clear();
				pipe.AddGSibFeatures(instance, s, t, gc, gc1, fv);
				prob_gsib = fv.GetScore(params);
				if(flag_reuse)
				*(probs_gsib+idx_gsib) = prob_gsib;
				}
				else
					prob_gsib = *(probs_gsib+idx_gsib);
				//
				int ch2 = b1r;
				int ch1 = s;
				if(ch2 != s){
				vector<SpanItem>::iterator p =  pf.GetItem(s,b1r,0,1);
				int pr = p->r;
				p =  pf.GetItem(s,pr,0,0);
				ch1 = p->r;
				}
				double prob_3sib = 0.0;
				int idx = pipe.GetTriSibIdx(instance_length, s, ch1, ch2, t);
				if(!flag_reuse || *(probs_3sib+idx) == MINVALUE){
				//FeatureVector fv;
				fv.Clear();
				pipe.AddTriSibFeatures(instance, s, ch1, ch2, t, fv);
				prob_3sib = fv.GetScore(params);
				if(flag_reuse)
				*(probs_3sib+idx) = prob_3sib;
				}
				else
					prob_3sib = *(probs_3sib+idx);
				//cout << s << " " << ch1 << " " << ch2 << " " << t << " Score:" <<prob_high << endl;
				prob_fin += prob_gsib+prob_3sib;
				}
				//FVLink fv_fin;
				//fv_fin.Add(prodFV_st);
                if(labeled_parsing) {
                    prob_fin += scorest;
                }
				pf.Add(s,r,t,labelst,0,1,prob_fin,fv_fin,b1[comp1],c1[comp2]);

				// <-
				double prob_fin2 = bc+prodProb_ts;
				{
                //sib
				int sib = c1r;
                int idx3_1 = pipe.GetFVTripIdx(instance_length,  t, sib, s);
                int idx2_1 = pipe.GetFVSIBIdx(instance_length,   sib, s, 0);
                int idx3 = trip_ids[idx3_1];
                int idx2 = sib_ids[idx2_1];
                double prob_trip = probs_trip.at(idx3);
                double prob_sib = sib == t? probs_sib.at(idx2):probs_sib.at(idx2+1);
                prob_fin2 += prob_trip + prob_sib;
                }
                {
                //gc
                int gc = b1r;
                if(gc == s)
                    gc = -1;
                int idx3_1 = pipe.GetFVGCIdx(instance_length,  t, s, gc);
                int idx3 = gc_ids[idx3_1];
                double prob_gc = probs_gc.at(idx3);
                prob_fin2 += prob_gc;
                }
				{//higher-order features
				
                int gc = -1;
                int gc1 = -1;
                if(b1r != s){
				gc = b1r;
                vector<SpanItem>::iterator p =  pf.GetItem(s,b1r,0,1);
                int pr = p->r;
                p =  pf.GetItem(s,pr,0,0);
				if(p->r != s){
					gc1 = gc;
					gc = p->r;
				}
                }
				double prob_gsib = 0.0;
                int idx_gsib = pipe.GetGSibIdx(instance_length, t, s, gc, gc1);
                if(!flag_reuse || *(probs_gsib+idx_gsib) == MINVALUE){
                //FeatureVector fv;
				fv.Clear();
                pipe.AddGSibFeatures(instance, t, s, gc, gc1, fv);
                prob_gsib = fv.GetScore(params);
				if(flag_reuse)
                *(probs_gsib+idx_gsib) = prob_gsib;
                }
                else
                    prob_gsib = *(probs_gsib+idx_gsib);
                //
                int ch2 = c1r;
                int ch1 = t;
                if(ch2 != t){
                vector<SpanItem>::iterator p =  pf.GetItem(c1r,t,1,1);
                int pr = p->r;
                p =  pf.GetItem(pr+1,t,1,0);
                ch1 = p->r;
                }
                //pipe.AddTriSibFeatures(instance, t, ch1, ch2, s, fv);

				double prob_3sib = 0.0;
                int idx = pipe.GetTriSibIdx(instance_length, t, ch1, ch2, s);
                if(!flag_reuse || *(probs_3sib+idx) == MINVALUE){
                //FeatureVector fv;
				fv.Clear();
                pipe.AddTriSibFeatures(instance, t, ch1, ch2, s, fv);
                prob_3sib = fv.GetScore(params);
				if(flag_reuse)
                *(probs_3sib+idx) = prob_3sib;
                }
                else
                    prob_3sib = *(probs_3sib+idx);
                //cout << s << " " << ch1 << " " << ch2 << " " << t << " Score:" <<prob_high << endl;
                prob_fin2 += prob_gsib+prob_3sib;
                }

				//FVLink fv_fin2;
				//fv_fin2.Add(prodFV_ts);
                if(labeled_parsing) {
                    prob_fin2 += scorets;
                }
				pf.Add(s,r,t,labelts,1,1,prob_fin2,fv_fin,b1[comp1],c1[comp2]);

			    }
			}						
		    }					
		}
				
				
		for(int r = s; r <= t; r++) {
					
		    if(r != s) {
            pf.GetItems(s,r,0,1,K, b1);
            pf.GetItems(r,t,0,0,K, c1);

			if(b1.size() != 0 && c1.size() != 0) {
                //vector<pair<int, int> > pairs;
				ResetPairs(pairs);
                pf.GetKBestPairs(b1,c1, pairs);
                for(int k = 0; k < pairs.size(); k++) {

                int comp1 = pairs.at(k).first; int comp2 = pairs.at(k).second;
				if(comp1 <0 || comp2 < 0)
                    break;
                double bc = b1[comp1]->prob+c1[comp2]->prob;
 				{
                //gc
                int gc = c1[comp2]->r;
                if(gc == r)
                    gc = -2;
                int idx3_1 = pipe.GetFVGCIdx(instance_length,  s, r, gc);
                int idx3 = gc_ids[idx3_1];
                double prob_gc = probs_gc.at(idx3);
                bc += prob_gc;
                }
				{//higher-order features
				int gc = -2;
                int gc1 = -2;
                if(c1[comp2]->r != r){
				gc = c1[comp2]->r;
                vector<SpanItem>::iterator p =  pf.GetItem(r,gc,0,1);
                int pr = p->r;
                p =  pf.GetItem(r,pr,0,0);
				if(p->r != r){
					gc1 = gc;
					gc = p->r;
				}
                }
                //pipe.AddGSibFeatures(instance, s, r, gc, gc1, fv);
                //
				double prob_gsib = 0.0;
                int idx_gsib = pipe.GetGSibIdx(instance_length, s, r, gc, gc1);
                if(!flag_reuse || *(probs_gsib+idx_gsib) == MINVALUE){
				fv.Clear();
                pipe.AddGSibFeatures(instance, s, r, gc, gc1, fv);
                prob_gsib = fv.GetScore(params);
				if(flag_reuse)
                *(probs_gsib+idx_gsib) = prob_gsib;
                }
                else
                    prob_gsib = *(probs_gsib+idx_gsib);
				bc += prob_gsib;
				}				
				//FVLink fv;					
				if(!pf.Add(s,r,t,-1,0,0,bc,
					   fv_fin,
					   b1[comp1],c1[comp2]))
				    break;
			    }
			}
		    }
						
		    if(r != t) {
            pf.GetItems(s,r,1,0,K, b1);
            pf.GetItems(r,t,1,1,K, c1);
			if(b1.size() != 0 && c1.size() != 0) {
                //vector<pair<int, int> > pairs;
				ResetPairs(pairs);
                pf.GetKBestPairs(b1,c1, pairs);
                for(int k = 0; k < pairs.size(); k++) {

                int comp1 = pairs.at(k).first; int comp2 = pairs.at(k).second;
				if(comp1 <0 || comp2 < 0)
                    break;
                double bc = b1[comp1]->prob+c1[comp2]->prob;

				{
                //gc
                int gc = b1[comp1]->r;
                if(gc == r)
                    gc = -2;
                int idx3_1 = pipe.GetFVGCIdx(instance_length,  t, r, gc);
                int idx3 = gc_ids[idx3_1];
                double prob_gc = probs_gc.at(idx3);
                bc += prob_gc;
                }
				{//higher-order features
                int gc = -2;
                int gc1 = -2;
                if(b1[comp1]->r != r){
				gc = b1[comp1]->r;
                vector<SpanItem>::iterator p =  pf.GetItem(gc,r,1,1);
                int pr = p->r;
                p =  pf.GetItem(pr+1,r,1,0);	
				if(p->r != r){
					gc1 = gc;
					gc = p->r;
				}
                }
                //pipe.AddGSibFeatures(instance, t, r, gc, gc1, fv);
				double prob_gsib = 0.0;
                int idx_gsib = pipe.GetGSibIdx(instance_length, t, r, gc, gc1);
                if(!flag_reuse || *(probs_gsib+idx_gsib) == MINVALUE){
                //FeatureVector fv;
				fv.Clear();
                pipe.AddGSibFeatures(instance, t, r, gc, gc1, fv);
                prob_gsib = fv.GetScore(params);
				if(flag_reuse)
                *(probs_gsib+idx_gsib) = prob_gsib;
                }
                else
                    prob_gsib = *(probs_gsib+idx_gsib);
                bc += prob_gsib;
                }

                //FVLink fv;
									
				if(!pf.Add(s,r,t,-1,1,0,bc,
					   fv_fin,b1[comp1],c1[comp2]))
				    break;
			    }
			}
		    }
		}
				
	    }
	}
	//return;
	pf.GetBestParses(d);
	delete probs_gsib;
	delete probs_3sib;
	return;
}
