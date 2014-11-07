#include "Tagger.h"
#include <ctime>
#include "FeatureVector.h"
#include "MIRA.h"
#include "MyTool.h"
Tagger::Tagger(POSPipe *p1, SymOptions *p2){
	ppipe = p1;
	poptions = p2;
	params.Init(p1->feat_list.GetSize());
}

void Tagger::Train(string &trainfile){
	
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

void Tagger::TrainingIter(string &trainfile, int iter){
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
	decoder.types_num = ppipe->types_num;
	decoder.root_type = ppipe->root_type;
	MIRA mira;
	int total_n = 0;
	int err_n = 0;
	for(int i = 0; i < instance_num; i++) {
	    if((i+1) % 1000 == 0) {
			cerr << (i+1) << ","<< flush; 
			end_time = time(NULL);
			
	//		cerr << "("<< (end_time-start_time)<< ")," << flush;
	//		break;
	    }
		Instance instance;
		ppipe->NextInstance(instance);
		if(instance.word_items.size() == 0)
			continue;

	    int length = instance.word_items.size();
		//cerr << length << ",";
	    // Get production crap.
		vector<FeatureVector> fvs;//[.length][.length][2]
		vector<double> probs;

		//cerr << fvs.size() << "-" << fvs.capacity()<< " " << length << " ";
		
	//	ppipe->FillFeatureVectors(instance,fvs,probs,nt_fvs,nt_probs,params);
		ppipe->ReadInstance(feat_rd, instance,fvs,probs,params);
		//continue;
		//cerr << i <<":"<< fvs.size() <<"-" << fvs.capacity()<< endl;
	    double upd = (double)(poptions->num_iters*instance_num - (instance_num*(iter-1)+(i+1)) + 1);
		int K = poptions->trainK;
		vector<ResultItem > d;
		decoder.DecodeProjective(instance,fvs,probs,K, d);
		//cerr << "\nGold:" << instance.act_parsetree << endl;
		//cerr <<   "PRED:" << d[0].parse_tree << "<--"<< endl;
		
	    //
		FeatureVector fv;
		fv.CopyFrom(instance.act_fv);
		vector<double> num_errs;
		for(int k = 0; k < K && k < d.size(); ++k){
			ppipe->CreateTagFeatureVector(instance, d[k].parse_tree, d[k].fv);
			double err = ppipe->GetNumErrors(instance, d[k].parse_tree, instance.act_parsetree);
			num_errs.push_back(err);
			if(k == 0) err_n += err;
		//	cerr << "Err:" << err << endl;
		}
		total_n += length;
		double score = fv.GetScore(params);
		double score2 = d[0].fv.GetScore(params);
		//cerr << "Scores:" << score << " vs "<< score2 << endl;
		mira.UpdateParamsMIRA(fv,d,upd, num_errs, params);
		//if(i >= 2)break;
		if(true) continue;

	}
	feat_rd.close();
	ppipe->CloseFile();
	cerr << instance_num << ",ErrRate=" << ((double)err_n/total_n);
}

int Tagger::LoadModelNew(string &model_name){
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


    vector<string> typepairs;
    vector<long> typepair_vals;
	MyTool::VectorRead(model_rd,typepairs);
	MyTool::VectorRead(model_rd,typepair_vals);

    ppipe->feat_list.FromArray(feats, feat_vals);
    ppipe->type_list.FromArray(types,type_vals);
    ppipe->typepair_list.FromArray(typepairs,typepair_vals);
	
	model_rd.close();
	ppipe->CloseAlphabets();

}

void Tagger::SaveModelNew(string &model_name){
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

	vector<string> typepairs;
    vector<long> typepair_vals;
    ppipe->typepair_list.ToArray(typepairs, typepair_vals);
	MyTool::VectorWrite(model_wd, typepairs);
    MyTool::VectorWrite(model_wd, typepair_vals);
	model_wd.close();
}
void Tagger::SaveModel(string &model_name){
	SaveModelNew(model_name);
	return;
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
	cerr << params.parameters.size() << flush << endl;

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

	vector<string> typepairs;
    vector<long> typepair_vals;
    ppipe->typepair_list.ToArray(typepairs, typepair_vals);
    oa << typepairs;
    oa << typepair_vals;

	model_wd.close();
*/

}

int Tagger::LoadModel(string &model_name){
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

	vector<string> typepairs;
    vector<long> typepair_vals;
    ia >> typepairs;
    ia >> typepair_vals;

	ppipe->feat_list.FromArray(feats, feat_vals);
	ppipe->type_list.FromArray(types,type_vals);
	ppipe->typepair_list.FromArray(typepairs,typepair_vals);
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

void Tagger::Test(string &testfile){
    time_t start_time, end_time;
    start_time = time(NULL);
	string type = "POS";
	BaseWriter result_wd(poptions->outfile, poptions->format, type);
	if(!result_wd.IsOpen())
		return;
	if(ppipe->InitFile(testfile) < 0){
		cerr << "Err: opening testfile " << testfile << endl;
		return;
	}
	cerr << "Processing sentences:" << flush;
	decoder.types_num = ppipe->types_num;
	
	int i = 0;
	for(i = 0; ; i++) {
	    if((i+1) % 100 == 0) {
		cerr << (i+1) << "," << flush;
			end_time = time(NULL);
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

		vector<ResultItem > d;
		int K = poptions->testK;
		TestOneSent(instance, d);
	    /*int length = instance.word_items.size();
	    // Get production crap.
        vector<FeatureVector> fvs;//[.length][.length][2]
        fvs.reserve(length*ppipe->types_num*ppipe->types_num);
        vector<double> probs;
        probs.reserve(length*ppipe->types_num*ppipe->types_num);


		//feature generation
		ppipe->FillFeatureVectors(instance,fvs,probs,params);
		//continue;	
	    //decoding
		decoder.DecodeProjective(instance,fvs,probs,K, d);
		*/
		//output
		if(d.size() > 0){
			for(int k = 0; k < K && k < d.size(); ++k){
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

void Tagger::TestOneSent(Instance &instance, vector<ResultItem > &d){

		ppipe->PreprocessInstance(instance);

	    int length = instance.word_items.size();
        // Get production crap.
        vector<FeatureVector> fvs;//[.length][.length][2]
        fvs.reserve(length*ppipe->types_num*ppipe->types_num);
        vector<double> probs;
        probs.reserve(length*ppipe->types_num*ppipe->types_num);


        //feature generation
        ppipe->FillFeatureVectors(instance,fvs,probs,params);
		//return;
        //continue;
        //decoding
        int K = poptions->testK;
        decoder.DecodeProjective(instance,fvs,probs,K, d);
 //		cerr << d[0].parse_tree << endl;
}

void Tagger::TestOneSent(Instance &instance){
	vector<ResultItem > d;
	TestOneSent(instance, d);
	if(d.size() > 0){
		//only k = 0;
		string best_tree = d[0].parse_tree;
//		cerr << best_tree << endl;
		ppipe->TypeID2Str(best_tree);
//		cerr <<" "<< best_tree << endl;
		//addToInstance
		vector<string> tag_items;
	    boost::split(tag_items, best_tree, boost::is_any_of(" "));
    	for(int i = 0; i < tag_items.size()-1; i++) {
        	instance.word_items.at(i+1).postag = tag_items[i].c_str();
        	instance.word_items.at(i+1).cpostag = tag_items[i].c_str();
			string postag = instance.word_items.at(i+1).postag;
			if(tag_items[i].length() > 1 && (postag.compare("PRP") != 0 && postag.compare("PRP$") != 0))
        		instance.word_items.at(i+1).cpostag = tag_items[i].substr(0,2);

    	}

   }


}
