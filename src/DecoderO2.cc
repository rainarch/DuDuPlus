#include "DecoderO2.h"
#include "ParseForestO2.h"
#include "SpanItem.h"
#define DEBUG false


void DecoderO2::ResetBC(vector<double> &b, vector<double> &c){
	for(int k = 0; k < b.size(); k++){
		b.at(k) = MINVALUE;
		c.at(k) = MINVALUE;
	}
}

void DecoderO2::DecodeProjective(Instance &instance,vector<FeatureVector> &fvs,  vector<double> &probs,
							vector<FeatureVector> &fvs_trip,vector<double> &probs_trip,
                            vector<FeatureVector> &fvs_sib,vector<double> &probs_sib,
                            vector<FeatureVector> &fvs_gc,vector<double> &probs_gc,
							vector<int> &trip_ids,vector<int> &sib_ids,vector<int> &gc_ids,
                            vector<FeatureVector> &nt_fvs,vector<double> &nt_probs,
                            int K, vector<ResultItem > &d, Pipe &pipe){
	int instance_length = instance.word_items.size();
    //labeled parsing
    vector<int> static_labels;
    vector<double> static_scores;
    bool labeled_parsing = pipe.poptions->labeled_parsing;
    if(labeled_parsing)
        GetDepLabels(nt_probs, instance_length, pipe, static_labels, static_scores);



	//cout << "ILen=" << instance_length << endl;
	ParseForestO2 pf(0, instance_length-1, K);
	FeatureVector tmpfv;
	for(int s = 0; s < instance_length; s++) {
	    pf.Add(s,-1,0,0.0);
	    pf.Add(s,-1,1,0.0);
	}

	vector<SpanItem*> b1;
	b1.reserve(K);
	vector<double> b1p;
	b1p.reserve(K);
	vector<SpanItem*> c1;
	c1.reserve(K);
	vector<double> c1p;
    c1p.reserve(K);
    vector<SpanItem*> a1;
	a1.reserve(K);
	vector<pair<int, int> > pairs;
	pairs.reserve(K);
	for(int k = 0; k < K; k++){
		double val = MINVALUE;
		b1.push_back(NULL);
		b1p.push_back(val);
		c1.push_back(NULL);
		c1p.push_back(val);
		a1.push_back(NULL);
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
		int vect_idx = 2*((s*instance_length - (s+1)*s/2) + j-1);
		
		//cout << s <<" " << t << " "<< 0 <<" idx:" <<vect_idx <<endl;
		//FeatureVector *prodFV_st = &fvs.at(vect_idx + 0);
		//FeatureVector *prodFV_ts = &fvs.at(vect_idx + 1);
		double prodProb_st = probs.at(vect_idx + 0);
		double prodProb_ts = probs.at(vect_idx + 1);
		//cout << "j"<< j << " "<< s <<" " << t << " "<<  " st:" << prodProb_st << " ts:"<< prodProb_ts <<endl;
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
			//cout << "r=" << r << endl;		
		    /** first is direction, second is complete=0*/
		    /** _s means s is the parent*/
		    if(r != t) {
			{//->
			//sib
			ResetBC(b1p, c1p);
			if(DEBUG)
			cout << s <<" "<< r << " " << t << " " << endl; 			
			int starts = s == r ?s:s+1; 
			for(int r1 = starts; r1 <= r; r1++){
				int idx3_1 = pipe.GetFVTripIdx(instance_length,  s, r1, t);
				int idx2_1 = pipe.GetFVSIBIdx(instance_length,   r1, t, 0);
				int idx3 = trip_ids[idx3_1];
				int idx2 = sib_ids[idx2_1];
				double prob_trip = probs_trip.at(idx3);
				double prob_sib = r1 == s? probs_sib.at(idx2):probs_sib.at(idx2+1);
				//vector<SpanItem*> a1;
				//a1.reserve(K);
				pf.GetItemsC(s,r,r1,0, a1);
				double probN = prob_trip + prob_sib + a1.at(0)->prob;
				if(DEBUG)
				cout << "-sib->" << r1 << " " << a1.size() << " score0=" << probN << "=" <<prob_trip<<" + " <<prob_sib << endl;
				for(int k = 0; k < a1.size();k++){
					if(a1.at(k)->prob == MINVALUE)
						break;
					if(DEBUG)
					cout << "k:" << k << " " << a1.at(k)->prob << endl;
					double prob = prob_trip + prob_sib + a1.at(k)->prob;
					if(!pf.AddToKItems(a1.at(k), prob, b1, b1p))
						break;
				}
				if(DEBUG)
				cout << "Esib->" << r1 << " " << b1p.size() << " score0=" << b1p[0] << endl;
			}
			//gc
			int endt = r+1 == t ? t: t-1;
			for(int r1 = r+1; r1 <= endt; r1++){
				int gc = r1;
				if(r1 == t){
					gc = -1;
				}
                int idx3_1 = pipe.GetFVGCIdx(instance_length,  s, t, gc);
				int idx3 = gc_ids[idx3_1];
                double prob_gc = probs_gc.at(idx3);
                //vector<SpanItem*> a1;
				//a1.reserve(K);
                pf.GetItemsC(r+1,t,r1,1, a1);
				if(DEBUG)
				cout << "gc->" << r1 << " " << prob_gc << " score0=" << a1[0]->prob << endl;
                for(int k = 0; k < a1.size();k++){
					if(a1.at(k)->prob == MINVALUE)
						break;
					if(DEBUG)
					cout << "k:" << k << " " << a1.at(k)->prob << endl;
                    double prob =  a1.at(k)->prob+prob_gc;
                    if(!pf.AddToKItems(a1.at(k), prob, c1, c1p))
                        break;
                }
            }
			


	//		cout << s <<" "<< r << " " << t << " "<< b1.size() << "=" << c1.size() << endl; 			
			if(b1.size() != 0 && c1.size() != 0) {
				//vector<pair<int, int> > pairs;
				ResetPairs(pairs);
			    pf.GetKBestPairs2(b1,c1, b1p, c1p, pairs);
			    for(int k = 0; k < pairs.size(); k++) {
	//			cout << s <<" "<< r << " " << t << " "<< b1.size() << "a=K=" << c1.size() << endl; 			
								
				int comp1 = pairs.at(k).first; int comp2 = pairs.at(k).second;
				if(comp1 <0 || comp2 < 0)
					break;				
				double bc = b1p[comp1]+c1p[comp2];
				// ->	
				double prob_fin = bc+prodProb_st;
				//cout << comp1 << " vs " << comp2  << endl;
				if(DEBUG)
				cout << "ALL:" << prob_fin << "BC:" << bc << "="<< b1p[comp1]<<"+" <<c1p[comp2] <<" st:" << prodProb_st << endl;			
				//FVLink fv_fin;
				int r1 = b1[comp1]->r;
				int idx3_1 = pipe.GetFVTripIdx(instance_length,  s, r1, t);
                int idx2_1 = pipe.GetFVSIBIdx(instance_length,   r1, t, 0);
				int idx3 = trip_ids[idx3_1];
                int idx2 = sib_ids[idx2_1];

				//FeatureVector *fv_trip = &fvs_trip.at(idx3);
                //FeatureVector *fv_sib = r1 == s? &fvs_sib.at(idx2):&fvs_sib.at(idx2+1);
				int gc = c1[comp2]->r;
                if(gc == t)
                    gc = -1;
                int idx4_1 = pipe.GetFVGCIdx(instance_length,   s, t, gc);
				int idx4 = gc_ids[idx4_1];
				//FeatureVector *fv_gc = &fvs_gc.at(idx4);
				//fv_fin.Add(prodFV_st);
				//fv_fin.Add(fv_trip);
				//fv_fin.Add(fv_sib);
				//fv_fin.Add(fv_gc);
				
				if(labeled_parsing) {
                    prob_fin += scorest;
                }
				if(!pf.AddO(s,r,t,labelst,0,1,prob_fin,fv_fin,b1[comp1],c1[comp2]))
					break;
			}
			}// ->
			
			{
			ResetBC(b1p,c1p);
			//sib
			int endt = r+1 == t?t:t-1;
			for(int r1 = r+1; r1 <= endt; r1++){
				int idx3_1 = pipe.GetFVTripIdx(instance_length,  t, r1, s);
				int idx2_1 = pipe.GetFVSIBIdx(instance_length,   r1, s, 0);
				int idx3 = trip_ids[idx3_1];
                int idx2 = sib_ids[idx2_1];

				double prob_trip = probs_trip.at(idx3);
				double prob_sib = r1 == t? probs_sib.at(idx2):probs_sib.at(idx2+1);
				//vector<SpanItem*> a1;
				//a1.reserve(K);
				pf.GetItemsC(r+1,t,r1,1, a1);
				double probN = prob_trip + prob_sib + a1.at(0)->prob;
				if(DEBUG)
				cout << "-sib<-" << r1 << " " << a1.size() << " score0=" << probN << "=" <<prob_trip<<" + " <<prob_sib << endl;
				for(int k = 0; k < a1.size();k++){
					if(a1.at(k)->prob == MINVALUE)
						break;
					double prob = prob_trip + prob_sib + a1.at(k)->prob;
					if(!pf.AddToKItems(a1.at(k), prob, b1, b1p))
						break;
				}
				if(DEBUG)
				cout << "Esib<-" << r1 << " " << b1p.size() << " score0=" << b1p[0] << endl;
			}
			//gc
			int starts = s == r?s:s+1;
			for(int r1 = starts; r1 <= r; r1++){
				int gc = r1;
                if(r1 == s){
                    gc = -1;
				}	
                int idx3_1 = pipe.GetFVGCIdx(instance_length,  t, s, gc);
				int idx3 = gc_ids[idx3_1];
                double prob_gc = probs_gc.at(idx3);
                //vector<SpanItem*> a1;
				//a1.reserve(K);
                pf.GetItemsC(s,r,r1,0, a1);
                for(int k = 0; k < a1.size();k++){
					if(a1.at(k)->prob == MINVALUE)
                        break;
                    double prob =  a1.at(k)->prob+prob_gc;
                    if(!pf.AddToKItems(a1.at(k), prob, c1, c1p))
                        break;
                }
            }
			


	//		cout << s <<" "<< r << " " << t << " "<< b1.size() << "=" << c1.size() << endl; 			
			if(b1.size() != 0 && c1.size() != 0) {
				ResetPairs(pairs);
			    pf.GetKBestPairs2(b1,c1, b1p, c1p, pairs);
			    for(int k = 0; k < pairs.size(); k++) {
	//			cout << s <<" "<< r << " " << t << " "<< b1.size() << "a=K=" << c1.size() << endl; 			
								
				int comp1 = pairs.at(k).first; int comp2 = pairs.at(k).second;
				if(comp1 <0 || comp2 < 0)
					break;				
								
				double bc = b1p[comp1]+c1p[comp2];
				// ->				
				double prob_fin = bc+prodProb_ts;
				if(DEBUG)
				cout << "ALL:" << prob_fin << "BC:" << bc << "="<< b1p[comp1]<<"+" <<c1p[comp2] <<" ts:" << prodProb_ts << endl;			
				//FVLink fv_fin;
				//fv_fin.Add(prodFV_ts);

				int r1 = b1[comp1]->r;
				int idx3_1 = pipe.GetFVTripIdx(instance_length,  t, r1, s);
                int idx2_1 = pipe.GetFVSIBIdx(instance_length,   r1, s, 0);
				int idx3 = trip_ids[idx3_1];
                int idx2 = sib_ids[idx2_1];

				int gc = c1[comp2]->r;
				if(gc == s)
					gc = -1;
                int idx4_1 = pipe.GetFVGCIdx(instance_length,   t, s, gc);
				int idx4 = gc_ids[idx4_1];
				//FeatureVector *fv_trip = &fvs_trip.at(idx3);
                //FeatureVector *fv_sib = r1 == t? &fvs_sib.at(idx2):&fvs_sib.at(idx2+1);
				//FeatureVector *fv_gc = &fvs_gc.at(idx4);

                //fv_fin.Add(fv_trip);
                //fv_fin.Add(fv_sib);
                //fv_fin.Add(fv_gc);
				if(labeled_parsing) {
                    prob_fin += scorets;
                }
                if(!pf.AddO(s,r,t,labelts,1,1,prob_fin,fv_fin,c1[comp2],b1[comp1]))//!!!! b1 <-> c1
					break;
				}//for K
			}
			}//end <--
			}						
		    }					
		}
				
		//for complete
			
		for(int r = s; r <= t; r++) {
					
		    if(r != s) {
			ResetBC(b1p, c1p);

            pf.GetItemsO(s,r,0, b1);
			for(int k = 0; k < b1.size();k++){
				if(b1[k]->prob == MINVALUE)
					break;
				b1p.at(k)= b1[k]->prob;
			}
			//gc
			int starti = r ==t?r:r+1;
            for(int r1 = starti; r1 <= t; r1++){
				int gc = r1;
                if(r1 == r){
                    gc = -2;
				}
                int idx3_1 = pipe.GetFVGCIdx(instance_length,  s, r, gc);
				int idx3 = gc_ids[idx3_1];
                double prob_gc = probs_gc.at(idx3);
                pf.GetItemsC(r,t,r1,0, a1);
				//cout << "comp gc->" << r1 << " " << a1.size() << " score0=" << a1[0]->prob << endl;
                for(int k = 0; k < a1.size();k++){
					if(a1.at(k)->prob == MINVALUE)
						break;
                    double prob =  a1.at(k)->prob+prob_gc;
                    if(!pf.AddToKItems(a1.at(k), prob, c1, c1p))
                        break;
                }
            }
			

			if(b1.size() != 0 && c1.size() != 0) {
                //vector<pair<int, int> > pairs;
				ResetPairs(pairs);
                pf.GetKBestPairs2(b1,c1, b1p, c1p, pairs);
                for(int k = 0; k < pairs.size(); k++) {

                int comp1 = pairs.at(k).first; int comp2 = pairs.at(k).second;
				if(comp1 <0 || comp2 < 0)
					break;				

                double bc = b1p[comp1]+c1p[comp2];
 
									
				//FVLink fv;
				int gc = c1[comp2]->r;					
				if(gc == r)
                    gc = -2;
                int idx4_1 = pipe.GetFVGCIdx(instance_length,   s, r, gc);
				int idx4 = gc_ids[idx4_1];
                //FeatureVector *fv_gc = &fvs_gc.at(idx4);
				//fv.Add(fv_gc);

				if(!pf.AddC(s,r,t,-1,0,0,bc,
					   fv_fin,
					   b1[comp1],c1[comp2]))
				    break;
			    }
			}
		    }
						
		    if(r != t) {
			ResetBC(b1p, c1p);
            pf.GetItemsO(r,t,1, b1);
            for(int k = 0; k < b1.size();k++)
                b1p.at(k) = (b1[k]->prob);
            //gc
			int endi = s == r? r: r-1;
            for(int r1 = s; r1 <= endi; r1++){
				int gc = r1;
                if(r1 == r){
                    gc = -2;
				}
                int idx3_1 = pipe.GetFVGCIdx(instance_length,  t, r, gc);
				int idx3 = gc_ids[idx3_1];
                double prob_gc = probs_gc.at(idx3);
                pf.GetItemsC(s,r,r1,1, a1);
				//cout << "comp gc<-" << r1 << " " << a1.size() << " score0=" << a1[0]->prob << endl;
                for(int k = 0; k < a1.size();k++){
					if(a1.at(k)->prob == MINVALUE)
						break;
                    double prob =  a1.at(k)->prob+prob_gc;
                    if(!pf.AddToKItems(a1.at(k), prob, c1, c1p))
                        break;
                }
            }

			if(b1.size() != 0 && c1.size() != 0) {
                //vector<pair<int, int> > pairs;
				ResetPairs(pairs);
                pf.GetKBestPairs2(b1,c1, b1p, c1p, pairs);
                for(int k = 0; k < pairs.size(); k++) {

                int comp1 = pairs.at(k).first; int comp2 = pairs.at(k).second;
				if(comp1 <0 || comp2 < 0)
					break;				

                double bc = b1p[comp1]+c1p[comp2];


                //FVLink fv;
				int gc = c1[comp2]->r;
                if(gc == r)
                    gc = -2;
                int idx4_1 = pipe.GetFVGCIdx(instance_length,   t, r, gc);
				int idx4 = gc_ids[idx4_1];
                //FeatureVector *fv_gc = &fvs_gc.at(idx4);
                //fv.Add(fv_gc);					
				if(!pf.AddC(s,r,t,-1,1,0,bc,
					   fv_fin,c1[comp2],b1[comp1]))//!!!!b1 <-> c1
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
void DecoderO2::DecodeBeam(Instance &instance,vector<FeatureVector> &fvs,  vector<double> &probs,
                            vector<FeatureVector> &fvs_trip,vector<double> &probs_trip,
                            vector<FeatureVector> &fvs_sib,vector<double> &probs_sib,
                            vector<FeatureVector> &fvs_gc,vector<double> &probs_gc,
                            vector<int> &trip_ids,vector<int> &sib_ids,vector<int> &gc_ids,
                            vector<FeatureVector> &nt_fvs,vector<double> &nt_probs,
                            int K, vector<ResultItem > &d, Pipe &pipe, Parameters &params){
	int instance_length = instance.word_items.size();
	    //labeled parsing
    vector<int> static_labels;
    vector<double> static_scores;
    bool labeled_parsing = pipe.poptions->labeled_parsing;
    if(labeled_parsing)
        GetDepLabels(nt_probs, instance_length, pipe, static_labels, static_scores);


	//cout << "ILen=" << instance_length << endl;
	ParseForestO2 pf(0, instance_length-1, K);
	FeatureVector tmpfv;
	for(int s = 0; s < instance_length; s++) {
	    pf.Add(s,-1,0,0.0);
	    pf.Add(s,-1,1,0.0);
	}

	vector<SpanItem*> b1;
	b1.reserve(K);
	vector<double> b1p;
	b1p.reserve(K);
	vector<SpanItem*> c1;
	c1.reserve(K);
	vector<double> c1p;
    c1p.reserve(K);
    vector<SpanItem*> a1;
	a1.reserve(K);
	vector<pair<int, int> > pairs;
	pairs.reserve(K);
	for(int k = 0; k < K; k++){
		double val = MINVALUE;
		b1.push_back(NULL);
		b1p.push_back(val);
		c1.push_back(NULL);
		c1p.push_back(val);
		a1.push_back(NULL);
		pair<int, int> p;
		p.first = -1;
		p.second = -1;
		pairs.push_back(p);
	}	
	FVLink fv_fin;
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
        *(probs_gsib+i) = MINVALUE;
    for(int i = 0; i < size_3sib; i++)
        *(probs_3sib+i) = MINVALUE;
	FeatureVector fv;

	//cout << "Endof" << endl;
	for(int j = 1; j < instance_length; ++j) {
		
	  	for(int s = 0; s < instance_length && s+j < instance_length; s++) {
		
		int t = s+j;
		int vect_idx = 2*((s*instance_length - (s+1)*s/2) + j-1);
		
		//cout << s <<" " << t << " "<< 0 <<" idx:" <<vect_idx <<endl;
		//FeatureVector *prodFV_st = &fvs.at(vect_idx + 0);
		//FeatureVector *prodFV_ts = &fvs.at(vect_idx + 1);
		double prodProb_st = probs.at(vect_idx + 0);
		double prodProb_ts = probs.at(vect_idx + 1);
		//cout << "j"<< j << " "<< s <<" " << t << " "<<  " st:" << prodProb_st << " ts:"<< prodProb_ts <<endl;
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
			//cout << "r=" << r << endl;		
		    /** first is direction, second is complete=0*/
		    /** _s means s is the parent*/
		    if(r != t) {
			{//->
			//sib
			ResetBC(b1p, c1p);
			if(DEBUG)
			cout << s <<" "<< r << " " << t << " " << endl; 			
			int starts = s == r ?s:s+1; 
			for(int r1 = starts; r1 <= r; r1++){
				int idx3_1 = pipe.GetFVTripIdx(instance_length,  s, r1, t);
				int idx2_1 = pipe.GetFVSIBIdx(instance_length,   r1, t, 0);
				int idx3 = trip_ids[idx3_1];
				int idx2 = sib_ids[idx2_1];
				double prob_trip = probs_trip.at(idx3);
				double prob_sib = r1 == s? probs_sib.at(idx2):probs_sib.at(idx2+1);
				//vector<SpanItem*> a1;
				//a1.reserve(K);
				pf.GetItemsC(s,r,r1,0, a1);
				double probN = prob_trip + prob_sib + a1.at(0)->prob;
				if(DEBUG)
				cout << "-sib->" << r1 << " " << a1.size() << " score0=" << probN << "=" <<prob_trip<<" + " <<prob_sib << endl;
				for(int k = 0; k < a1.size();k++){
					if(a1.at(k)->prob == MINVALUE)
						break;
					if(DEBUG)
					cout << "k:" << k << " " << a1.at(k)->prob << endl;
					double prob = prob_trip + prob_sib + a1.at(k)->prob;
					if(!pf.AddToKItems(a1.at(k), prob, b1, b1p))
						break;
				}
				if(DEBUG)
				cout << "Esib->" << r1 << " " << b1p.size() << " score0=" << b1p[0] << endl;
			}
			//gc
			int endt = r+1 == t ? t: t-1;
			for(int r1 = r+1; r1 <= endt; r1++){
				int gc = r1;
				if(r1 == t){
					gc = -1;
				}
                int idx3_1 = pipe.GetFVGCIdx(instance_length,  s, t, gc);
				int idx3 = gc_ids[idx3_1];
                double prob_gc = probs_gc.at(idx3);
                //vector<SpanItem*> a1;
				//a1.reserve(K);
                pf.GetItemsC(r+1,t,r1,1, a1);
				if(DEBUG)
				cout << "gc->" << r1 << " " << prob_gc << " score0=" << a1[0]->prob << endl;
                for(int k = 0; k < a1.size();k++){
					if(a1.at(k)->prob == MINVALUE)
						break;
					if(DEBUG)
					cout << "k:" << k << " " << a1.at(k)->prob << endl;
                    double prob =  a1.at(k)->prob+prob_gc;
                    if(!pf.AddToKItems(a1.at(k), prob, c1, c1p))
                        break;
                }
            }
			


	//		cout << s <<" "<< r << " " << t << " "<< b1.size() << "=" << c1.size() << endl; 			
			if(b1.size() != 0 && c1.size() != 0) {
				//vector<pair<int, int> > pairs;
				ResetPairs(pairs);
			    pf.GetKBestPairs2(b1,c1, b1p, c1p, pairs);
			    for(int k = 0; k < pairs.size(); k++) {
	//			cout << s <<" "<< r << " " << t << " "<< b1.size() << "a=K=" << c1.size() << endl; 			
								
				int comp1 = pairs.at(k).first; int comp2 = pairs.at(k).second;
				if(comp1 <0 || comp2 < 0)
					break;				
				double bc = b1p[comp1]+c1p[comp2];
				// ->	
				double prob_fin = bc+prodProb_st;
				//cout << comp1 << " vs " << comp2  << endl;
				if(DEBUG)
				cout << "ALL:" << prob_fin << "BC:" << bc << "="<< b1p[comp1]<<"+" <<c1p[comp2] <<" st:" << prodProb_st << endl;			
				
				//Add Higher-order features dynamically
				

				//FVLink fv_fin;
				int r1 = b1[comp1]->r;
				int idx3_1 = pipe.GetFVTripIdx(instance_length,  s, r1, t);
                int idx2_1 = pipe.GetFVSIBIdx(instance_length,   r1, t, 0);
				int idx3 = trip_ids[idx3_1];
                int idx2 = sib_ids[idx2_1];

				//FeatureVector *fv_trip = &fvs_trip.at(idx3);
                //FeatureVector *fv_sib = r1 == s? &fvs_sib.at(idx2):&fvs_sib.at(idx2+1);
				int gc = c1[comp2]->r;
                if(gc == t)
                    gc = -1;
                int idx4_1 = pipe.GetFVGCIdx(instance_length,   s, t, gc);
				int idx4 = gc_ids[idx4_1];
				//FeatureVector *fv_gc = &fvs_gc.at(idx4);
				//fv_fin.Add(prodFV_st);
				//fv_fin.Add(fv_trip);
				//fv_fin.Add(fv_sib);
				//fv_fin.Add(fv_gc);
				
				//if(pipe.labeled) {
				//    fv_fin = nt_fv_s_01.cat(nt_fv_t_00.cat(fv_fin));
				//    prob_fin += nt_prob_s_01+nt_prob_t_00;
				//}
				int b1r = b1[comp1]->r;
				int c1r = c1[comp2]->r;
			    {//higher-order features
                int gc = -1;
                int gc1 = -1;
                if(c1r != t){
                gc = c1r;
                SpanItem *p =  c1[comp2]->pright->pright;
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
                SpanItem *p =  b1[comp1]->pleft->pleft;
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
				 if(labeled_parsing) {
                    prob_fin += scorest;
                }
				if(!pf.AddO(s,r,t,labelst,0,1,prob_fin,fv_fin,b1[comp1],c1[comp2]))
					break;
			}
			}// ->
			
			{
			ResetBC(b1p,c1p);
			//sib
			int endt = r+1 == t?t:t-1;
			for(int r1 = r+1; r1 <= endt; r1++){
				int idx3_1 = pipe.GetFVTripIdx(instance_length,  t, r1, s);
				int idx2_1 = pipe.GetFVSIBIdx(instance_length,   r1, s, 0);
				int idx3 = trip_ids[idx3_1];
                int idx2 = sib_ids[idx2_1];

				double prob_trip = probs_trip.at(idx3);
				double prob_sib = r1 == t? probs_sib.at(idx2):probs_sib.at(idx2+1);
				//vector<SpanItem*> a1;
				//a1.reserve(K);
				pf.GetItemsC(r+1,t,r1,1, a1);
				double probN = prob_trip + prob_sib + a1.at(0)->prob;
				if(DEBUG)
				cout << "-sib<-" << r1 << " " << a1.size() << " score0=" << probN << "=" <<prob_trip<<" + " <<prob_sib << endl;
				for(int k = 0; k < a1.size();k++){
					if(a1.at(k)->prob == MINVALUE)
						break;
					double prob = prob_trip + prob_sib + a1.at(k)->prob;
					if(!pf.AddToKItems(a1.at(k), prob, b1, b1p))
						break;
				}
				if(DEBUG)
				cout << "Esib<-" << r1 << " " << b1p.size() << " score0=" << b1p[0] << endl;
			}
			//gc
			int starts = s == r?s:s+1;
			for(int r1 = starts; r1 <= r; r1++){
				int gc = r1;
                if(r1 == s){
                    gc = -1;
				}	
                int idx3_1 = pipe.GetFVGCIdx(instance_length,  t, s, gc);
				int idx3 = gc_ids[idx3_1];
                double prob_gc = probs_gc.at(idx3);
                //vector<SpanItem*> a1;
				//a1.reserve(K);
                pf.GetItemsC(s,r,r1,0, a1);
                for(int k = 0; k < a1.size();k++){
					if(a1.at(k)->prob == MINVALUE)
                        break;
                    double prob =  a1.at(k)->prob+prob_gc;
                    if(!pf.AddToKItems(a1.at(k), prob, c1, c1p))
                        break;
                }
            }
			


	//		cout << s <<" "<< r << " " << t << " "<< b1.size() << "=" << c1.size() << endl; 			
			if(b1.size() != 0 && c1.size() != 0) {
				ResetPairs(pairs);
			    pf.GetKBestPairs2(b1,c1, b1p, c1p, pairs);
			    for(int k = 0; k < pairs.size(); k++) {
	//			cout << s <<" "<< r << " " << t << " "<< b1.size() << "a=K=" << c1.size() << endl; 			
								
				int comp1 = pairs.at(k).first; int comp2 = pairs.at(k).second;
				if(comp1 <0 || comp2 < 0)
					break;				
								
				double bc = b1p[comp1]+c1p[comp2];
				// ->				
				double prob_fin = bc+prodProb_ts;
				if(DEBUG)
				cout << "ALL:" << prob_fin << "BC:" << bc << "="<< b1p[comp1]<<"+" <<c1p[comp2] <<" ts:" << prodProb_ts << endl;			
				//FVLink fv_fin;
				//fv_fin.Add(prodFV_ts);

				int r1 = b1[comp1]->r;
				int idx3_1 = pipe.GetFVTripIdx(instance_length,  t, r1, s);
                int idx2_1 = pipe.GetFVSIBIdx(instance_length,   r1, s, 0);
				int idx3 = trip_ids[idx3_1];
                int idx2 = sib_ids[idx2_1];

				int gc = c1[comp2]->r;
				if(gc == s)
					gc = -1;
                int idx4_1 = pipe.GetFVGCIdx(instance_length,   t, s, gc);
				int idx4 = gc_ids[idx4_1];
				//FeatureVector *fv_trip = &fvs_trip.at(idx3);
                //FeatureVector *fv_sib = r1 == t? &fvs_sib.at(idx2):&fvs_sib.at(idx2+1);
				//FeatureVector *fv_gc = &fvs_gc.at(idx4);

                //fv_fin.Add(fv_trip);
                //fv_fin.Add(fv_sib);
                //fv_fin.Add(fv_gc);
				//if(pipe.labeled) {
                //    fv_fin = nt_fv_t_11.cat(nt_fv_s_10.cat(fv_fin));
                //    prob_fin += nt_prob_t_11+nt_prob_s_10;
                //}
				int b1r = b1[comp1]->r;
				int c1r = c1[comp2]->r;//!! b1 <-> c1
                {//higher-order features

                int gc = -1;
                int gc1 = -1;
                if(c1r != s){
                gc = c1r;
                SpanItem *p =  c1[comp2]->pleft->pleft;
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
                int ch2 = b1r;
                int ch1 = t;
                if(ch2 != t){
                SpanItem *p =  b1[comp1]->pright->pright;
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
                prob_fin += prob_gsib+prob_3sib;
                }

				if(labeled_parsing) {
                    prob_fin += scorets;
                }
                if(!pf.AddO(s,r,t,labelts,1,1,prob_fin,fv_fin,c1[comp2],b1[comp1]))//!!!! b1 <-> c1
					break;
				}//for K
			}
			}//end <--
			}						
		    }					
		}
				
		//for complete
			
		for(int r = s; r <= t; r++) {
					
		    if(r != s) {
			ResetBC(b1p, c1p);

            pf.GetItemsO(s,r,0, b1);
			for(int k = 0; k < b1.size();k++){
				if(b1[k]->prob == MINVALUE)
					break;
				b1p.at(k)= b1[k]->prob;
			}
			//gc
			int starti = r ==t?r:r+1;
            for(int r1 = starti; r1 <= t; r1++){
				int gc = r1;
                if(r1 == r){
                    gc = -2;
				}
                int idx3_1 = pipe.GetFVGCIdx(instance_length,  s, r, gc);
				int idx3 = gc_ids[idx3_1];
                double prob_gc = probs_gc.at(idx3);
                pf.GetItemsC(r,t,r1,0, a1);
				//cout << "comp gc->" << r1 << " " << a1.size() << " score0=" << a1[0]->prob << endl;
                for(int k = 0; k < a1.size();k++){
					if(a1.at(k)->prob == MINVALUE)
						break;
                    double prob =  a1.at(k)->prob+prob_gc;
                    if(!pf.AddToKItems(a1.at(k), prob, c1, c1p))
                        break;
                }
            }
			

			if(b1.size() != 0 && c1.size() != 0) {
                //vector<pair<int, int> > pairs;
				ResetPairs(pairs);
                pf.GetKBestPairs2(b1,c1, b1p, c1p, pairs);
                for(int k = 0; k < pairs.size(); k++) {

                int comp1 = pairs.at(k).first; int comp2 = pairs.at(k).second;
				if(comp1 <0 || comp2 < 0)
					break;				

                double bc = b1p[comp1]+c1p[comp2];
 
									
				//FVLink fv;
				int gc = c1[comp2]->r;					
				if(gc == r)
                    gc = -2;
                int idx4_1 = pipe.GetFVGCIdx(instance_length,   s, r, gc);
				int idx4 = gc_ids[idx4_1];
                //FeatureVector *fv_gc = &fvs_gc.at(idx4);
				//fv.Add(fv_gc);
				{//higher-order features
                int gc = -2;
                int gc1 = -2;
                if(c1[comp2]->r != r){
                gc = c1[comp2]->r;
                SpanItem *p =  c1[comp2]->pleft->pleft;
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

				if(!pf.AddC(s,r,t,-1,0,0,bc,
					   fv_fin,
					   b1[comp1],c1[comp2]))
				    break;
			    }
			}
		    }
						
		    if(r != t) {
			ResetBC(b1p, c1p);
            pf.GetItemsO(r,t,1, b1);
            for(int k = 0; k < b1.size();k++)
                b1p.at(k) = (b1[k]->prob);
            //gc
			int endi = s == r? r: r-1;
            for(int r1 = s; r1 <= endi; r1++){
				int gc = r1;
                if(r1 == r){
                    gc = -2;
				}
                int idx3_1 = pipe.GetFVGCIdx(instance_length,  t, r, gc);
				int idx3 = gc_ids[idx3_1];
                double prob_gc = probs_gc.at(idx3);
                pf.GetItemsC(s,r,r1,1, a1);
				//cout << "comp gc<-" << r1 << " " << a1.size() << " score0=" << a1[0]->prob << endl;
                for(int k = 0; k < a1.size();k++){
					if(a1.at(k)->prob == MINVALUE)
						break;
                    double prob =  a1.at(k)->prob+prob_gc;
                    if(!pf.AddToKItems(a1.at(k), prob, c1, c1p))
                        break;
                }
            }

			if(b1.size() != 0 && c1.size() != 0) {
                //vector<pair<int, int> > pairs;
				ResetPairs(pairs);
                pf.GetKBestPairs2(b1,c1, b1p, c1p, pairs);
                for(int k = 0; k < pairs.size(); k++) {

                int comp1 = pairs.at(k).first; int comp2 = pairs.at(k).second;
				if(comp1 <0 || comp2 < 0)
					break;				

                double bc = b1p[comp1]+c1p[comp2];


                //FVLink fv;
				int gc = c1[comp2]->r;
                if(gc == r)
                    gc = -2;
                int idx4_1 = pipe.GetFVGCIdx(instance_length,   t, r, gc);
				int idx4 = gc_ids[idx4_1];
                //FeatureVector *fv_gc = &fvs_gc.at(idx4);
                //fv.Add(fv_gc);					

                {//higher-order features
                int gc = -2;
                int gc1 = -2;
                if(c1[comp2]->r != r){
                gc = c1[comp2]->r;
                SpanItem *p =  c1[comp2]->pright->pright;
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


				if(!pf.AddC(s,r,t,-1,1,0,bc,
					   fv_fin,c1[comp2],b1[comp1]))//!!!!b1 <-> c1
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
