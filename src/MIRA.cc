/*
*/
#include <float.h>
#include <cmath>
#include "MIRA.h"
void MIRA::UpdateParamsMIRA(FeatureVector &act_fv,
                                 vector<ResultItem > &d, double upd,
                                 string gold_string, Parameters &params){
	vector<double> num_errs;
	for(int k = 0; k < d.size(); k++){
		double err = params.GetNumErrors(d[k].parse_tree, gold_string);
		num_errs.push_back(err);
	}
	UpdateParamsMIRA(act_fv, d, upd, num_errs, params);
}

void MIRA::UpdateParamsMIRA(FeatureVector &act_fv,
                                 vector<ResultItem > &d, double upd, 
                                 vector<double> &num_errs, Parameters &params){

	//if(true) return;
	int K = 0;
	for(int i = 0; i < d.size(); ++i) {
	    K = i+1;
	}
	vector<double> b;
	vector<FeatureVector> dist;

	for(int k = 0; k < K; k++) {
		FeatureVector fv;
		fv.CopyFrom(act_fv);
		double lamda = fv.GetScore(params) - d[k].fv.GetScore(params); 
/*		for(int jj = 0; jj < d[k].first.feats.size(); ++jj){
			cout << d[k].first.feats[jj].first << "=" << d[k].first.feats[jj].second << "   ";
		}
		cout << endl;
*/		//cout <<"Scores="<< fv.GetScore(params)<< " - " << d[k].first.GetScore(params) << endl;
		
		//double err = params.GetNumErrors(instance, d[k].second, act_parsetree);
		double err = num_errs[k];
		b.push_back(err - lamda);
	
		fv.GetDistVector(d[k].fv);
		//cout << "End of GetDisk" << endl;
		dist.push_back(fv);
	}
	vector<double> alpha;
//	alpha.push_back(0.1);
//	cout << "B0:" << b[0] << endl;	
	Hildreth(dist,b, alpha);
	//cout << "Alpha:" << alpha[0] << endl;	
	FeatureVector fv;
	int res = 0;
	for(int k = 0; k < K; k++) {
	    fv.CopyFrom(dist[k]);

	    Update(params, alpha[k], upd, fv);
	    
	}

}

void MIRA::Hildreth(vector<FeatureVector> & dist, vector<double> &b, vector<double> & alpha){
	int i;
	int max_iter = 10000;
	double eps = 0.00000001;
	double zero = 0.000000000001;
	
	int K = b.size();
		
	//double[] alpha = new double[b.length];

	//double[] F = new double[b.length];
	//double[] kkt = new double[b.length];
	vector<double> kkt;
	vector<double> F;
	double max_kkt = -DBL_MAX;

	vector<vector<double> > A;	
	vector<bool> is_computed;
	for(i = 0; i < K; i++) {
		vector<double> A0;
		for(int j = 0; j < K; j++){
			 A0.push_back(0.0);
		}
		A.push_back(A0);
		
	    A[i][i] = dist[i].DotProduct(dist[i]);
	//	cout << "A00:" << A[i][i] << endl;
	    is_computed.push_back(false);
		alpha.push_back(0.0);
	}
				
	int max_kkt_i = -1;

		
	for(i = 0; i < K; i++) {
		F.push_back(b[i]);
		kkt.push_back(F[i]);
	    if(kkt[i] > max_kkt) { max_kkt = kkt[i]; max_kkt_i = i; }
	}

	int iter = 0;
	double diff_alpha;
	double try_alpha;
	double add_alpha;
	
	while(max_kkt >= eps && iter < max_iter) {
	    diff_alpha = A[max_kkt_i][max_kkt_i] <= 0.0 ? 0.0 : F[max_kkt_i]/A[max_kkt_i][max_kkt_i];
	    try_alpha = alpha[max_kkt_i] + diff_alpha;
	    add_alpha = 0.0;
//		cout << max_kkt << " " << max_kkt_i << " "<< try_alpha << " " << F[max_kkt_i] << " A00=" <<A[max_kkt_i][max_kkt_i] << endl;	

	    if(try_alpha < 0.0)
		add_alpha = -1.0 * alpha[max_kkt_i];
	    else
		add_alpha = diff_alpha;

	    alpha[max_kkt_i] = alpha[max_kkt_i] + add_alpha;
	//	cout << "i:" << iter << " " << alpha[max_kkt_i] << " add=" << add_alpha << endl;
	    if (!is_computed[max_kkt_i]) {
		for(i = 0; i < K; i++) {
		    A[i][max_kkt_i] = dist[i].DotProduct(dist[max_kkt_i]); // for version 1
		    is_computed[max_kkt_i] = true;
		}
	    }

	    for(i = 0; i < K; i++) {
		F[i] -= add_alpha * A[i][max_kkt_i];
		kkt[i] = F[i];
		if(alpha[i] > 0.0)
		    kkt[i] = abs(F[i]);
	    }

	    max_kkt = -DBL_MAX;
	    max_kkt_i = -1;
	    for(i = 0; i < K; i++)
		if(kkt[i] > max_kkt) { max_kkt = kkt[i]; max_kkt_i = i; }

	    iter++;
		
	}

 
}

void MIRA::Update(Parameters &params, double a, double upd, FeatureVector &fv){
	fv.UpdateParams(params, a, upd, 1);
}

void MIRA::Update(Parameters &params, double a, double upd, FeatureVector &fv, int neg_flag){
}

