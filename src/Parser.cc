#include "Parser.h"
#include <ctime>
#include "FeatureVector.h"
#include "MIRA.h"

Parser::Parser(Pipe *p1, SymOptions *p2){
	ppipe = p1;
	poptions = p2;
	params.Init(p1->feat_list.GetSize());
}

void Parser::Train(string &trainfile){
	
	int i = 0;
	for(i = 0; i < poptions->num_iters;) {
			
	    cerr << " Iteration " << i ;
	    cerr << "[" << flush;
		time_t start_time;
	    start_time = time(NULL);

	    TrainingIter(trainfile,i+1);
		
		time_t end_time;
	    end_time = time(NULL);

	    cerr << "|Time:" << end_time - start_time << "]" << endl;			
		i++;
		//break;
	}
	//cerr << params.parameters[0] << " " << params.parameters[1] << endl;
	//cerr << "Inst-Num=" << ppipe->instance_num << endl;
	params.AverageParams((double)i*ppipe->instance_num);
	//cerr << params.parameters[0] << " " << params.parameters[1] << endl;

}

void Parser::TrainingIter(string &trainfile, int iter){
    ifstream feat_rd;
    feat_rd.open(ppipe->poptions->featfile.c_str(), ios::binary);
    if(!feat_rd.is_open()){
        cerr << "Err: opening "<< ppipe->poptions->featfile << endl;
        return ;
    }
    time_t start_time, end_time;
    start_time = time(NULL);



	int instance_num = ppipe->instance_num;
	ppipe->InitFile(trainfile);
	MIRA mira;
	for(int i = 0; i < instance_num; i++) {
	    if((i+1) % 500 == 0) {
			cerr << (i+1) << ","<< flush; 
	//		end_time = time(NULL);
			
	//		cerr << (i+1) << "("<< (end_time-start_time)<< ")," << flush;
	//		break;
	    }
		Instance instance;
		ppipe->NextInstance(instance);
		if(instance.word_items.size() == 0)
			continue;
		//cerr << length << ",";
	    // Get production crap.
		vector<FeatureVector> fvs;//[.length][.length][2]
		vector<double> probs;
		vector<FeatureVector> nt_fvs;//[.length][.types.length][2][2]
		vector<double> nt_probs;

		vector<FeatureVector> fvs_trip;//[.length][.length][2]
        vector<double> probs_trip;

		vector<FeatureVector> fvs_sib;//[.length][.length][2]
        vector<double> probs_sib;
		vector<FeatureVector> fvs_gc;//[.length][.length][2]
        vector<double> probs_gc;
		vector<int> trip_ids;
		vector<int> sib_ids;
		vector<int> gc_ids;
		//cerr << fvs.size() << "-" << fvs.capacity()<< " " << length << " ";
		
	//	ppipe->FillFeatureVectors(instance,fvs,probs,nt_fvs,nt_probs,params);
		if(poptions->order == 2){
			ppipe->ReadInstance2(feat_rd, instance,fvs,probs,fvs_trip,probs_trip,
                                   fvs_sib,probs_sib,fvs_gc, probs_gc, trip_ids, sib_ids, gc_ids, nt_fvs,nt_probs,params);
		}
		else if(poptions->beamfeat){
			ppipe->ReadInstance2(feat_rd, instance,fvs,probs,fvs_trip,probs_trip,
                                   fvs_sib,probs_sib,fvs_gc, probs_gc, trip_ids, sib_ids, gc_ids, nt_fvs,nt_probs,params);
		}
		else
			ppipe->ReadInstance(feat_rd, instance,fvs,probs,nt_fvs,nt_probs,params);
		//cerr << fvs.size() <<"-" << fvs.capacity()<< endl;
		
	    double upd = (double)(poptions->num_iters*instance_num - (instance_num*(iter-1)+(i+1)) + 1);
		int K = poptions->trainK;
		vector<ResultItem > d;
		if(poptions->order == 2){
			if(poptions->beamfeat){
            decoder2.DecodeBeam(instance,fvs,probs,fvs_trip, probs_trip,
                                        fvs_sib, probs_sib, fvs_gc, probs_gc,trip_ids, sib_ids, gc_ids, 
                                         nt_fvs,nt_probs,K, d, *ppipe, params);
			}
			else{
			decoder2.DecodeProjective(instance,fvs,probs,fvs_trip, probs_trip, 
										fvs_sib, probs_sib, fvs_gc, probs_gc,trip_ids, sib_ids, gc_ids, 
                                        nt_fvs,nt_probs,K, d, *ppipe);
			}
		}
		else if(poptions->beamfeat){
			decoder.DecodeBeam(instance,fvs,probs,fvs_trip, probs_trip, 
										fvs_sib, probs_sib, fvs_gc, probs_gc,trip_ids, sib_ids, gc_ids, 
										nt_fvs,nt_probs,K, d, *ppipe, params);
        }
		else//order=1
			decoder.DecodeProjective(instance,fvs,probs,nt_fvs,nt_probs,K, d, *ppipe);
		//if(true) continue;
		//break;
		//if(poptions->DEBUG){
	    //
		FeatureVector fv;
		fv.CopyFrom(instance.act_fv);
		vector<double> num_errs;
		for(int k = 0; k < K && k < d.size(); ++k){
			//get fv
			ppipe->CreateTagFeatureVector(instance, d[k].parse_tree, d[k].fv);

			double err = ppipe->GetNumErrors(instance, d[k].parse_tree, instance.act_parsetree);
			num_errs.push_back(err);
		
		}
		if(false)
		{
		
		cerr << "\nGold:" << instance.act_parsetree << endl;
		cerr << "Scores:" << instance.act_fv.GetScore(params)<< endl;
		for(int k = 0; k < K && k < d.size(); ++k){
		cerr <<   "PRED:" << d[k].parse_tree << "<--"<< endl;
		cerr << "Scores:" << d[k].fv.GetScore(params)<< endl;
		cerr << "NewScores:" << d[k].score<< endl;
		cerr <<	 "Errs:" << ppipe->GetNumErrors(instance, d[k].parse_tree, instance.act_parsetree) << endl;
		}
		if(i > 3) break;
		}
		//if(i == 2)
		mira.UpdateParamsMIRA(fv,d,upd, num_errs, params);
		//break;
		//if(true) continue;

	}
	feat_rd.close();
	ppipe->CloseFile();
	cerr << instance_num;
}

void Parser::SaveModel(string &model_name){
	SaveModelNew(model_name);
/*
	ofstream model_wd;
    model_wd.open(model_name.c_str(), ios::binary);
    if(!model_wd.is_open()){
        cerr << "Err: opening "<< model_name << endl;
        return ;
    }
	boost::archive::binary_oarchive oa(model_wd);
	int m = -1;
	oa << params.parameters;
	//cerr << params.parameters.size() << flush << endl;

	vector<string> feats;
	vector<long> feat_vals;
	ppipe->feat_list.ToArray(feats, feat_vals);
	oa << feats;
	oa << feat_vals;
	cerr << "feat_size=" << feats.size() << endl;
	//oa << ppipe->feat_list.mytable;
	vector<string> types;
    vector<long> type_vals;
    ppipe->type_list.ToArray(types, type_vals);
	oa << types;
	oa << type_vals;
	//oa << ppipe->type_list.mytable;
	model_wd.close();
*/

}
void Parser::SaveModelNew(string &model_name){
    ofstream model_wd;
    model_wd.open(model_name.c_str(), ios::binary);
    if(!model_wd.is_open()){
        cerr << "Err: opening "<< model_name << endl;
        return ;
    }
    MyTool::VectorWrite(model_wd, params.parameters);
    vector<string> feats;
    vector<long> feat_vals;
    ppipe->feat_list.ToArray(feats, feat_vals);
    MyTool::VectorWrite(model_wd, feats);
    MyTool::VectorWrite(model_wd, feat_vals);

    vector<string> types;
    vector<long> type_vals;
    ppipe->type_list.ToArray(types, type_vals);
    MyTool::VectorWrite(model_wd, types);
    MyTool::VectorWrite(model_wd, type_vals);
	
	vector<string> pospair_types;
    vector<long> pospair_type_vals;
    ppipe->pospair_type_list.ToArray(pospair_types, pospair_type_vals);
    MyTool::VectorWrite(model_wd, pospair_types);
    MyTool::VectorWrite(model_wd, pospair_type_vals);
    model_wd.close();
}


int Parser::LoadModelNew(string &model_name){
	ifstream model_rd;
    cerr << "INFO: opening "<< model_name << endl;
    model_rd.open(model_name.c_str(), ios::binary);
    if(!model_rd.is_open()){
        cerr << "Err: opening "<< model_name << endl;
        return -1;
    }
    MyTool::VectorRead(model_rd, params.parameters);
    vector<string> feats;
    vector<long> feat_vals;
    MyTool::VectorRead(model_rd,feats);
    MyTool::VectorRead(model_rd,feat_vals);

    vector<string> types;
    vector<long> type_vals;
    MyTool::VectorRead(model_rd,types);
    MyTool::VectorRead(model_rd,type_vals);

    ppipe->feat_list.FromArray(feats, feat_vals);
    ppipe->type_list.FromArray(types,type_vals);

	vector<string> pospair_types;
    vector<long> pospair_type_vals;
	MyTool::VectorRead(model_rd,pospair_types);
    MyTool::VectorRead(model_rd,pospair_type_vals);
	ppipe->pospair_type_list.FromArray(pospair_types, pospair_type_vals);
    model_rd.close();
    ppipe->CloseAlphabets();
}
int Parser::LoadModel(string &model_name){
	return LoadModelNew(model_name);
	/*
	ifstream model_rd;
    model_rd.open(model_name.c_str(), ios::binary);
    if(!model_rd.is_open()){
        cerr << "Err: opening "<< model_name << endl;
        return -1;
    }
    boost::archive::binary_iarchive ia(model_rd);
	int m;
    ia >> params.parameters;
	//cerr << params.parameters.size() << flush << endl;
	vector<string> feats;
    vector<long> feat_vals;
	ia >> feats;
	ia >> feat_vals;
	vector<string> types;
    vector<long> type_vals;
	ia >> types;
	ia >> type_vals;
	ppipe->feat_list.FromArray(feats, feat_vals);
	ppipe->type_list.FromArray(types,type_vals);
    //ia >> ppipe->feat_list.mytable;
    //ia >> ppipe->type_list.mytable;
    model_rd.close();
	ppipe->CloseAlphabets();
	string fn = model_name+".New";
    SaveModelNew(fn);
    LoadModelNew(fn);
	return 0;
	*/
}

void Parser::Test(string &testfile){
    time_t start_time, end_time;
    start_time = time(NULL);
	string type = "DP";
	BaseWriter result_wd(poptions->outfile, poptions->format, type);
	if(!result_wd.IsOpen())
		return;
	if(ppipe->InitFile(testfile) < 0){
		cerr << "Err: opening testfile " << testfile << endl;
		return;
	}
	cerr << "Processing sentences:" << flush;
	
	int i = 0;
	for(i = 0; ; i++) {
	    if((i+1) % 100 == 0) {
		cerr << (i+1) << "," << flush;
		//	end_time = time(NULL);
		//	cerr << (i+1) << "("<< (end_time-start_time)<< ")," << flush;
		//	break;
	    }
		Instance instance;
		ppipe->NextInstance(instance);
		if(instance.word_items.size() == 0)
			break;
		if(instance.word_items.size() > (poptions->max_sentlen)){
            cerr << "Err: Too long! Skipped! INFO:Len="<<instance.word_items.size()
                 << " Max_len="<<poptions->max_sentlen<< endl;
            continue;
        }

/*	    int length = instance.word_items.size();
	    // Get production crap.
		vector<FeatureVector> fvs;
		vector<double> probs;
		vector<FeatureVector> nt_fvs;//[.length][.types.length][2][2]
		vector<double> nt_probs;

        vector<FeatureVector> fvs_trip;//[.length][.length][2]
        vector<double> probs_trip;

        vector<FeatureVector> fvs_sib;//[.length][.length][2]
        vector<double> probs_sib;

		vector<FeatureVector> fvs_gc;//[.length][.length][2]
        vector<double> probs_gc;
	
		vector<int> trip_ids;
        vector<int> sib_ids;
        vector<int> gc_ids;
		//feature generation
		if(poptions->order == 2){
		ppipe->FillFeatureVectors2(instance,fvs,probs,fvs_trip, probs_trip, 
                                  fvs_sib, probs_sib, fvs_gc, probs_gc,trip_ids, sib_ids, gc_ids, nt_fvs,nt_probs,params);
		}
		else if(poptions->beamfeat){
		ppipe->FillFeatureVectors2(instance,fvs,probs,fvs_trip, probs_trip, 
                                  fvs_sib, probs_sib, fvs_gc, probs_gc,trip_ids, sib_ids, gc_ids, nt_fvs,nt_probs,params);
        }
		else{
		ppipe->FillFeatureVectors(instance,fvs,probs,nt_fvs,nt_probs,params);
		}
		//continue;	
	    //decoding
		int K = poptions->testK;
		vector<ResultItem > d;
        if(poptions->order == 2){
			if(poptions->beamfeat){
            decoder2.DecodeBeam(instance,fvs,probs,fvs_trip, probs_trip,
                                        fvs_sib, probs_sib, fvs_gc, probs_gc,trip_ids, sib_ids, gc_ids,
                                         nt_fvs,nt_probs,K, d, *ppipe, params);
			}
            else{
            decoder2.DecodeProjective(instance,fvs,probs,fvs_trip, probs_trip, 
								fvs_sib, probs_sib, fvs_gc, probs_gc,trip_ids, sib_ids, gc_ids,
                                nt_fvs,nt_probs,K, d, *ppipe);
			}
		}
		else if(poptions->beamfeat){
			decoder.DecodeBeam(instance,fvs,probs,fvs_trip, probs_trip,
                                fvs_sib, probs_sib, fvs_gc, probs_gc,trip_ids, sib_ids, gc_ids,nt_fvs,nt_probs,K, d, *ppipe, params);
		}
        else{
            decoder.DecodeProjective(instance,fvs,probs,nt_fvs,nt_probs,K, d, *ppipe);
 		}
*/		//output
		int K = poptions->testK;
        vector<ResultItem > d;
		TestOneSent(instance, d);
		if(d.size() > 0){
            for(int k = 0; k < d.size(); ++k){
                ppipe->TypeID2Str(d[k].parse_tree);
            }
			result_wd.OutputInstance( instance, d, K);
        }


	}
	ppipe->CloseFile();
    result_wd.CloseFile();
	end_time = time(NULL);
    cerr << (i) << "(Time:"<< (end_time-start_time)<< ")" << endl;
        
}
void Parser::TestOneSent(Instance &instance, vector<ResultItem > &d){
		ppipe->CreateFeatureVector(instance, instance.act_fv);
        int length = instance.word_items.size();
        // Get production crap.
        vector<FeatureVector> fvs;
        vector<double> probs;
        vector<FeatureVector> nt_fvs;//[.length][.types.length][2][2]
        vector<double> nt_probs;

        vector<FeatureVector> fvs_trip;//[.length][.length][2]
        vector<double> probs_trip;

        vector<FeatureVector> fvs_sib;//[.length][.length][2]
        vector<double> probs_sib;

        vector<FeatureVector> fvs_gc;//[.length][.length][2]
        vector<double> probs_gc;

        vector<int> trip_ids;
        vector<int> sib_ids;
        vector<int> gc_ids;
        //feature generation
        if(poptions->order == 2){
        ppipe->FillFeatureVectors2(instance,fvs,probs,fvs_trip, probs_trip,
                                  fvs_sib, probs_sib, fvs_gc, probs_gc,trip_ids, sib_ids, gc_ids, nt_fvs,nt_probs,params);
        }
        else if(poptions->beamfeat){
        ppipe->FillFeatureVectors2(instance,fvs,probs,fvs_trip, probs_trip,
                                  fvs_sib, probs_sib, fvs_gc, probs_gc,trip_ids, sib_ids, gc_ids, nt_fvs,nt_probs,params);
        }
        else{
        ppipe->FillFeatureVectors(instance,fvs,probs,nt_fvs,nt_probs,params);
        }
  
		int K = poptions->testK;
        if(poptions->order == 2){
            if(poptions->beamfeat){
            decoder2.DecodeBeam(instance,fvs,probs,fvs_trip, probs_trip,
                                        fvs_sib, probs_sib, fvs_gc, probs_gc,trip_ids, sib_ids, gc_ids,
                                         nt_fvs,nt_probs,K, d, *ppipe, params);
            }
            else{
            decoder2.DecodeProjective(instance,fvs,probs,fvs_trip, probs_trip,
                                fvs_sib, probs_sib, fvs_gc, probs_gc,trip_ids, sib_ids, gc_ids,
                                nt_fvs,nt_probs,K, d, *ppipe);
            }
        }
        else if(poptions->beamfeat){
            decoder.DecodeBeam(instance,fvs,probs,fvs_trip, probs_trip,
                                fvs_sib, probs_sib, fvs_gc, probs_gc,trip_ids, sib_ids, gc_ids,nt_fvs,nt_probs,K, d, *ppipe, params);
        }
        else{
            decoder.DecodeProjective(instance,fvs,probs,nt_fvs,nt_probs,K, d, *ppipe);
        }

}
