#include "SegPipe.h"
#include "MyTool.h"
#include <stdlib.h>
#define NONPUNC "nonpunc"
/*SegPipe::SegPipe(){
	types = NULL;
	loss_type = NONPUNC;
	//mydict.Load();
	cerr << "Error: Should not call this SegPipe()" << endl;
}
*/
SegPipe::~SegPipe(){
	//if(types)
	//	delete[] types;
}

SegPipe::SegPipe(SymOptions *options){
//	cerr << "Error: Should not call this SegPipe(opt)" << endl;
	loss_type = NONPUNC;
	poptions = options;
	types = NULL;		
	mydict.Load(options->lang);
	rd_data.SetFormat(poptions->format);
	rd_data.SetType("SEG");
}

int SegPipe::InitFile (string file){
    return rd_data.InitFile(file);
}


void SegPipe::PreprocessInstance(Instance &instance){
	//Set properties of char
	vector<WordItem >::iterator it = instance.word_items.begin();
    int len = instance.word_items.size();
	it++;//skip first
	for(int i = 1; i < len;i++, it++){
		CodeMap(instance.word_items[i].form, instance.word_items[i].chcode);
	}
}

void SegPipe::CodeMap(string &w, string &code){
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
	else if(val == "YMD")
		code = "3";
	else 
		code = "0";//others
	return;
}
	
void SegPipe::AddCoreFeatures(Instance &instance,
                int cur_i,
                string curT,
                FeatureVector &fv) {
	//word == ch
    vector<WordItem >::iterator it = instance.word_items.begin();
    int len = instance.word_items.size();

	string chcode0, chcode1, chcode2, chcode_1, chcode_2;
	string w0, w1, w2, w_1, w_2;
    w0 = (it+cur_i)->form;
	chcode0 = (it+cur_i)->chcode;

    w_1 = (it+cur_i-1)->form;
	chcode_1 = (it+cur_i-1)->chcode;

    w_2 = "WSTART2";
	chcode_2 = "TSAT2";
    if(cur_i-2>=0){
        w_2 = (it+cur_i-2)->form;
		chcode_2 = (it+cur_i-2)->chcode;
	}

    w1 = "WEND1";
	chcode1 = "TEND1";
    if(cur_i+1 < len){
        w1 = (it+cur_i+1)->form;
        chcode1 = (it+cur_i+1)->chcode;
	}
    w2 = "WEND2";
	chcode2 = "TEND2";
    if(cur_i+2 < len){
        w2 = (it+cur_i+2)->form;
        chcode2 = (it+cur_i+2)->chcode;
	}
	//
	string isPU = "F";
	string val;
	mydict.GetDictItemVal(mydict.PU_list, w0, val);
    if(val == "PU")
		isPU = "T";
	string code5 = chcode_2+chcode_1+chcode0+chcode1+chcode2;
	string code3 = chcode_1+chcode0+chcode1;

	string feat;
	feat.reserve(100);
    //form i, i-1,i-2, i+1, i+2
    //feat = "W0:"+w0+":"+curT;
	feat = "";
    feat.append("W0:").append(w0).append(":").append(curT);
    Add(feat, fv);
	feat = "";
    feat.append("W_1:").append(w_1).append(":").append(curT);
    Add(feat, fv);
	feat = "";
    feat.append("W1:").append(w1).append(":").append(curT);
    Add(feat, fv);
	//bi
	feat = "";
    feat.append("W0:").append(w0).append(":").append(w_1).append(curT);
    Add(feat, fv);
	feat = "";
    feat.append("W1:").append(w1).append(":").append(w0).append(curT);
    Add(feat, fv);
	//jump
	feat = "";
    feat.append("J1:").append(w_1).append(":").append(w1).append(curT);
    Add(feat, fv);
	//PU
	feat = "";
    feat.append("PU0:").append(isPU).append(curT);
    Add(feat, fv);
	//code itself
	feat = "";
    feat.append("CD0:").append(chcode0).append(curT);
    //Add(feat, fv);
	//code3
	feat = "";
    feat.append("CD3:").append(code3).append(curT);
    Add(feat, fv);
}

void SegPipe::AddTagFeatures(Instance &instance,
                int cur_i,
                string preT,
                string curT,
                FeatureVector &fv) {
	string feat;
	feat.reserve(10);
	feat = "";
	feat.append("B2:").append(curT).append(":").append(preT);
	//feat = "B2:" + curT + ":" + preT;
    Add(feat, fv);
}
bool SegPipe::SeqTagCheck(string &s){
	vector<string> tag_items;
    boost::split(tag_items, s, boost::is_any_of(" "));
    string new_tags = "";
    for(int i = 0; i < tag_items.size(); i++) {
		if(i == 0 && (tag_items[i] != "S" && tag_items[i] != "B")){
			cerr << "Err: " << i << " " << tag_items[i]<< endl;
			return false;
		}
		if(i == (tag_items.size()-2) && (tag_items[i] != "S" && tag_items[i] != "E")){
            cerr << "Err: " << i << " " << tag_items[i]<< endl;
			return false;
		}
		
		if(i >= tag_items.size()-2)
			continue;
		
		if(tag_items[i] == "S" && (tag_items[i+1] != "S" && tag_items[i+1] != "B")){
			cerr << "Err: " << i << " " << tag_items[i] << "/" << tag_items[i+1]<< endl;
            return false;
		}
		 if(tag_items[i] == "B" && (tag_items[i+1] != "E" && tag_items[i+1] != "B2")){
            cerr << "Err: " << i << " " << tag_items[i] << "/" <<tag_items[i+1]<< endl;
            return false;
        }
		 if(tag_items[i] == "B2" && (tag_items[i+1] != "B3" && tag_items[i+1] != "E")){
            cerr << "Err: " << i << " " << tag_items[i] << "/" <<tag_items[i+1]<< endl;
            return false;
        }
		 if(tag_items[i] == "B3" && (tag_items[i+1] != "M" && tag_items[i+1] != "E")){
            cerr << "Err: " << i << " " << tag_items[i] << "/" <<tag_items[i+1]<< endl;
            return false;
        }
		 if(tag_items[i] == "M" && (tag_items[i+1] != "E" && tag_items[i+1] != "M")){
            cerr << "Err: " << i << " " << tag_items[i] << "/" <<tag_items[i+1]<< endl;
            return false;
        }
		 if(tag_items[i] == "E" && (tag_items[i+1] != "S" && tag_items[i+1] != "B")){
            cerr << "Err: " << i << " " << tag_items[i] << "/" <<tag_items[i+1]<< endl;
            return false;
        }	
		//or others
    }
	return true;
}
