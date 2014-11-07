#include "POSPipe.h"
#include "MyTool.h"
#include <stdlib.h>
#define NONPUNC "nonpunc"
POSPipe::POSPipe(){
	types = NULL;
	loss_type = NONPUNC;
	//mydict.Load();
}

POSPipe::~POSPipe(){
	if(types)
		delete[] types;
}

POSPipe::POSPipe(SymOptions *options){
	loss_type = NONPUNC;
	poptions = options;
	types = NULL;		
	mydict.Load(options->lang);
	rd_data.SetFormat(poptions->format);
	rd_data.SetType("POS");

	start_tag = "TSTART";
	end_tag = "TEND";
}
int POSPipe::InitFile (string file){
	return rd_data.InitFile(file);
}
void POSPipe::CloseFile(){
	rd_data.CloseFile();
}

void POSPipe::GetNext(Instance &inst){
	rd_data.GetNext(inst);
	if (inst.word_items.size() > 0)
		PreprocessInstance(inst);
}

void POSPipe::PreprocessInstance(Instance &inst){
	for(int i = 1; i < (inst.word_items.size()-1);i++){
		WordType(inst.word_items.at(i).form, inst.word_items.at(i).lemma);
	}
}

void POSPipe::NextInstance(Instance &inst) {
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
		spans << type_list.LookupIndex((pit)->cpostag)<< " ";
		pit++;
		i++; 
	}
	string s = spans.str();
	inst.act_parsetree = s.substr(0, s.length()-1);
	//cerr << inst.act_parsetree << "==" << endl;
}

void POSPipe::CreateInstances(string &file,	 string &feat_filename) {

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


void POSPipe::CreateAlphabet(string &file) {

	cerr << "Creating Alphabet ... " << flush;
	rd_data.InitFile(file);

	Instance instance;
	instance_num = 0;
	GetNext(instance);
	type_list.LookupIndex(end_tag);//0
	type_list.LookupIndex(start_tag);//1
	while(instance.word_items.size() != 0) {
		instance_num++;
	    vector<WordItem>::iterator pit = instance.word_items.begin();
	 	while(pit != instance.word_items.end()){
			type_list.LookupIndex((pit)->cpostag);
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
	
void POSPipe::CloseAlphabets() {
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
	//	cerr << idx <<"="<< keys.size()<< "="<< keys.at(i) << endl;
	    types[idx] = keys.at(i);
	}
	types_num = keys.size();
	cerr << " Features Num:" << feat_list.mytable.size() << endl; 
	//cerr << "Words Num  :" << word_list.mytable.size() << endl;
	cerr << " Types(POS) Num  :" << types_num << endl;
	//cerr << "Type pairs Num  :" << typepair_list.mytable.size() << endl;
}


    // add with default 1.0
int POSPipe::Add(string &feat, FeatureVector &fv) {
	int num = feat_list.LookupIndex(feat);
	//return 1;
	//int num = 1;
	//cerr << feat << "=" << num << endl;
	if(num >= 0)
	    fv.Add(num, 1.0);
	return num;
}
int POSPipe::Add(string &feat, string &suffix, FeatureVector &fv) {
	AddTwo(feat, suffix, fv);
}
int POSPipe::AddTwo(string &feat, string &suffix, FeatureVector &fv) {
	int num = Add(feat, fv);
	if(num >= 0){
		//feat += "*"+suffix;
		feat += (suffix);
		num = Add(feat, fv);
	}
    return num;
}

	
void POSPipe::CreateFeatureVector(Instance &instance, FeatureVector &fv) {

	int inst_length = instance.word_items.size();
	vector<WordItem>::iterator it = instance.word_items.begin();
	++it;
	int i = 1;
	for(; i < inst_length; i++, ++it) {
		string curT = (it)->cpostag;
		string preT = (it-1)->cpostag;
		string curW = (it)->form;
		word_list.LookupIndex(curW);
		typepair_list.LookupIndex(preT+" "+curT);
		//cerr << "I:" << i <<it->form<< " " << preT << " " << curT << endl;
	    AddCoreFeatures(instance,i,curT,fv);
		
	    AddTagFeatures(instance,i,preT,curT,fv);
	}

	return;
}

void POSPipe::WordType(string &w0, string &type){
	//contains number/UPPERCASE/ hyphen

    if(w0.at(0) <= 'Z' && w0.at(0) >= 'A'){
		type = "TAZ";
    }
	else if(w0.at(0) <= '9' && w0.at(0) >= '0'){
		type = "T09";
    }
    else if(w0.find("-") != string::npos){
		type = "THP";
    }
	else{
		type = "T0";
	}
	
}

void POSPipe::CNWordType(string &w, string &code){
    code = "0";
    if(w.length() == 0)
        return;
    char c = w.at(0);
    string val;
    mydict.GetDictItemVal(mydict.PU_list, w, val);
    if((c >= '0' && c <= '9') || val == "NUM" || (w.compare("£°") >= 0 && w.compare("£¹") <= 0))
        code = "1";//include 0-9, Chinese num
    else if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')
           || (w.compare("£Á") >= 0 && w.compare("£Ú") <= 0)|| (w.compare("£á") >= 0 && w.compare("£ú") <= 0))//Chinese
        code = "2";
    else if(val == "PU")
        code = "3";//for Chinese/English punctuations
    else
        code = "0";//others
    return;

}

void POSPipe::AddCoreFeatures(Instance &instance,
                int cur_i,
                string curT,
                FeatureVector &fv) {
	if(poptions->lang == "cn")
		AddCoreFeaturesCN(instance, cur_i, curT, fv);
	else//default en
		AddCoreFeaturesSIZUKI(instance, cur_i, curT, fv);
	return;
	if((poptions->debug_level) % 10 == 1){
		AddCoreFeaturesMXPOST(instance, cur_i, curT, fv);
	}
	else{//0 default
		AddCoreFeaturesSIZUKI(instance, cur_i, curT, fv);
	}
	if((poptions->debug_level/10) % 10 == 1){
		AddSBFeatures(instance, cur_i, curT, fv);
	}
}

void POSPipe::AddSBFeatures(Instance &instance,
                int cur_i,
                string curT,
                FeatureVector &fv) {
	vector<WordItem >::iterator it = instance.word_items.begin();
    int len = instance.word_items.size();
	string feat;
    string w0, w_1;
    w0 = (it+cur_i)->form;
    w_1 = (it+cur_i-1)->form;
    //addtional features from unlabeled data
    string entry = "";
    string val = "ZERO";
    entry.append("N1 ").append(w0).append(" ").append(curT);

    unordered_map<string, string>::iterator wit = mydict.wordpos_list.find(entry);

    if(wit != mydict.wordpos_list.end()){
        val = wit->second;
    }
	if(val == "ZERO" && (poptions->debug_level/100000) % 10 == 1)
        return;

	if((poptions->debug_level/100) % 10 == 1){
    //feat = "SB:"+val;
	feat = "";
    feat.append("SB:").append(val);
    Add(feat,  fv);
    //cerr << feat << endl;
	feat.append(curT);
    Add(feat,  fv);
	}
	entry = "";
	entry.append("N2 ").append(w_1).append(" ").append(w0).append(" ").append(curT);
    //entry = "N2 "+w_1+" "+w0+" "+curT;

    wit = mydict.wordpos_list.find(entry);
    val = "ZERO";
    if(wit != mydict.wordpos_list.end()){
        val = wit->second;
    }
	if(val == "ZERO" && (poptions->debug_level/100000) % 10 == 1)
        return;

	if((poptions->debug_level/1000) % 10 == 1){
    //feat = "SB2:"+val;
	feat = "";
    feat.append("SB2:").append(val);
	Add(feat,  fv);
    //cerr << feat << endl;
    feat.append(curT);
    Add(feat,  fv);
    }

}
void POSPipe::AddCoreFeaturesSIZUKI(Instance &instance,
                int cur_i,
                string curT,
                FeatureVector &fv) {
    //features
    //form i, i-1,i-2, i+1, i+2
    //tag i, i-1
    //prefix //suffix
    //contains number/UPPERCASE/ hyphen
    vector<WordItem >::iterator it = instance.word_items.begin();
    int len = instance.word_items.size();

	string wtp0, wtp1, wtp2, wtp_1, wtp_2;
	string w0, w1, w2, w_1, w_2;
    w0 = (it+cur_i)->form;
	wtp0 = (it+cur_i)->lemma;
	//cerr << cur_i << w0 << "\t" << wtp0 << endl;
    w_1 = (it+cur_i-1)->form;
	wtp_1 = (it+cur_i-1)->lemma;

    w_2 = "WSTART2";
	wtp_2 = "TSAT2";
    if(cur_i-2>=0){
        w_2 = (it+cur_i-2)->form;
		wtp_2 = (it+cur_i-2)->lemma;
	}

    w1 = "WEND1";
	wtp1 = "TEND1";
    if(cur_i+1 < len){
        w1 = (it+cur_i+1)->form;
        wtp1 = (it+cur_i+1)->lemma;
	}
    w2 = "WEND2";
	wtp2 = "TEND2";
    if(cur_i+2 < len){
        w2 = (it+cur_i+2)->form;
        wtp2 = (it+cur_i+2)->lemma;
	}

    string feat;
	feat.reserve(100);
	//curT
	//feat = "CT:"+curT;
	feat = "";
	feat.append("CT:").append(curT);
	Add(feat, fv);
    //form i, i-1,i-2, i+1, i+2
    //feat = "W0:"+w0+":"+curT;
	feat = "";
    feat.append("W0:").append(w0).append(":").append(curT);
    Add(feat, fv);
	feat = "";
    feat.append("W_1:").append(w_1).append(":").append(curT);
    //feat = "W_1:"+w_1+":"+curT;
    Add(feat, fv);
	feat = "";
    feat.append("W_2:").append(w_2).append(":").append(curT);
    //feat = "W_2:"+w_2+":"+curT;
    Add(feat, fv);
	feat = "";
    feat.append("W1:").append(w1).append(":").append(curT);
    //feat = "W1:"+w1+":"+curT;
    Add(feat, fv);
	feat = "";
    feat.append("W2:").append(w2).append(":").append(curT);
    //feat = "W2:"+w2+":"+curT;
    Add(feat, fv);
	//bi
	feat = "";
    feat.append("W0:").append(w0).append(":").append(w_1).append(curT);
	//feat = "W0:"+w0+":"+w_1+curT;
    Add(feat, fv);
	feat = "";
    feat.append("W_1:").append(w_1).append(":").append(w_2).append(curT);
    //feat = "W_1:"+w_1+":"+w_2+curT;
    Add(feat, fv);
	feat = "";
    feat.append("W1:").append(w1).append(":").append(w0).append(curT);
    //feat = "W1:"+w1+":"+w0+curT;
    Add(feat, fv);
	feat = "";
    feat.append("W2:").append(w2).append(":").append(w1).append(curT);
    //feat = "W2:"+w2+":"+w1+curT;
    Add(feat, fv);
	//wtp
	feat = "";
    feat.append("WT0:").append(wtp0).append(":").append(curT);
	//feat = "WT0:"+wtp0+":"+curT;
    Add(feat, fv);
	feat = "";
    feat.append("WT_1:").append(wtp_1).append(":").append(curT);
    //feat = "WT_1:"+wtp_1+":"+curT;
    Add(feat, fv);
	feat = "";
    feat.append("WT_2:").append(wtp_2).append(":").append(curT);
    //feat = "WT_2:"+wtp_2+":"+curT;
    Add(feat, fv);
	feat = "";
    feat.append("WT1:").append(wtp1).append(":").append(curT);
    //feat = "WT1:"+wtp1+":"+curT;
    Add(feat, fv);
	feat = "";
    feat.append("WT2:").append(wtp2).append(":").append(curT);
    //feat = "WT2:"+wtp2+":"+curT;
    Add(feat, fv);
    //bi
	feat = "";
    feat.append("WT0:").append(wtp0).append(":").append(wtp_1).append(curT);
    //feat = "WT0:"+wtp0+":"+wtp_1+curT;
    Add(feat, fv);
	feat = "";
    feat.append("WT_1:").append(wtp_1).append(":").append(wtp_2).append(curT);
    //feat = "WT_1:"+wtp_1+":"+wtp_2+curT;
    Add(feat, fv);
	feat = "";
    feat.append("WT1:").append(wtp1).append(":").append(wtp0).append(curT);
    //feat = "WT1:"+wtp1+":"+wtp0+curT;
    Add(feat, fv);
	feat = "";
    feat.append("WT2:").append(wtp2).append(":").append(wtp1).append(curT);
    //feat = "WT2:"+wtp2+":"+wtp1+curT;
    Add(feat, fv);

    for(int j = 1; j <=9;++j){
    string pref = w0.substr(0,j);
    int k = w0.length()-j;
    if(k < 0) k = 0;
    string suff = w0.substr(k,j);
	stringstream sj;
	sj << j;
	feat = "";
	feat.append("PF:").append(sj.str()).append(pref).append(":").append(curT);
    Add(feat, fv);
	feat = "";
	feat.append("SF:").append(sj.str()).append(suff).append(":").append(curT);
    //feat = "SF:"+j+suff+":"+curT;
    Add(feat, fv);
    }

}

void POSPipe::AddCoreFeaturesMXPOST(Instance &instance,
				int cur_i,
				string curT,
				FeatureVector &fv) {
	//features
	//form i, i-1,i-2, i+1, i+2
	//tag i, i-1
	//prefix //suffix
	//contains number/UPPERCASE/ hyphen
	vector<WordItem >::iterator it = instance.word_items.begin();
	int len = instance.word_items.size();

	string w0 = (it+cur_i)->form;
	string w_1 = (it+cur_i-1)->form;
	string w_2 = "WSTART2";
	if(cur_i-2>=0)
		w_2 = (it+cur_i-2)->form;
	string w1 = "WEND1";
	if(cur_i+1 < len)
		w1 = (it+cur_i+1)->form;
	string w2 = "WEND2";
    if(cur_i+2 < len)
        w2 = (it+cur_i+2)->form;
	//form i, i-1,i-2, i+1, i+2
	string feat ="";

    feat = "";
    feat.append("CT:").append(curT);
    Add(feat, fv);
    //form i, i-1,i-2, i+1, i+2
    //feat = "W0:"+w0+":"+curT;
    feat = "";
    feat.append("W0:").append(w0).append(":").append(curT);
    Add(feat, fv);
    feat = "";
    feat.append("W_1:").append(w_1).append(":").append(curT);
    //feat = "W_1:"+w_1+":"+curT;
    Add(feat, fv);
    feat = "";
    feat.append("W_2:").append(w_2).append(":").append(curT);
    //feat = "W_2:"+w_2+":"+curT;
    Add(feat, fv);
    feat = "";
    feat.append("W1:").append(w1).append(":").append(curT);
    //feat = "W1:"+w1+":"+curT;
    Add(feat, fv);
    feat = "";
    feat.append("W2:").append(w2).append(":").append(curT);
    //feat = "W2:"+w2+":"+curT;
    Add(feat, fv);

    //tag i, i-1
    //prefix //suffix
	for(int j = 1; j <=4;++j){
	string pref = w0.substr(0,j);
	int k = w0.length()-j;
	if(k < 0) k = 0;
	stringstream sj;
    sj << j;
	string suff = w0.substr(k,j);
    feat = "";
    feat.append("PF:").append(sj.str()).append(pref).append(":").append(curT);
    //feat = "PF:"+j+pref+":"+curT;
    Add(feat, fv);
    feat = "";
    feat.append("SF:").append(sj.str()).append(suff).append(":").append(curT);
    //feat = "SF:"+j+suff+":"+curT;
    Add(feat, fv);

	}
    //contains number/UPPERCASE/ hyphen
	if(w0.at(0) <= '9' && w0.at(0) >= '0'){
		feat = "NM:Y"+ curT;
		Add(feat, fv);
	}
	else if(w0.at(0) <= 'Z' && w0.at(0) >= 'A'){
        feat = "UC:Y"+ curT;
		Add(feat, fv);
    }
	else if(w0.find("-") != string::npos){
		feat = "HP:Y"+curT;
		Add(feat, fv);
	}
	else{
		feat = "T0:Y"+curT;
        Add(feat, fv);
	}

}
void POSPipe::AddCoreFeaturesCN(Instance &instance,
                int cur_i,
                string curT,
                FeatureVector &fv) {
    //features
    //form i, i-1,i-2, i+1, i+2
    //tag i, i-1
    //prefix //suffix
    //contains number/UPPERCASE/ hyphen
    vector<WordItem >::iterator it = instance.word_items.begin();
    int len = instance.word_items.size();

	string wtp0, wtp1, wtp2, wtp_1, wtp_2;
	string w0, w1, w2, w_1, w_2;
    w0 = (it+cur_i)->form;
	wtp0 = (it+cur_i)->lemma;

    w_1 = (it+cur_i-1)->form;
	wtp_1 = (it+cur_i-1)->lemma;

    w_2 = "WSTART2";
	wtp_2 = "TSAT2";
    if(cur_i-2>=0){
        w_2 = (it+cur_i-2)->form;
		wtp_2 = (it+cur_i-2)->lemma;
	}

    w1 = "WEND1";
	wtp1 = "TEND1";
    if(cur_i+1 < len){
        w1 = (it+cur_i+1)->form;
        wtp1 = (it+cur_i+1)->lemma;
	}
    w2 = "WEND2";
	wtp2 = "TEND2";
    if(cur_i+2 < len){
        w2 = (it+cur_i+2)->form;
        wtp2 = (it+cur_i+2)->lemma;
	}

    string feat;
	feat.reserve(100);
	//curT
	//feat = "CT:"+curT;
	feat = "";
	feat.append("CT:").append(curT);
	Add(feat, fv);
    //form i, i-1,i-2, i+1, i+2
    //feat = "W0:"+w0+":"+curT;
	feat = "";
    feat.append("W0:").append(w0).append(":").append(curT);
    Add(feat, fv);
	feat = "";
    feat.append("W_1:").append(w_1).append(":").append(curT);
    //feat = "W_1:"+w_1+":"+curT;
    Add(feat, fv);
	feat = "";
    feat.append("W_2:").append(w_2).append(":").append(curT);
    //feat = "W_2:"+w_2+":"+curT;
    Add(feat, fv);
	feat = "";
    feat.append("W1:").append(w1).append(":").append(curT);
    //feat = "W1:"+w1+":"+curT;
    Add(feat, fv);
	feat = "";
    feat.append("W2:").append(w2).append(":").append(curT);
    //feat = "W2:"+w2+":"+curT;
    Add(feat, fv);
	//bi
	feat = "";
    feat.append("W0:").append(w0).append(":").append(w_1).append(curT);
	//feat = "W0:"+w0+":"+w_1+curT;
    Add(feat, fv);
	feat = "";
    feat.append("W_1:").append(w_1).append(":").append(w_2).append(curT);
    //feat = "W_1:"+w_1+":"+w_2+curT;
    Add(feat, fv);
	feat = "";
    feat.append("W1:").append(w1).append(":").append(w0).append(curT);
    //feat = "W1:"+w1+":"+w0+curT;
    Add(feat, fv);
	feat = "";
    feat.append("W2:").append(w2).append(":").append(w1).append(curT);
    //feat = "W2:"+w2+":"+w1+curT;
    Add(feat, fv);
	
	//Jump
	feat = "";
    feat.append("J1:").append(w_1).append(":").append(w1).append(curT);
    Add(feat, fv);
	//firstchar
	string ch;
	ch = w0.substr(0,2);
	feat = "";
    feat.append("F1C:").append(ch).append(curT);
    Add(feat, fv);
	//firstch type
	string type;
	CNWordType(ch, type);
	feat = "";
    feat.append("F1CT:").append(type).append(curT);
    Add(feat, fv);
	//cerr << w0 << " " << ch << " " << type << endl;
	//last char
	int i = w0.length()-2;
	if(i < 0) i = 0;
	ch = w0.substr(i,2);
    feat = "";
    feat.append("L1C:").append(ch).append(curT);
    Add(feat, fv);
	//len
	feat = "";
	stringstream sj;
    sj << w0.length();
    feat.append("LEN:").append(sj.str()).append(curT);
    Add(feat, fv);

}

void POSPipe::AddTagFeatures(Instance &instance,
                int cur_i,
                string preT,
                string curT,
                FeatureVector &fv) {
	int idx = typepair_list.LookupIndex(preT+" "+curT);
	if(idx < 0)
		return;
	//cn and en use the same setting
	AddTagCoreFeatures(instance, cur_i, preT, curT, fv);
	return;
	if((poptions->debug_level) % 10 == 0)
		AddTagSuzukiFeatures(instance, cur_i, preT, curT,fv);
	if((poptions->debug_level/10) % 10 == 1)
		 AddTagSBFeatures(instance, cur_i, preT, curT,fv);
}
void POSPipe::AddTagSBFeatures(Instance &instance,
                int cur_i,
                string preT,
                string curT,
                FeatureVector &fv) {
    vector<WordItem >::iterator it = instance.word_items.begin();
    int len = instance.word_items.size();
    string feat;
    string w0, w_1;
    w0 = (it+cur_i)->form;
    w_1 = (it+cur_i-1)->form;
    //addtional features from unlabeled data
    string entry = "";
    string val = "ZERO";
    entry.append("T2 ").append(w_1).append(" ").append(w0).append(" ").append(preT).append("-").append(curT);

    unordered_map<string, string>::iterator wit = mydict.wordpos_list.find(entry);

    if(wit != mydict.wordpos_list.end()){
        val = wit->second;
    }
	if(val == "ZERO" && (poptions->debug_level/100000) % 10 == 1)
		return;
	//	cerr << entry << "\t" << val << endl;
    if((poptions->debug_level/10000) % 10 == 1){
    //feat = "SB:"+val;
    feat = "";
    feat.append("TSB:").append(val);
    Add(feat,  fv);
    //cerr << feat << endl;
    feat.append(curT);
    Add(feat,  fv);
    }

}
void POSPipe::AddTagCoreFeatures(Instance &instance,
                int cur_i,
                string preT,
                string curT,
                FeatureVector &fv) {
	string feat;
	feat.reserve(100);
	feat = "";
	feat.append("B2:").append(curT).append(":").append(preT);
	//feat = "B2:" + curT + ":" + preT;
    Add(feat, fv);
}
void POSPipe::AddTagSuzukiFeatures(Instance &instance,
                int cur_i,
                string preT,
                string curT,
                FeatureVector &fv) {
	//add
    vector<WordItem >::iterator it = instance.word_items.begin();
    int len = instance.word_num;

	
    string wtp0, wtp1, wtp2, wtp_1, wtp_2;
    //string w0, w1, w2, w_1, w_2;
    //w0 = (it+cur_i)->form;
    wtp0 = (it+cur_i)->lemma;

    //w_1 = (it+cur_i-1)->form;
    wtp_1 = (it+cur_i-1)->lemma;

    if(cur_i-2>=0){
    //    w_2 = (it+cur_i-2)->form;
        wtp_2 = (it+cur_i-2)->lemma;
    }
	else{
    //w_2 = "WSTART2";
    wtp_2 = "TSAT2";
	}

    if(cur_i+1 < len){
    //    w1 = (it+cur_i+1)->form;
        wtp1 = (it+cur_i+1)->lemma;
    }
	else{
    //w1 = "WEND1";
    wtp1 = "TEND1";
	}
    if(cur_i+2 < len){
    //    w2 = (it+cur_i+2)->form;
        wtp2 = (it+cur_i+2)->lemma;
    }
	else{
    //w2 = "WEND2";
    wtp2 = "TEND2";
	}
	//return;
	//
	//feat = "BT_2:" +curT + ":" + preT+":"+wtp_2;
	string prefix2;
	prefix2.append(curT).append(preT);
	string feat;
	feat.reserve(20);
	feat = prefix2;
    feat.append("BT_2").append(wtp_2);
	Add(feat, fv); 
	
	feat = prefix2;
	feat.append("BT_1").append(wtp_1);
	//feat = "BT_1:" +curT + ":" + preT+":"+wtp_1;
	Add(feat, wtp_2, fv); 

	feat = prefix2;
	feat.append("BT0").append(wtp0);
	//feat = "BT0:" +curT + ":" + preT+":"+wtp0;
	Add(feat, wtp_1,fv); 

	feat = prefix2;
	feat.append("BT1").append(wtp1);
	//feat = "BT1:" +curT + ":" + preT+":"+wtp1;
	Add(feat,wtp0, fv); 

	feat = prefix2;
	feat.append("BT2").append(wtp2);
	//feat = "BT2:" +curT + ":" + preT+":"+wtp2;
	Add(feat,wtp1, fv);
/*
	feat = prefix2;
	feat.append("BT_1:").append(wtp_1).append(wtp_2);
    //feat = "BT_1:" +curT + ":" + preT+":"+wtp_1+wtp_2;
    Add(feat, fv);
	feat = prefix2;
	feat.append("BT0:").append(wtp0).append(wtp_1);
    //feat = "BT0:" +curT + ":" + preT+":"+wtp0+wtp_1;
    Add(feat, fv);
	feat = prefix2;
	feat.append("BT1:").append(wtp1).append(wtp0);
    //feat = "BT1:" +curT + ":" + preT+":"+wtp1+wtp0;
    Add(feat, fv);
	feat = prefix2;
	feat.append("BT2:").append(wtp2).append(wtp1);
    //feat = "BT2:" +curT + ":" + preT+":"+wtp2+wtp1;
    Add(feat, fv); 
*/
}

void POSPipe::CreateTagFeatureVector(Instance &instance, string &tag, FeatureVector &fv) {
    Instance new_inst;
    new_inst.word_items = instance.word_items;
    new_inst.word_num = instance.word_items.size();
	string postag = tag;
	//cerr << "POS:" << postag << endl;
	TypeID2Str(postag);
	//cerr << "POS:" << postag << endl;
    vector<string> tag_items;
    boost::split(tag_items, postag, boost::is_any_of(" "));

    for(int i = 0; i < tag_items.size() && i+1 < new_inst.word_num; i++) {
		new_inst.word_items[i+1].cpostag = tag_items[i];
		new_inst.word_items[i+1].postag = tag_items[i];
    }
    CreateFeatureVector(new_inst, fv);
}

//void POSPipe::FillFeatureVectors(Instance &instance,
//                            vector<FeatureVector> &fvs,  vector<double> &probs,
//                             Parameters &params){
//	FillFeatureVectors(instance, fvs);
//	ScoreFeatureVectors(instance, fvs, probs, params);
//}

void POSPipe::ScoreFeatureVectors(Instance &instance,
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
void POSPipe::FillFeatureVectors(Instance &instance,
                            vector<FeatureVector> &fvs){
}
void POSPipe::FillFeatureVectors(Instance &instance,
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
		for(int wc = 0; wc < types_num; wc++) {
	    for(int w2 = 0; w2 < types_num; w2++) {
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

void POSPipe::WriteInstance(Instance &instance, ofstream &feat_wd){
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
void POSPipe::ReadInstance(ifstream &feat_rd, Instance &instance,
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
double POSPipe::GetNumErrors(Instance &instance, string &tag, string &gold){
    return GetNumErrors2(instance, tag, gold);
}
double POSPipe::GetNumErrors2(Instance &instance, string &tag, string &gold){

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

double POSPipe::TypeID2Str(string &s){
	vector<string> tag_items;
    boost::split(tag_items, s, boost::is_any_of(" "));
	string new_tags = "";
	for(int i = 0; i < tag_items.size(); i++) {
		int idx = atoi(tag_items[i].c_str());
		new_tags += types[idx]+" ";
    }

	s = new_tags.substr(0, new_tags.length()-1);
}
