#include "BaseTagPipe.h"
#include "MyTool.h"
#include <stdlib.h>
#define NONPUNC "nonpunc"

BaseTagPipe::BaseTagPipe(){
	types = NULL;
	loss_type = NONPUNC;
	//cerr << "Error: Should not call this BaseTagPipe()" << endl;
	start_tag = "TSTART";
    end_tag = "TEND";
	//mydict.Load();
}

BaseTagPipe::~BaseTagPipe(){
	if(types){
	//	cerr << "Free BaseTagPipe" << flush << endl;
		delete[] types;
	}
}

BaseTagPipe::BaseTagPipe(SymOptions *options){
	//cerr << "Error: Should not call this BaseTagPipe(SymOptions *options)" << endl;
	loss_type = NONPUNC;
	poptions = options;
	types = NULL;		
	mydict.Load(options->lang);
	start_tag = "TSTART";
    end_tag = "TEND";
}
void BaseTagPipe::CloseFile(){
	rd_data.CloseFile();
}

void BaseTagPipe::GetNext(Instance &inst){
	rd_data.GetNext(inst);
	PreprocessInstance(inst);
	//set the START Tag / END Tag
	if(inst.word_items.size() > 2){
		inst.word_items[0].seqtag = start_tag;
		inst.word_items[inst.word_items.size()-1].seqtag = end_tag;
	}
}


void BaseTagPipe::NextInstance(Instance &inst) {
	GetNext(inst);
	if (inst.word_items.size() == 0) 
		return;
	inst.word_num = inst.word_items.size();
	CreateFeatureVector(inst, inst.act_fv);

	vector<WordItem>::iterator pit = inst.word_items.begin();

	stringstream spans;
	int i = 1;
	pit++;//skip root
	while(pit != inst.word_items.end()){
		spans << type_list.LookupIndex((pit)->seqtag)<< " ";
		pit++;
		i++; 
	}
	string s = spans.str();
	inst.act_parsetree = s.substr(0, s.length()-1);
	//cerr << inst.act_parsetree << "==" << endl;
}

void BaseTagPipe::CreateInstances(string &file,	 string &feat_filename) {

	CreateAlphabet(file);

	ofstream feat_wd;
	feat_wd.open(feat_filename.c_str(), ios::binary);
    if(!feat_wd.is_open()){
        cerr << "Err: opening "<< feat_filename << endl;
        return ;
    }
	
    cerr << "Creating Instance Feat File ... "  << flush;
	
    rd_data.InitFile(file);
	time_t start_time;
    start_time = time(NULL);

    Instance instance;
    instance_num = 0;
	NextInstance(instance);
    while(instance.word_items.size() != 0) {
        instance_num++;
		if(instance_num % 100 == 0){
			cerr << instance_num << "," << flush;
			time_t end_time;
 		  	end_time = time(NULL);
			//cerr << (end_time-start_time) << "]" << flush; 
		}
		if(poptions->feat_memo){
			int length = instance.word_items.size();
	        vector<FeatureVector> fvs;//[.length][.length][2]
	        fvs.reserve((length-1)*types_num*types_num);
      		FillFeatureVectors(instance,fvs);
			allFVs.push_back(fvs);
		}
		else{
		WriteInstance(instance, feat_wd);
		}
        NextInstance(instance);
    }

    rd_data.CloseFile();
	
	feat_wd.close();
	cerr << instance_num << endl;
    cerr << "Have "<< instance_num << " instances. Creating Instance FeatFile Done." << endl;
		
		
}


void BaseTagPipe::CreateAlphabet(string &file) {
	//return;
	cerr << "Creating Alphabet ... " << flush;
	rd_data.InitFile(file);

	Instance instance;
	instance_num = 0;
	GetNext(instance);
	//always set end_tag 0 and start_tag 1
	type_list.LookupIndex(end_tag);//0
	type_list.LookupIndex(start_tag);//1
	while(instance.word_items.size() != 0) {
		instance_num++;
	    vector<WordItem>::iterator pit = instance.word_items.begin();
	 	while(pit != instance.word_items.end()){
			type_list.LookupIndex((pit)->seqtag);
			//cerr << (pit)->seqtag << endl;
			pit++;
		}
		FeatureVector fv;
	    CreateFeatureVector(instance, fv);
		instance.Clear();	
	    GetNext(instance);
		//cerr << instance_num <<" " << flush;
	}
	CloseAlphabets();
	rd_data.CloseFile();

	cerr << "Have "<< instance_num << " instances. Creating Alphabet Done." << flush << endl;
}
	
void BaseTagPipe::CloseAlphabets() {
	feat_list.StopGrowth();
	type_list.StopGrowth();
	typepair_list.StopGrowth();
	word_list.StopGrowth();
	vector<string> keys;
	vector<long> key_vals;
	type_list.ToArray(keys, key_vals);
	types = new string[keys.size()];
	for(int i = 0; i < keys.size(); i++) {
	    int idx = type_list.LookupIndex(keys.at(i));
		//cerr << idx <<"="<< keys.size()<< "="<< keys.at(i) << endl;
	    types[idx] = keys.at(i);
	}
	types_num = keys.size();
	cerr << " Features Num:" << feat_list.mytable.size() << endl; 
	//cerr << "Words Num  :" << word_list.mytable.size() << endl;
	cerr << " Types Num  :" << types_num << endl;
	//cerr << "Type pairs Num  :" << typepair_list.mytable.size() << endl;
}


    // add with default 1.0
int BaseTagPipe::Add(string &feat, FeatureVector &fv) {
	int num = feat_list.LookupIndex(feat);
	//return 1;
	//int num = 1;
	//cerr << feat << "=" << num << endl;
	if(num >= 0)
	    fv.Add(num, 1.0);
	return num;
}
int BaseTagPipe::Add(string &feat, string &suffix, FeatureVector &fv) {
	AddTwo(feat, suffix, fv);
}
int BaseTagPipe::AddTwo(string &feat, string &suffix, FeatureVector &fv) {
	int num = Add(feat, fv);
	if(num >= 0){
		//feat += "*"+suffix;
		feat += (suffix);
		num = Add(feat, fv);
	}
    return num;
}

	
void BaseTagPipe::CreateFeatureVector(Instance &instance, FeatureVector &fv) {

	int inst_length = instance.word_items.size();
	vector<WordItem>::iterator it = instance.word_items.begin();
	++it;
	int i = 1;
	for(; i < inst_length; i++, ++it) {
		string curT = (it)->seqtag;
		string preT = (it-1)->seqtag;
		string curW = (it)->form;
		word_list.LookupIndex(curW);
		typepair_list.LookupIndex(preT+" "+curT);
		//cerr << "I:" << i <<it->form<< " " << preT << " " << curT << endl;
	    AddCoreFeatures(instance,i,curT,fv);
		
	    AddTagFeatures(instance,i,preT,curT,fv);
	}

	return;
}

void BaseTagPipe::WordType(string &w0, string &type){
	//contains number/UPPERCASE/ hyphen

    if(w0.at(0) <= 'Z' && w0.at(0) >= 'A'){
		type = "TAZ";
    }
	else if(w0.at(0) <= '9' && w0.at(0) >= '0'){
		type = "T09";
    }
    else if(w0.find("-") >= 0){
		type = "THP";
    }
	else{
		type = "T0";
	}
	
}


void BaseTagPipe::CreateTagFeatureVector(Instance &instance, string &tag, FeatureVector &fv) {
    Instance new_inst;
    new_inst.word_items = instance.word_items;
    new_inst.word_num = instance.word_items.size();
	string newtags = tag;
	TypeID2Str(newtags);
    vector<string> tag_items;
    boost::split(tag_items, newtags, boost::is_any_of(" "));

    for(int i = 0; i < tag_items.size() && i+1 < new_inst.word_num; i++) {
		new_inst.word_items[i+1].seqtag = tag_items[i];
		//new_inst.word_items[i+1].postag = tag_items[i];
    }
    CreateFeatureVector(new_inst, fv);
}

//void BaseTagPipe::FillFeatureVectors(Instance &instance,
//                            vector<FeatureVector> &fvs,  vector<double> &probs,
//                             Parameters &params){
//	FillFeatureVectors(instance, fvs);
//	ScoreFeatureVectors(instance, fvs, probs, params);
//}

void BaseTagPipe::ScoreFeatureVectors(Instance &instance,
                            vector<FeatureVector> &fvs,  vector<double> &probs,
                             Parameters &params){
	vector<FeatureVector>::iterator it = fvs.begin();
	probs.reserve(fvs.size());
    while(it != fvs.end()){
        double prob = it->GetScore(params);
        probs.push_back(prob);
        ++it;
    }
}
void BaseTagPipe::FillFeatureVectors(Instance &instance,
                            vector<FeatureVector> &fvs){
}
void BaseTagPipe::FillFeatureVectors(Instance &instance,
                            vector<FeatureVector> &fvs,  vector<double> &probs,
                             Parameters &params){
	int instance_length = instance.word_items.size();
	//types_num
	// Get production crap.	
	fvs.reserve(instance_length*types_num+ instance_length*types_num*types_num);
	probs.reserve(instance_length*types_num+ instance_length*types_num*types_num);
	int fv_maxsize = 10;
	int idx = 0;
    FeatureVector fv;
	for(int w1 = 1; w1 < instance_length; w1++) {
        for(int wc = 0; wc < types_num; wc++) {
			fv.Clear();
			fv.Reserve(fv_maxsize);
			
            AddCoreFeatures(instance,w1, types[wc], fv);
            //cerr << w1 <<" " << w2 << " "<< ph <<" idx:" <<idx++ <<endl;
            double prob = fv.GetScore(params);
            //fvs.push_back(fv);
            probs.push_back(prob);
			if(fv_maxsize < fv.GetFeatSize())
				fv_maxsize = fv.GetFeatSize();

        }
    }

	for(int w1 = 1; w1 < instance_length; w1++) {
		for(int wc = 0; wc < types_num; wc++) {//current
	    for(int w2 = 0; w2 < types_num; w2++) {//preT
			fv.Clear();
			fv.Reserve(fv_maxsize); 
		    AddTagFeatures(instance,w1, types[w2],types[wc], fv);
			//cerr << w1 <<" " << w2 << " "<< ph <<" idx:" <<idx++ <<endl;
		    double prob = fv.GetScore(params);
			//fvs.push_back(fv);
		    probs.push_back(prob);
			if(fv_maxsize < fv.GetFeatSize())
                fv_maxsize = fv.GetFeatSize();
		}
	    }
	}
	

}

void BaseTagPipe::WriteInstance(Instance &instance, ofstream &feat_wd){
	int instance_length = instance.word_items.size();
    // Get production crap.a
	//boost::archive::binary_oarchive oa(feat_wd);
    int idx = 0;
	vector<int > keys;
	keys.reserve(10000);
	int fv_maxsize = 10;
    FeatureVector fv;
	for(int w1 = 1; w1 < instance_length; w1++) {
        for(int wc = 0; wc < types_num; wc++) {
			fv.Clear();
			fv.Reserve(fv_maxsize);
            AddCoreFeatures(instance,w1, types[wc], fv);
            //cerr << w1 <<" " << w2 << " "<< ph <<" idx:" <<idx++ <<endl;
            fv.GetKeys(keys);
			if(fv_maxsize < fv.GetFeatSize())
                fv_maxsize = fv.GetFeatSize();
        }
    }
    for(int w1 = 1; w1 < instance_length; w1++) {
        for(int wc = 0; wc < types_num; wc++) {
        for(int w2 = 0; w2 < types_num; w2++) {
			fv.Clear();
			fv.Reserve(fv_maxsize);
            AddTagFeatures(instance,w1, types[w2],types[wc], fv);
            //cerr << w1 <<" " << w2 << " "<< ph <<" idx:" <<idx++ <<endl;
			fv.GetKeys(keys);
			if(fv_maxsize < fv.GetFeatSize())
                fv_maxsize = fv.GetFeatSize();
        }
        }
    }
	MyTool::VectorWrite(feat_wd, keys);
	//int keys_len = keys.size();
	//oa << keys_len;
	//oa << keys;
}
void BaseTagPipe::ReadInstance(ifstream &feat_rd, Instance &instance,
                            vector<FeatureVector> &fvs,  vector<double> &probs,
                             Parameters &params){
	vector<FeatureVector>::iterator fv_it;
    int instance_length = instance.word_items.size();

    fvs.reserve(instance_length*types_num + instance_length*types_num*types_num);
    probs.reserve(instance_length*types_num + instance_length*types_num*types_num);


    int idx = 0;
    //boost::archive::binary_iarchive ia(feat_rd);
	vector<int > keys;
	MyTool::VectorRead(feat_rd, keys);
	//int keys_len = 0;
	//ia >> keys_len;
	//keys.reserve(keys_len);
	//ia >> keys;
	int keys_idx = 0;
	int keys_size = 0;
    FeatureVector fv;
	for(int w1 = 1; w1 < instance_length; w1++) {
        for(int wc = 0; wc < types_num; wc++) {
			fv.Clear();
            fvs.push_back(fv);
			fv_it = fvs.end()-1;
            keys_size = keys.at(keys_idx++);
            //cerr << keys_idx << " " << keys_size << endl;
            if(keys_size > 0){
				fv_it->Reserve(keys_size);
                fv_it->Add(keys, keys_idx, keys_size);
                keys_idx += keys_size;
            }
            double prob = fv_it->GetScore(params);
            probs.push_back(prob);
            idx++;
        }
    }

	for(int w1 = 1; w1 < instance_length; w1++) {
        for(int wc = 0; wc < types_num; wc++) {
        for(int w2 = 0; w2 < types_num; w2++) {

			fv.Clear();
			fvs.push_back(fv);
            fv_it = fvs.end()-1;
			keys_size = keys.at(keys_idx++);
			//cerr << keys_idx << " " << keys_size << endl; 
			if(keys_size > 0){
				fv_it->Reserve(keys_size);
				fv_it->Add(keys, keys_idx, keys_size);
				keys_idx += keys_size;
			}
			double prob = fv_it->GetScore(params);
            probs.push_back(prob);
			idx++;
        }
        }
    }

}
double BaseTagPipe::GetNumErrors(Instance &instance, string &tag, string &gold){
    return GetNumErrors2(instance, tag, gold);
}
double BaseTagPipe::GetNumErrors2(Instance &instance, string &tag, string &gold){

    double err;
    vector<string> tag_items;
    vector<string> gold_items;
    boost::split(tag_items, tag, boost::is_any_of(" "));
    boost::split(gold_items, gold, boost::is_any_of(" "));


    int correct = 0;

    for(int i = 0; i < tag_items.size(); i++) {
		if(tag_items.at(i) == gold_items.at(i))
			correct++;
    }
    err = tag_items.size() -  correct;
/*  cerr << "===" << endl;
    cerr << gold << endl;
    cerr << tag << endl;
    cerr << err << endl;
*/  return err;
}

double BaseTagPipe::TypeID2Str(string &s){
	vector<string> tag_items;
    boost::split(tag_items, s, boost::is_any_of(" "));
	string new_tags = "";
	for(int i = 0; i < tag_items.size(); i++) {
		int idx = atoi(tag_items[i].c_str());
		new_tags += types[idx]+" ";
    }

	s = new_tags.substr(0, new_tags.length()-1);
}
