#include "TagDecoder.h"
#include "TagSpan.h"
#include "SpanItem.h"


void TagDecoder::DecodeProjective(Instance &instance,vector<FeatureVector> &fvs,  vector<double> &probs,
                            int K, vector<ResultItem> &d){
	int instance_length = instance.word_items.size();
	//cout << "ILen=" << instance_length << endl;
	TagSpan pf(0, instance_length-1,types_num, K);
	pf.root_type = this->root_type;
	FeatureVector tmpfv;
	//return;
	//cout << "Endof" << endl;
	for(int t = 0; t < types_num; ++t){
		FVLink fv_fin;
		pf.Add(0, t, 0, fv_fin, NULL);
	}
	vector<SpanItem*> b1;
	b1.reserve(K);
	for(int k = 0; k < K; k++){
        b1.push_back(NULL);
	}
	int first_size = (instance_length-1)*types_num;
	for(int s = 1; s < instance_length;++s){
	for(int t = 0; t < types_num; ++t){
		//skip 0,1 if 1<=s < (instance_length-1)
		if(s < instance_length-1 && (t == 0 || t == 1))
			continue;
		if(s == instance_length-1 && t != 0)//end_tag always = 0
			continue;
		int vect_idx0 = (s-1) * types_num + t;
		//FeatureVector *prodFV0 = &fvs.at(vect_idx0);
		double prob0 = probs.at(vect_idx0);
		for(int t_1 = 0; t_1 < types_num; ++t_1){

			if(s == 1 && t_1 != 1)
				continue;
			if(s > 1 && (t_1 == 0 || t_1 == 1))
				continue;

			int vect_idx = first_size + (s-1) * types_num*types_num + t*types_num +t_1;
			//FeatureVector *prodFV = &fvs.at(vect_idx);
			double prob = probs.at(vect_idx);
	        pf.GetItems(s-1,t_1,K, b1);
			for(int k = 0; k < b1.size(); ++k){
				if(b1[k]->prob == MINVALUE)
                   break;
				double prob_fin = b1[k]->prob +prob+prob0;
	            FVLink fv_fin;
	            //fv_fin.Add(prodFV);
				//fv_fin.Add(prodFV0);
	            pf.Add(s,t,prob_fin,fv_fin,b1[k]);	
				//cout << s << " " << t << " " << t_1 << " " << prob_fin << endl;	
			}
		}
	}}//forfor
	pf.GetBestPaths(d);	
}
