#include "Pipe.h"
#include "MyTool.h"
#include <stdlib.h>
#include <ctime>
#define NONPUNC "nonpunc"
Pipe::Pipe(){
	types = NULL;
	loss_type = NONPUNC;
	//mydict.Load();
}

Pipe::~Pipe(){
	if(types)
		delete[] types;
}

Pipe::Pipe(SymOptions *options){
	Init(options);
}
void Pipe::Init(SymOptions *options){
	loss_type = options->loss_type;
    poptions = options;
	mydict.Load(options->lang);	
	if(poptions->flag_subtree)
		mydict.LoadDictList(poptions->subtreefile, subtree_list);
	if(poptions->flag_cluster)
		mydict.LoadDictList(poptions->clusterfile, wordclu_list);
	string zerohdfile = "res/"+options->lang + ".hd0";
	mydict.LoadDictList(zerohdfile, wordzerohd_list);
    types = NULL;
	rd_data.SetFormat(poptions->format);
	rd_data.SetType("DP");
}

int Pipe::InitFile (string file){
	return rd_data.InitFile(file);
}
void Pipe::CloseFile(){
	rd_data.CloseFile();
}

void Pipe::NextInstance(Instance &inst) {
	rd_data.GetNext(inst);
	if (inst.word_items.size() == 0) 
		return;
	CreateFeatureVector(inst, inst.act_fv);
	vector<WordItem>::iterator pit = inst.word_items.begin();

	stringstream spans;
	int i = 1;
	pit++;//skip root
	while(pit != inst.word_items.end()){
		spans << (pit)->head << "|" << i << ":" << type_list.LookupIndex((pit)->deprel)<< " ";
		pit++;
		i++; 
	}
	string s = spans.str();
	inst.act_parsetree = s.substr(0, s.length()-1);

	//cerr << inst.act_parsetree << "==" << endl;
}

void Pipe::CreateClusterItems(Instance &instance){
	vector<WordItem>::iterator pit = instance.word_items.begin();
	pit->cF = "CLROOTF";
	pit->c4 = "CLROOT4";
	pit->c6 = "CLROOT6";
	pit->cW = "CLROOTW";
	pit++;//skip root
    while(pit != instance.word_items.end()){
		mydict.GetDictItemVal(wordclu_list, pit->form , pit->cF);
        pit->c4 = pit->cF.substr(0,4);
        pit->c6 = pit->cF.substr(0,6);
		pit->cW = "LFW";//low freq word
		if(mydict.IsInList(mydict.wordfreq_list, pit->form))
			pit->cW = pit->form;
		//cerr << "CLUs:" << pit->cF << "\t" << pit->c4 << "\t" << pit->cW << endl;

        pit++;
    }

}

void Pipe::CreateInstances(string &file,	 string &feat_filename) {

	CreateAlphabet(file);

	ofstream feat_wd;
	feat_wd.open(feat_filename.c_str(), ios::binary);
    if(!feat_wd.is_open()){
        cerr << "Err: opening "<< feat_filename << endl;
        return ;
    }
	
    cerr << "Creating Instance Feat File ... "  << flush;
	time_t start_time;
    start_time = time(NULL);
    rd_data.InitFile(file);

    Instance instance;
    instance_num = 0;
	NextInstance(instance);
    while(instance.word_items.size() != 0) {
        instance_num++;
		if(instance_num % 500 == 0)
			cerr << instance_num << "," << flush;
		WriteInstance(instance, feat_wd);
        NextInstance(instance);
    }

    rd_data.CloseFile();
	
	feat_wd.close();
	cerr << instance_num << endl;
    time_t end_time;
    end_time = time(NULL);		
    cerr << "Have "<< instance_num << " instances. Time:" << (end_time-start_time) << endl;
    cerr << "Creating Instance FeatFile Done." << endl;
		
}


void Pipe::CreateAlphabet(string &file) {

	cerr << "Creating Alphabet ... " << flush;
	rd_data.InitFile(file);

	Instance instance;
	instance_num = 0;
	rd_data.GetNext(instance);
	while(instance.word_items.size() != 0) {
		instance_num++;
	    vector<WordItem>::iterator pit = instance.word_items.begin();
	 	while(pit != instance.word_items.end()){
			type_list.LookupIndex((pit)->deprel);
			pit++;
		}
		FeatureVector fv;
	    CreateFeatureVector(instance, fv);
		instance.Clear();	
	    rd_data.GetNext(instance);
		//cerr << instance_num <<" " << flush;
	}

	CloseAlphabets();
	rd_data.CloseFile();

	cerr << "Have "<< instance_num << " instances. Creating Alphabet Done." << flush << endl;
}
	
void Pipe::CloseAlphabets() {
	feat_list.StopGrowth();
	type_list.StopGrowth();
	pospair_type_list.StopGrowth();

	
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
	cerr << " Feature Num:" << feat_list.mytable.size() << endl; 
}


    // add with default 1.0
int Pipe::Add(string &feat, FeatureVector &fv) {
	int num = feat_list.LookupIndex(feat);
	//int num = 1;
	//cerr << feat << "=" << num << endl;
	if(num >= 0)
	    fv.Add(num, 1.0);
	return num;
}
int Pipe::Add(string &feat, string &suffix, FeatureVector &fv) {
	AddTwo(feat, suffix, fv);
}
int Pipe::AddTwo(string &feat, string &suffix, FeatureVector &fv) {
	int num = Add(feat, fv);
	if(num >= 0){
		feat.append( "*").append(suffix);
		num = Add(feat, fv);
	}
    return num;
}

	
void Pipe::CreateFeatureVector(Instance &instance, FeatureVector &fv) {
	//for cluster-based features
	if(poptions->flag_cluster){
		CreateClusterItems(instance);
	}

	int inst_length = instance.word_items.size();
	vector<WordItem>::iterator it = instance.word_items.begin();
	vector<WordItem>::iterator itb = it;
	for(int i = 0; i < inst_length; i++, ++it) {
		int hd = (it)->head;
	    if(hd == -1){
			continue;
		}
	    int small = i < hd ? i : hd;
	    int large = i > hd ? i : hd;
	    bool attR = i < hd ? false : true;
		string label = (it)->deprel;

	    AddCoreFeatures(instance,small,large,attR,fv);
	    if(poptions->labeled_parsing) {
		
		string pospair_type = it->postag+" "+(itb+hd)->postag+" "+label;
		pospair_type_list.LookupIndex(pospair_type);	
		AddLabeledFeatures(instance,small, large, label ,attR,fv);
		//AddLabeledFeatures(instance,i,label ,attR,true,fv);
		//AddLabeledFeatures(instance,hd,label,attR,false,fv);
	    }
	}
	AddExtendedFeatures(instance, fv);
	return;
}

void Pipe::CreateTagFeatureVector(Instance &instance, string &tag, FeatureVector &fv) {
	Instance new_inst;
	new_inst.word_items = instance.word_items;
	new_inst.word_num = instance.word_num;
	vector<string> tag_items;
    boost::split(tag_items, tag, boost::is_any_of(" "));
    for(int i = 0; i < tag_items.size() && i+1 < new_inst.word_num; i++) {
        vector<string> p1;
        boost::split(p1, tag_items.at(i), boost::is_any_of("|:"));
		new_inst.word_items[i+1].head = atoi(p1.at(0).c_str());
		new_inst.word_items[i+1].deprel = types[atoi(p1.at(2).c_str())];
    }
	CreateFeatureVector(new_inst, fv);	
}

void Pipe::ParseTree2Array(string tag, vector<int> &tag_heads){
	tag_heads.clear();
	tag_heads.push_back(-1);//for root
    vector<string> tag_items;
    boost::split(tag_items, tag, boost::is_any_of(" "));
    for(int i = 0; i < tag_items.size(); i++) {
        vector<string> p1;
        boost::split(p1, tag_items.at(i), boost::is_any_of(":"));
        tag_heads.push_back( atoi(p1.at(0).c_str()));
    }
}

void Pipe::TagFeatureVector(Instance &instance, vector<int> &tag_heads, vector<FeatureVector> &fvs,  vector<double> &probs,
                            vector<FeatureVector> &nt_fvs,vector<double> &pnt_probs,
                            FVLink &fvlink) {
	cerr<< "ERROR: Should not be here! Pipe::TagFeatureVector " << endl;
    int inst_length = instance.word_num;

    for(int i = 0; i < inst_length; i++) {
        int hd = tag_heads[i];
        if(hd == -1){
            continue;
        }
        int small = i < hd ? i : hd;
        int large = i > hd ? i : hd;
        bool attR = i < hd ? false : true;
		int ph = attR?0:1;
		int vect_idx = GetFVIdx(inst_length, small, large);
		fvlink.Add(&fvs[vect_idx+ph]);	
        //AddCoreFeatures(instance,small,large,attR,fv);
        //if(labeled) {
        //addLabeledFeatures(instance,i,labs[i],attR,true,fv);
        //addLabeledFeatures(instance,heads[i],labs[i],attR,false,fv);
        //}
    }
    return;
}

void Pipe::AddCoreFeatures(Instance &instance,
				int small,
				int large,
				bool attR,
				FeatureVector &fv) {
    vector<WordItem >::iterator it = instance.word_items.begin();
	int idx_hd = small;
	int idx_ch = large;
	if (!attR) {
	    idx_hd = large;
	    idx_ch = small;
	}

	string whd = (it+idx_hd)->form;
	if(!IsGoodHead(whd))
		return;//dont generate the features for some zerohd words

	string att = attR?"R":"L";


	int dist = large-small;
	string dist_type = "0";
	if (dist > 10)
	    dist_type = "10";
	else if (dist > 5)
	    dist_type = "5";
	else{
		std::stringstream ss;
  		ss << (dist-1);
	    dist_type = ss.str();
	}
		
	string att_dist = "";
	att_dist.append("&").append(att).append("&").append(dist_type);
	string prefix = "POS";
	
	AddLinearFeatures(prefix, instance, small, large, att_dist, fv );
	
	
	prefix = "HC";
	AddTwoObsFeatures(prefix, instance, idx_hd, idx_ch, att_dist, fv);
    
	if(poptions->flag_coarsepos){
		prefix = "CPOS";
        AddLinearFeatures(prefix,instance, small, large, att_dist, fv);
		prefix = "CHC";
		AddTwoObsFeatures(prefix, instance, idx_hd, idx_ch, att_dist, fv);
	}

	////////////////////////////////////////////////////////////////////
	//Other features
	if(poptions->flag_subtree){
		prefix = "ST2";
		AddSubtreeFeatures(prefix,instance, small, large, attR, fv);
	}	
	
	if(poptions->flag_cluster){
		bool wcopy = true;
		bool pcopy = true;
		//c4 +cF
		prefix = "CL4F";
		wcopy = true;//only one copy of cF
		pcopy = true;
		AddTwoObsFeatures(prefix, instance, (it+small)->cF, (it+large)->cF, 
											(it+small)->c4, (it+large)->c4, wcopy, pcopy, att_dist, fv);
		//c6 + cF
		prefix = "CL6F";
		wcopy = false;
		pcopy = true;
		AddTwoObsFeatures(prefix, instance, (it+small)->cF, (it+large)->cF, 
											(it+small)->c6, (it+large)->c6, wcopy, pcopy, att_dist, fv);
		//c4 +ct-postag
		prefix = "CL4T";
        wcopy = false;
		pcopy = false;
        AddTwoObsFeatures(prefix, instance, (it+small)->postag, (it+large)->postag,
                                            (it+small)->c4, (it+large)->c4, wcopy, pcopy, att_dist, fv);
		//c6 +ct
		prefix = "CL6T";
        wcopy = false;
		pcopy = false;
        AddTwoObsFeatures(prefix, instance, (it+small)->postag, (it+large)->postag,
                                            (it+small)->c6, (it+large)->c6, wcopy, pcopy, att_dist, fv);
		//c4 +cW
		prefix = "CL4W";
        wcopy = true;
		pcopy = false;
        AddTwoObsFeatures(prefix, instance, (it+small)->cW, (it+large)->cW,
                                            (it+small)->c4, (it+large)->c4, wcopy, pcopy, att_dist, fv);
		//c6 +cW
		prefix = "CL6W";
        wcopy = false;
		pcopy = false;
        AddTwoObsFeatures(prefix, instance, (it+small)->cW, (it+large)->cW,
                                            (it+small)->c6, (it+large)->c6, wcopy, pcopy, att_dist, fv);
		//dont want
		//prefix = "CLPlus";//if want to add, should modify 
        //AddLinearFeatures(prefix,instance, small, large, att_dist, fv);
	}

}



void Pipe::AddSubtreeFeatures(string &prefix, Instance &instance,
                             int small, int large, bool attR, FeatureVector &fv){
	//if(poptions->debug_level % 10 != 1)
	//	return;

	vector<WordItem >::iterator it = instance.word_items.begin();
    string sw0 = (it+small)->form;
    string lw0 = (it+large)->form;
	string sp0 = (it+small)->postag;
	string lp0 = (it+large)->postag;

	string sw_1 = small > 1 ? (it+small-1)->form:"START";
    string lw_1 = large -1 > small ? (it+large-1)->form:"SMALL";
    string sp_1 = small > 1 ? (it+small-1)->postag:"STARTP";;
    string lp_1 = large -1 > small ? (it+large-1)->postag:"SMALLP";

	string sw1 = small+1 < large ? (it+small+1)->form:"LARGE";
    string lw1 = large+1 < instance.word_num ? (it+large+1)->form:"END";
    string sp1 = small+1 < large ? (it+small+1)->postag:"LARGEP";
    string lp1 = large+1 < instance.word_num ? (it+large+1)->postag:"ENDP";

	string st = "BI ";
	string newprefix = "";
	string val;
	
	//if(attR)
	{
		prefix += "R";
		
		st = "BI ";
		st.append(sw0).append(":0 ").append(lw0).append(":1");
		mydict.GetDictItemVal(subtree_list, st, val);
		newprefix = prefix + "0";
		AddSubtreeFeaturesItems(newprefix, attR, val, sw0, lw0, sp0, lp0, fv);

		st = "BI ";
		st.append(sw_1).append(":0 ").append(lw0).append(":1");
		mydict.GetDictItemVal(subtree_list, st, val);
		newprefix = prefix + "_1";
    	AddSubtreeFeaturesItems(newprefix, attR, val, sw_1, lw0, sp_1, lp0, fv);

		st = "BI ";
		st.append(sw1).append(":0 ").append(lw0).append(":1");
		mydict.GetDictItemVal(subtree_list, st, val);
        newprefix = prefix + "1";
        AddSubtreeFeaturesItems(newprefix, attR, val, sw1, lw0, sp1, lp0, fv);
		
		//if(poptions->debug_level == 2){
		{
		st = "BI ";
		st.append(sw_1).append(":2 ").append(lw0).append(":0");
        mydict.GetDictItemVal(subtree_list, st, val);
        newprefix = prefix + "_V1";
        AddSubtreeFeaturesItems(newprefix, attR, val, sw_1, lw0, sp_1, lp0, fv);

		st = "BI ";
        st.append(sw1).append(":2 ").append(lw0).append(":0");
        mydict.GetDictItemVal(subtree_list, st, val);
        newprefix = prefix + "V1";
        AddSubtreeFeaturesItems(newprefix, attR, val, sw1, lw0, sp1, lp0, fv);
		}
	}
	//else
	{
		prefix += "L";
		st = "BI ";
		st.append(sw0).append(":2 ").append(lw0).append(":0");
		mydict.GetDictItemVal(subtree_list, st, val);
        newprefix = prefix + "0";
        AddSubtreeFeaturesItems(newprefix, attR, val, sw0, lw0, sp0, lp0, fv);

		st = "BI ";
		st.append(sw0).append(":2 ").append(lw_1).append(":0");
		mydict.GetDictItemVal(subtree_list, st, val);
        newprefix = prefix + "_1";
        AddSubtreeFeaturesItems(newprefix, attR, val, sw0, lw_1, sp0, lp_1, fv);

		st = "BI ";
		st.append(sw0).append(":2 ").append(lw1).append(":0");
		mydict.GetDictItemVal(subtree_list, st, val);
        newprefix = prefix + "1";
        AddSubtreeFeaturesItems(newprefix, attR, val, sw0, lw1, sp0, lp1, fv);
		//if(poptions->debug_level == 2){
		{
		st = "BI ";
		st.append(sw0).append(":0 ").append(lw_1).append(":1");
        mydict.GetDictItemVal(subtree_list, st, val);
        newprefix = prefix + "_V1";
        AddSubtreeFeaturesItems(newprefix, attR, val, sw0, lw_1, sp0, lp_1, fv);
		//cerr << newprefix << st << "\t" << val << endl;

		st = "BI ";
        st.append(sw0).append(":0 ").append(lw1).append(":1");
        mydict.GetDictItemVal(subtree_list, st, val);
        newprefix = prefix + "V1";
        AddSubtreeFeaturesItems(newprefix, attR, val, sw0, lw1, sp0, lp1, fv);
		}
	}


}
void Pipe::AddSubtreeFeaturesItems(string &prefix, bool attR, string &val,
                                   string &w1, string &p1,
                                    FeatureVector &fv){
	string feat;
    feat = "";
    feat.append(prefix).append(val);
    Add(feat, fv);
    //only consider the word/pos of head side
    if(mydict.IsInList(mydict.wordfreq_list, w1)){
    feat = "";
    feat.append(prefix).append(val).append(":").append(w1);
    Add(feat, fv);
    }
    feat = "";
    feat.append(prefix).append(val).append(":P:").append(p1);
    Add(feat, fv);

}
void Pipe::AddSubtreeFeaturesItems(string &prefix,bool attR, string &val, 
                                   string &w1, string &w2, string &p1, string &p2, 
									FeatureVector &fv){
	//cerr << prefix << attR << val << endl;
	//if(val == "O")
	//	return;

	string feat;
    feat = "";
    feat.append(prefix).append(val);
    Add(feat, fv);
	//only consider the word/pos of head side
	if(attR){
    if(mydict.IsInList(mydict.wordfreq_list, w1)){
    feat = "";
    feat.append(prefix).append(val).append(":S:").append(w1);
    Add(feat, fv);
    }
    feat = "";
    feat.append(prefix).append(val).append(":SP:").append(p1);
    Add(feat, fv);
    }	
	else{
	if(mydict.IsInList(mydict.wordfreq_list, w2)){
    feat = "";
    feat.append(prefix).append(val).append(":L:").append(w2);
    Add(feat, fv);
    }

    feat = "";
    feat.append(prefix).append(val).append(":LP:").append(p2);
    Add(feat, fv);
	}
}

void Pipe::AddLinearFeatures(string prefix, Instance &instance, 
                             int idx1, int idx2, string &att_dist, FeatureVector &fv){
	vector<WordItem >::iterator it = instance.word_items.begin();
	//POS
	string p1 = (it+idx1)->postag;
	string p2 = (it+idx2)->postag;
	string p1L = idx1 > 0? (it+idx1-1)->postag: "STR";
	string p2R = idx2 < (instance.word_items.size()-1)? (it+idx2+1)->postag: "END";
	string p1R = idx1 < idx2-1? (it+idx1+1)->postag: "MID";
	string p2L = idx2 > idx1+1? (it+idx2-1)->postag: "MID";
	//CPOS
	if(prefix == "CPOS"){
	p1 = (it+idx1)->cpostag;
    p2 = (it+idx2)->cpostag;
    p1L = idx1 > 0? (it+idx1-1)->cpostag: "STRCP";
    p2R = idx2 < (instance.word_items.size()-1)? (it+idx2+1)->cpostag: "ENDCP";
    p1R = idx1 < idx2-1? (it+idx1+1)->cpostag: "MIDCP";
    p2L = idx2 > idx1+1? (it+idx2-1)->cpostag: "MIDCP";
	}
	
	// feature posR posMid posL
	string feat = "";
	feat.append(prefix).append("PC").append(p1).append("&").append(p2);

	for(int i = idx1+1; i < idx2; i++) {
	    string allPOS = feat;
		if(prefix == "CPOS")
			allPOS.append( "&").append((it+i)->cpostag);
		else
			allPOS.append( "&").append((it+i)->postag);
		AddTwo(allPOS, att_dist, fv);
	}
	
	AddCorePosFeatures(prefix+"PT", p1L, p1, p1R, 
				p2L, p2, p2R, att_dist, fv);
	
}

void Pipe::AddCorePosFeatures(string prefix, string p1L, string p1, string p1R,
                             string p2L, string p2, string p2R, string &att_dist, FeatureVector &fv){

	string feat = "";
	feat.append(prefix).append("=").append(p1L).append(" ").append(p1).append(" ").append(p2).append("*").append(att_dist);
	Add(feat,fv);
	feat = "";
	feat.append(prefix).append("1=").append(p1L).append(" ").append(p1).append(" ").append(p2);
	if(Add(feat, fv) >= 0){
	feat.append(" ").append(p2R);
	Add(feat, att_dist, fv);
	}

	feat = "";
    feat.append(prefix).append("2=").append(p1L).append(" ").append(p2).append(" ").append(p2R);
	Add(feat, att_dist, fv);

	feat = "";
    feat.append(prefix).append("3=").append(p1L).append(" ").append(p1).append(" ").append(p2R);
	Add(feat, att_dist, fv);
	
	feat = "";
    feat.append(prefix).append("4=").append(p1).append(" ").append(p2).append(" ").append(p2R);
	Add(feat, att_dist, fv);

	/////////////////////////////////////////////////////////////
	prefix.append("A");

	// feature posL posL+1 posR-1 posR
	feat = "";
    feat.append(prefix).append("1=").append(p1).append(" ").append(p1R).append(" ").append(p2L);
	feat.append("*").append(att_dist);
	Add(feat, fv);
	feat = "";
    feat.append(prefix).append("1=").append(p1).append(" ").append(p1R).append(" ").append(p2L);
	if(Add(feat, fv) >= 0){
	feat.append(" ").append(p2);
	Add(feat, att_dist,fv);
	}

	feat = "";
    feat.append(prefix).append("2=").append(p1).append(" ").append(p1R).append(" ").append(p2);
	Add(feat, att_dist, fv);

	feat = "";
    feat.append(prefix).append("3=").append(p1).append(" ").append(p2L).append(" ").append(p2);
	Add(feat, att_dist, fv);

	feat = "";
    feat.append(prefix).append("4=").append(p1R).append(" ").append(p2L).append(" ").append(p2);
	Add(feat, att_dist, fv);

	///////////////////////////////////////////////////////////////
	prefix.append("B");

	//// feature posL-1 posL posR-1 posR
	feat = "";
    feat.append(prefix).append("1=").append(p1L).append(" ").append(p1).append(" ").append(p2L).append(" ").append(p2);
	Add(feat, att_dist, fv);

	//// feature posL posL+1 posR posR+1
	feat = "";
    feat.append(prefix).append("2=").append(p1).append(" ").append(p1R).append(" ").append(p2).append(" ").append(p2R);
	Add(feat, att_dist, fv);
}
void Pipe::AddTwoObsFeatures(string prefix, Instance &instance, int wid1, int wid2,
                             string &att_dist, FeatureVector &fv){
    vector<WordItem >::iterator it = instance.word_items.begin();
    string p1 = (it+wid1)->postag;
    string p2 = (it+wid2)->postag;
	
    string w1 = (it+wid1)->form;
    string w2 = (it+wid2)->form;
 
	if(prefix.find("CHC") == 0){
		p1 = (it+wid1)->cpostag;
		p2 = (it+wid2)->cpostag;
	}

	string feat = "";
	if(prefix.find("HC") == 0){//only one copy
	feat = "";
	feat.append(prefix).append("2FF1=").append(w1);
	Add(feat, att_dist, fv);
	feat = "";
    feat.append(prefix).append("2FF2=").append(w1).append(" ").append(w2);
	Add(feat, att_dist, fv);
	feat = "";
    feat.append(prefix).append("2FF8=").append(w2);
	Add(feat, att_dist, fv);
	}
	feat = "";
    feat.append(prefix).append("2FF1=").append(w1).append(" ").append(p1);
	Add(feat, att_dist, fv);

	feat = "";
    feat.append(prefix).append("2FF1=").append(w1).append(" ").append(p1).append(" ").append(p2);
	Add(feat, att_dist, fv);

	feat = "";
    feat.append(prefix).append("2FF1=").append(w1).append(" ").append(p1).append(" ").append(p2).append(" ").append(w2);
	Add(feat, att_dist, fv);
	

	feat = "";
    feat.append(prefix).append("2FF3=").append(w1).append(" ").append(p2);
	Add(feat, att_dist, fv);

	feat = "";
    feat.append(prefix).append("2FF4=").append(p1).append(" ").append(w2);
	Add(feat, att_dist, fv);

	feat = "";
    feat.append(prefix).append("2FF4=").append(p1).append(" ").append(w2).append(" ").append(p2);
	Add(feat, att_dist, fv);

	feat = "";
    feat.append(prefix).append("2FF5=").append(p1).append(" ").append(p2);
	Add(feat, att_dist, fv);

	feat = "";
    feat.append(prefix).append("2FF6=").append(w2).append(" ").append(p2);
	Add(feat, att_dist, fv);

	feat = "";
    feat.append(prefix).append("2FF7=").append(p1);
	Add(feat, att_dist, fv);
	
	
	feat = "";
    feat.append(prefix).append("2FF9=").append(p2);
	Add(feat, att_dist, fv);
	

}
//w1 can be word form, full bit-string for clusters
//p1 can be postag, 4/6 bit-string
//wcopy 
void Pipe::AddTwoObsFeatures(string prefix, Instance &instance, string &w1, string &w2, 
							string &p1, string &p2, bool wcopy, bool pcopy,
                             string &att_dist, FeatureVector &fv){

	string feat = "";
	if(wcopy){//only one copy
	feat = "";
	feat.append(prefix).append("2FF1=").append(w1);
	Add(feat, att_dist, fv);
	feat = "";
    feat.append(prefix).append("2FF2=").append(w1).append(" ").append(w2);
	Add(feat, att_dist, fv);
	feat = "";
    feat.append(prefix).append("2FF8=").append(w2);
	Add(feat, att_dist, fv);
	}
	if(pcopy){//only one copy
	feat = "";
    feat.append(prefix).append("2FF7=").append(p1);
	Add(feat, att_dist, fv);
	
	feat = "";
    feat.append(prefix).append("2FF9=").append(p2);
	Add(feat, att_dist, fv);
	feat = "";
    feat.append(prefix).append("2FF5=").append(p1).append(" ").append(p2);
	Add(feat, att_dist, fv);
	
	}
	feat = "";
    feat.append(prefix).append("2FF1=").append(w1).append(" ").append(p1);
	Add(feat, att_dist, fv);

	feat = "";
    feat.append(prefix).append("2FF1=").append(w1).append(" ").append(p1).append(" ").append(p2);
	Add(feat, att_dist, fv);

	feat = "";
    feat.append(prefix).append("2FF1=").append(w1).append(" ").append(p1).append(" ").append(p2).append(" ").append(w2);
	Add(feat, att_dist, fv);
	

	feat = "";
    feat.append(prefix).append("2FF3=").append(w1).append(" ").append(p2);
	Add(feat, att_dist, fv);

	feat = "";
    feat.append(prefix).append("2FF4=").append(p1).append(" ").append(w2);
	Add(feat, att_dist, fv);

	feat = "";
    feat.append(prefix).append("2FF4=").append(p1).append(" ").append(w2).append(" ").append(p2);
	Add(feat, att_dist, fv);


	feat = "";
    feat.append(prefix).append("2FF6=").append(w2).append(" ").append(p2);
	Add(feat, att_dist, fv);

	

}
//
void Pipe::AddLabeledFeatures(Instance &instance,
                int small,
                int large,
				string &label,
                bool attR,
                FeatureVector &fv) {

	if(small == large)//
		return;
	if(small == 0 && !attR)
		return;
    vector<WordItem >::iterator it = instance.word_items.begin();
    int idx_hd = small;
    int idx_ch = large;
    if (!attR) {
        idx_hd = large;
        idx_ch = small;
    }
	string whd = (it+idx_hd)->form;
	if(!IsGoodHead( whd))
        return;//dont generate the features for some zerohd words

	string pospair_type = (it+idx_ch)->postag+" "+(it+idx_hd)->postag+" "+label;
	int ret = pospair_type_list.LookupIndex(pospair_type);
	if(ret == -1)
		return;//dont need
    string att = attR?"R":"L";


    int dist = large-small;
    string dist_type = "0";
    if (dist > 10)
        dist_type = "10";
    else if (dist > 5)
        dist_type = "5";
    else{
        std::stringstream ss;
        ss << (dist-1);
        dist_type = ss.str();
    }

    string att_dist = "";
    att_dist.append("&").append(att).append("&").append(dist_type);
    string prefix = "POS"+label;
	//too many
    //AddLinearFeatures(prefix, instance, small, large, att_dist, fv );


    prefix = "HC"+label;
    AddTwoObsFeatures(prefix, instance, idx_hd, idx_ch, att_dist, fv);

	if(poptions->flag_subtree){
        prefix = "ST2"+label;
        //AddSubtreeFeatures(prefix,instance, small, large, attR, fv);
    }
	AddLabeledFeatures1(instance, idx_hd, label, attR, false, fv);
	AddLabeledFeatures1(instance, idx_ch, label, attR, true, fv);
}

void Pipe::AddLabeledFeatures1(Instance &instance,
				   int wid,
				   string &label,
				   bool attR,
				   bool isChild,
				   FeatureVector &fv) {
	if(!poptions->labeled_parsing)
		return;
	vector<WordItem >::iterator it = instance.word_items.begin();
	    
	string att = "";
	if(attR)
	    att = "RA";
	else
	    att = "LA";
	if(isChild)
		att += "CH";
	else
		att += "HD";
    string p0 = (it+wid)->postag;
    string w0 = (it+wid)->form;
	
	string p_1 = wid-1>=0?(it+wid-1)->postag:"STRP";		
	string p1 = wid+1 < instance.word_num?(it+wid+1)->postag:"ENDP";

	string feat = "";
	feat = "LB1=";
	feat.append(label).append(att);
	Add(feat, fv);
	//for(int i = 0; i < 2; i++) 
	{
	    //string suff = i < 1 ? "&"+att : "";
		string suff = "&"+att;
	    suff += "&"+label;
		feat = "LBw=";
        feat.append(w0).append(suff);
        //Add(feat, fv);
		feat = "LB0=";
		feat.append(p0).append(suff);
		//Add(feat, w0, fv);
		feat = "LB1=";
		feat.append(p1).append(" ").append(p0).append(suff);
		Add(feat, fv);
		feat = "LB_1=";
        feat.append(p_1).append(" ").append(p0).append(suff);
        Add(feat, fv);
		feat = "LB1_1=";
        feat.append(p_1).append(" ").append(p0).append(p1).append(" ").append(suff);
        Add(feat, fv);

	}
}



void Pipe::FillFeatureVectors(Instance &instance,
                            vector<FeatureVector> &fvs,  vector<double> &probs,
                            vector<FeatureVector> &nt_fvs,vector<double> &nt_probs,
                             Parameters &params){
	int instance_length = instance.word_items.size();
	// Get production crap.		
	fvs.reserve(instance_length*instance_length*2);
	probs.reserve(instance_length*instance_length*2);
	nt_probs.reserve(instance_length*types_num*instance_length);
	nt_fvs.reserve(instance_length*types_num*instance_length);
	int idx = 0;
	int fv_maxsize = 10;
	FeatureVector fv;
	for(int w1 = 0; w1 < instance_length; w1++) {
	    for(int w2 = w1+1; w2 < instance_length; w2++) {
		for(int ph = 0; ph < 2; ph++) {
		    bool attR = ph == 0 ? true : false;
			fv.Clear();
			fv.Reserve(fv_maxsize);
		    AddCoreFeatures(instance,w1,w2,attR, fv);
			//cerr << w1 <<" " << w2 << " "<< ph <<" idx:" <<idx++ <<endl;
		    double prob = fv.GetScore(params);
			//fvs.push_back(fv);
		    probs.push_back(prob);
			if(fv_maxsize < fv.GetFeatSize())
				fv_maxsize = fv.GetFeatSize();
		}
	    }
	}
	fv_maxsize = 10;
	if(poptions->labeled_parsing) {
		for(int w1 = 0; w1 < instance_length; w1++) {
        for(int w2 = 0; w2 < instance_length; w2++) {
		for(int t = 0; t < types_num; t++) {
		string type = types[t];
        {//w1-hd w2-ch
            bool attR = w2 > w1 ? true : false;
			int small = w1 > w2? w2:w1;
			int large = w1 > w2? w1:w2;
            fv.Clear();
            fv.Reserve(fv_maxsize);
            AddLabeledFeatures(instance,small,large,type, attR, fv);
            double prob = fv.GetScore(params);
            nt_probs.push_back(prob);
            if(fv_maxsize < fv.GetFeatSize())
                fv_maxsize = fv.GetFeatSize();
        }
        }
		}
    	}//end for
		
/*	    for(int w1 = 0; w1 < instance_length; w1++) {
		for(int t = 0; t < types_num; t++) {
		    string type = types[t];
		    for(int ph = 0; ph < 2; ph++) {

			bool attR = ph == 0 ? true : false;
			for(int ch = 0; ch < 2; ch++) {

			    bool isChild = (ch == 0) ? true : false;
				fv.Clear();
				fv.Reserve(fv_maxsize);
			    AddLabeledFeatures(instance,w1,
					       type,attR,isChild, fv);
			    
			    double nt_prob = fv.GetScore(params);
				nt_probs.push_back(nt_prob);
			    
			}
		    }
		}
	    }
*/	}		

}

void Pipe::WriteInstance(Instance &instance, ofstream &feat_wd){
	int instance_length = instance.word_items.size();
    // Get production crap.a
	//boost::archive::binary_oarchive oa(feat_wd);
    int idx = 0;
	vector<int> keys;
	int fv_maxsize = 10;
    FeatureVector fv;
	//instance.act_fv.GetKeys(keys);
    for(int w1 = 0; w1 < instance_length; w1++) {
        for(int w2 = w1+1; w2 < instance_length; w2++) {
        for(int ph = 0; ph < 2; ph++) {
            bool attR = ph == 0 ? true : false;

			fv.Clear();
			fv.Reserve(fv_maxsize);
            AddCoreFeatures(instance,w1,w2,attR, fv);
			fv.GetKeys(keys);
			if(fv_maxsize < fv.GetFeatSize())
                fv_maxsize = fv.GetFeatSize();
            //cerr << w1 <<" " << w2 << " "<< ph <<" idx:" <<idx++ <<endl;
            //fvs.push_back(fv);
	//		oa << fv;
        }
        }
    }
	fv_maxsize = 10;
    if(poptions->labeled_parsing) {
		for(int w1 = 0; w1 < instance_length; w1++) {
        for(int w2 = 0; w2 < instance_length; w2++) {
        for(int t = 0; t < types_num; t++) {
        string type = types[t];
        {//w1-hd w2-ch
            bool attR = w2 > w1 ? true : false;
            int small = w1 > w2? w2:w1;
            int large = w1 > w2? w1:w2;
            fv.Clear();
            fv.Reserve(fv_maxsize);
            AddLabeledFeatures(instance,small,large,type, attR, fv);
			fv.GetKeys(keys);
            if(fv_maxsize < fv.GetFeatSize())
                fv_maxsize = fv.GetFeatSize();
        }
        }
        }
        }//end for
/*
        for(int w1 = 0; w1 < instance_length; w1++) {
        for(int t = 0; t < types_num; t++) {
            string type = types[t];
            for(int ph = 0; ph < 2; ph++) {

            bool attR = ph == 0 ? true : false;
            for(int ch = 0; ch < 2; ch++) {

                bool isChild = (ch == 0) ? true : false;
                fv.Clear();
                fv.Reserve(fv_maxsize);
                AddLabeledFeatures(instance,w1,
                           type,attR,isChild, fv);

				fv.GetKeys(keys);
				if(fv_maxsize < fv.GetFeatSize())
                	fv_maxsize = fv.GetFeatSize();
            }
            }
        }
        }
 */   }

	//oa << instance_length;
	//oa << instance.act_parsetree;
	MyTool::VectorWrite(feat_wd, keys);
	//int keys_len = keys.size();
	//oa << keys_len;
	//oa << keys;
	WriteExtendedFeatures(instance, feat_wd);
}
void Pipe::ReadInstance(ifstream &feat_rd, Instance &instance,
                            vector<FeatureVector> &fvs,  vector<double> &probs,
                            vector<FeatureVector> &nt_fvs,vector<double> &nt_probs,
                             Parameters &params){
    int instance_length = instance.word_num;
    //boost::archive::binary_iarchive ia(feat_rd);
	//ia >> instance_length;
	//ia >> instance.act_parsetree;
	//instance.word_num = instance_length;
	fvs.reserve(instance_length*instance_length*2);
	probs.reserve(instance_length*instance_length*2);
	nt_probs.reserve(instance_length*types_num*instance_length);
	nt_fvs.reserve(instance_length*types_num*instance_length);
    int idx = 0;
	vector<int> keys;
	MyTool::VectorRead(feat_rd, keys);

//	int keys_len = 0;
//	ia >> keys_len;
//	keys.reserve(keys_len);
//	ia >> keys;
	int keys_idx = 0;
	int keys_size = 0;
	if(false){
	keys_size = keys.at(keys_idx++);
    if(keys_size > 0){
        instance.act_fv.Reserve(keys_size);
        instance.act_fv.Add(keys, keys_idx, keys_size);
        keys_idx += keys_size;
    }
	}

	int fv_maxsize = 10;
    FeatureVector fv;
    for(int w1 = 0; w1 < instance_length; w1++) {
        for(int w2 = w1+1; w2 < instance_length; w2++) {
        for(int ph = 0; ph < 2; ph++) {
            bool attR = ph == 0 ? true : false;
			fv.Clear();
            fv.Reserve(fv_maxsize);			

			keys_size = keys.at(keys_idx++);
			//cerr << keys_idx << " " << keys_size << endl; 
			if(keys_size > 0){
				fv.Reserve(keys_size);
				fv.Add(keys, keys_idx, keys_size);
				keys_idx += keys_size;
			}
			//fvs.push_back(fv);
			double prob = fv.GetScore(params);
            probs.push_back(prob);
			if(fv_maxsize < fv.GetFeatSize())
                fv_maxsize = fv.GetFeatSize();
			idx++;
        }
        }
    }
    fv_maxsize = 10;
    if(poptions->labeled_parsing) {
		for(int w1 = 0; w1 < instance_length; w1++) {
        for(int w2 = 0; w2 < instance_length; w2++) {
        for(int t = 0; t < types_num; t++) {
        string type = types[t];
        {//w1-hd w2-ch
            bool attR = w2 > w1 ? true : false;
            int small = w1 > w2? w2:w1;
            int large = w1 > w2? w1:w2;
			fv.Clear();
            fv.Reserve(fv_maxsize);
            keys_size = keys.at(keys_idx++);
            if(keys_size > 0){
                 fv.Reserve(keys_size);
                 fv.Add(keys, keys_idx, keys_size);
                 keys_idx += keys_size;
            }
            //fvs.push_back(fv);
            double prob = fv.GetScore(params);
            nt_probs.push_back(prob);
            if(fv_maxsize < fv.GetFeatSize())
                fv_maxsize = fv.GetFeatSize();
        }
        }
        }
        }//end for

/*        for(int w1 = 0; w1 < instance_length; w1++) {
        for(int t = 0; t < types_num; t++) {
            string type = types[t];
            for(int ph = 0; ph < 2; ph++) {

            bool attR = ph == 0 ? true : false;
            for(int ch = 0; ch < 2; ch++) {

                bool isChild = (ch == 0) ? true : false;
                fv.Clear();
                fv.Reserve(fv_maxsize);
				keys_size = keys.at(keys_idx++);
            	if(keys_size > 0){
                	fv.Reserve(keys_size);
                	fv.Add(keys, keys_idx, keys_size);
                	keys_idx += keys_size;
            	}
            	//fvs.push_back(fv);
            	double prob = fv.GetScore(params);
				nt_probs.push_back(prob);
                if(fv_maxsize < fv.GetFeatSize())
                    fv_maxsize = fv.GetFeatSize();
            }
            }
        }
        }
*/    }

}

void Pipe::TypeID2Str(string &s){
    vector<string> tag_items;
    boost::split(tag_items, s, boost::is_any_of(" "));
    string new_tags = "";
    for(int i = 0; i < tag_items.size(); i++) {
		vector<string> p1;
        boost::split(p1, tag_items.at(i), boost::is_any_of("|:"));
		
        int idx = atoi(p1[2].c_str());
		
        new_tags += p1[0]+"|"+p1[1]+":"+types[idx]+" ";
    }

    s = new_tags.substr(0, new_tags.length()-1);
}

bool Pipe::IsGoodHead(string &w){
	//return true;
	bool val = mydict.IsInList(wordzerohd_list, w);
	//cerr << w << "\t"<< val << endl;
	return !val;
}

double Pipe::GetNumErrors(Instance &instance, string &tag, string &gold){
    if(loss_type == NONPUNC)
        return GetNumErrorsDEPNonpunc(instance, tag, gold);
    return GetNumErrorsDEP(instance, tag, gold);
}
double Pipe::GetNumErrorsDEPNonpunc(Instance &instance, string &tag, string &gold){
    double err;
    vector<string> tag_items;
    vector<string> gold_items;
    boost::split(tag_items, tag, boost::is_any_of(" "));
    boost::split(gold_items, gold, boost::is_any_of(" "));

    string pun_en = "[,:.'`]+";
    string pun_cn = "PU";
    int correct = 0;
    int correctLB = 0;
    int num = 0;
    for(int i = 0; i < tag_items.size(); i++) {
        if(instance.word_items[i+1].postag == pun_cn ||
            pun_en.find(instance.word_items[i+1].postag) != string::npos){
            continue;
        }
        vector<string> p1;
        vector<string> p2;
        boost::split(p1, tag_items.at(i), boost::is_any_of(":"));
        boost::split(p2, gold_items.at(i), boost::is_any_of(":"));
        num++;
        if(p1.at(0) == p2.at(0))
            correct++;
		if(p1.at(2) == p2.at(2))
            correctLB++;
    }
    err = num -  correct;
	if(poptions->labeled_parsing){
        err += (num -  correctLB);
    }

    if(false){
    cerr << "===" << endl;    cerr << gold << endl;    cerr << tag << endl;    cerr << err << endl;
    }
  return err;
}
double Pipe::GetNumErrorsDEP(Instance &instance, string &tag, string &gold){

    double err;
    vector<string> tag_items;
    vector<string> gold_items;
    boost::split(tag_items, tag, boost::is_any_of(" "));
    boost::split(gold_items, gold, boost::is_any_of(" "));


    int correct = 0;
    int correctLB = 0;

    for(int i = 0; i < tag_items.size(); i++) {
        vector<string> p1;
        vector<string> p2;
        boost::split(p1, tag_items.at(i), boost::is_any_of("|:"));
        boost::split(p2, gold_items.at(i), boost::is_any_of("|:"));
        if(p1.at(0) == p2.at(0))
            correct++;
        if(p1.at(2) == p2.at(2))
            correctLB++;
		
    }
    err = tag_items.size() -  correct;
	if(poptions->labeled_parsing){
		err += (tag_items.size() -  correctLB);
	}
/*  cerr << "===" << endl;
    cerr << gold << endl;
    cerr << tag << endl;
    cerr << err << endl;
*/  return err;
}

int Pipe::GetFVIdx(int instance_length, int s, int t){
	int vect_idx = 2*((s*instance_length - (s+1)*s/2) + (t-s)-1);

	return vect_idx;
}
int Pipe::GetFVIdx2(int instance_length, int s, int t){
	int idx = 0;
	    for(int w1 = 0; w1 < instance_length; w1++) {
        for(int w2 = w1+1; w2 < instance_length; w2++) {
		if(s == w1 && t == w2)
			return idx;
        for(int ph = 0; ph < 2; ph++) {
			idx++;
 		}}}
	return idx;
}
/*
int Pipe::GetLBFVIdx(int instance_length, int wid, int t){
    int vect_idx = 2*2*(wid*types_num+t);

    return vect_idx;//attR/isChild 0 true/true 
}
int Pipe::GetLBFVIdx2(int instance_length, int wid, int tid){
	int idx = 0;
	for(int w1 = 0; w1 < instance_length; w1++) {
        for(int t = 0; t < types_num; t++) {
			//cerr << w1 << " " << t << endl;
			if(w1 == wid && t ==tid)
				return idx;
			
            for(int ph = 0; ph < 2; ph++) {

            bool attR = ph == 0 ? true : false;
            for(int ch = 0; ch < 2; ch++) {

                bool isChild = (ch == 0) ? true : false;
				idx++;
			}}
		}
	}
	return idx;
}        
*/
int Pipe::GetLBFVIdx(int instance_length, int par, int ch , int t){
    int vect_idx = (par*instance_length+ch)*types_num +t;

    return vect_idx;//attR/isChild 0 true/true
}
int Pipe::GetLBFVIdx2(int instance_length, int par, int ch , int tid){
    int idx = 0;
    for(int w1 = 0; w1 < instance_length; w1++) {
    for(int w2 = 0; w2 < instance_length; w2++) {
        for(int t = 0; t < types_num; t++) {
            if(w1 == par && w2 == ch && t ==tid)
                return idx;

			idx++;
        }
        }
    }
    return idx;
}

void Pipe::TestIdx(){
	int idx1, idx2;
	types_num = 50;
	for(int t = 20; t < 30; t++){
	idx1 = GetLBFVIdx(20, 10, 5, t);
	idx2 = GetLBFVIdx2(20, 10, 5, t);
	cerr << idx1 << " vs "<< idx2 << endl;
	continue;
    idx1 = GetFVIdx(100, 10, t);
    idx2 = GetFVIdx2(100, 10, t);
    cerr << idx1 << " vs "<< idx2 << endl;
    }

}
//*
