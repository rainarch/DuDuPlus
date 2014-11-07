#include "PipeO2.h"
#include "MyTool.h"

#include <stdlib.h>
#include <ctime>
void PipeO2::AddExtendedFeatures(Instance &instance,FeatureVector &fv){
	
	//cout << "PipeO2-AddExtendedFeatures" << endl;
	int inst_length = instance.word_items.size();
	vector<WordItem>::iterator it = instance.word_items.begin();
	vector<WordItem>::iterator itb = instance.word_items.begin();

	for(int i = 0; i < inst_length; i++, ++it) {
		int hd = (it)->head;
        if(hd == -1 && i != 0){
            continue;
        }
	
	    // right 
	    int prev = i;
		vector<WordItem>::iterator itj = it+1;
	    for(int j = i+1; j < inst_length; j++, ++itj) {
		int hdj = (itj)->head;
		if(hdj == i) {
		    AddTripFeatures(instance,i,prev,j,fv);
		    AddSiblingFeatures(instance,prev,j,prev==i,fv);
		    prev = j;
		}
	    }
		//left	
	    prev = i;
		itj = it-1;
	    for(int j = i-1; j >= 0; j--, --itj) {
		int hdj = (itj)->head;
        if(hdj == i) {
		    AddTripFeatures(instance,i,prev,j,fv);
		    AddSiblingFeatures(instance,prev,j,prev==i,fv);
		    prev = j;
		}
	    }
	}
	//gc
	it = instance.word_items.begin();
    for(int i = 0; i < inst_length; i++, ++it) {
        int hd = (it)->head;
        if(hd == -1 && i != 0){
            continue;
        }

        // right children
		vector<WordItem>::iterator itj = it+1;
        for(int j = i+1; j < inst_length; j++, ++itj) {
		int hdj = (itj)->head;
        if(hdj < i) break;
        if(hdj == i) {
            bool flag = false;
            //cmi
			vector<WordItem>::iterator itk = it+1;
            for(int k = i+1; k < j;k++, ++itk){
				int hdk = itk->head;
                if(hdk == j){
                    AddGCFeatures(instance,i,j,k,fv);
                    flag = true;
                    break;
                }
            }
            if(flag == false) AddGCFeatures(instance,i,j,-1,fv);
            //cmo
            flag = false;
			itk = itb+inst_length-1;
            for(int k = inst_length-1; k > j;k--, --itk){
				int hdk = itk->head;
                if(hdk == j){
                    AddGCFeatures(instance,i,j,k,fv);
                    flag = true;
                    break;
                }
            }
            if(flag == false) AddGCFeatures(instance,i,j,-2,fv);
        }
        }
		//left
		itj = it-1;
        for(int j = i-1; j >= 0; j--,--itj) {
		int hdj = itj->head;
        if(hdj > i) break;
        if(hdj == i) {
            bool flag = false;
            //cmi
			vector<WordItem>::iterator itk = it-1;
            for(int k = i-1; k > j;k--, --itk){
				int hdk = itk->head;
                if(hdk == j){
                    AddGCFeatures(instance,i,j,k,fv);
                    flag = true;
                    break;
                }
            }
            if(flag == false) AddGCFeatures(instance,i,j,-1,fv);

            flag = false;
            //cmo
			itk = itb;
            for(int k = 0; k < j; k++,++itk){
				int hdk = itk->head;
                if(hdk == j){
                    AddGCFeatures(instance,i,j,k,fv);
                    flag = true;
                    break;
                }
            }
            if(flag == false) AddGCFeatures(instance,i,j,-2,fv);
        }
        }
    }
	AddHigherFeatures(instance, fv);
}

void PipeO2::AddSiblingFeatures(Instance &instance,
					  int ch1, int ch2,
					  bool isST,
					  FeatureVector &fv) {
	//return;
	if(ch1 == ch2) return;
	//return;
	int word_num = instance.word_num;
	vector<WordItem >::iterator it = instance.word_items.begin();
    string dir = ch1 > ch2? "RA" : "LA";
    //ch2 ch1 par or par ch1 ch2
    string ch1_pos = (isST)? "STPOS":(it+ch1)->postag;
    string ch2_pos = (it+ch2)->postag;

    string ch1_cpos = (isST)? "STCPOS":(it+ch1)->cpostag;
    string ch2_cpos = (it+ch2)->cpostag;

	string ch1_word = (isST)? "STWD":(it+ch1)->form;
    string ch2_word = (it+ch2)->form;

	string ch1_pos1 = (ch1+1 < word_num)? (it+ch1+1)->postag:"ENDP";
    string ch1_pos_1 = (ch1-1 >= 0)? (it+ch1-1)->postag:"STARTP";

	string ch2_pos1 = (ch2+1 < word_num)? (it+ch2+1)->postag:"ENDP";
    string ch2_pos_1 = (ch2-1 >= 0)? (it+ch2-1)->postag:"STARTP";

	string ch1_c4 = (isST)? "STC4":(it+ch1)->c4;
    string ch2_c4 = (it+ch2)->c4;
	string ch1_c6 = (isST)? "STC6":(it+ch1)->c6;
    string ch2_c6 = (it+ch2)->c6;
	string ch1_cF = (isST)? "STCF":(it+ch1)->cF;
    string ch2_cF = (it+ch2)->cF;
	string ch1_cW = (isST)? "STCW":(it+ch1)->cW;
    string ch2_cW = (it+ch2)->cW;

	string feat;

	int dist = ch1 > ch2? ch1 - ch2: ch2-ch1;
	
	string distBool = "0";
	if(dist > 1)
	    distBool = "1";
	if(dist > 2)
	    distBool = "2";
	if(dist > 3)
	    distBool = "3";
	if(dist > 4)
	    distBool = "4";
	if(dist > 5)
	    distBool = "5";
	if(dist > 10)
	    distBool = "10";
	feat = "";
    feat.append("SIB=").append(distBool);
    Add(feat, dir, fv);
	feat = "";
    feat.append("SIB=DIST=").append(distBool).append(ch1_pos).append("_").append(ch2_pos);
    Add(feat, dir, fv);
	
	feat = "";
	feat.append("ACH_PAIR=").append(ch1_pos).append("_").append(ch2_pos);
    Add(feat, dir, fv);
    feat = "";
    feat.append("ACH_WPAIR=").append(ch1_word).append("_").append(ch2_word);
    Add(feat, dir, fv);
    feat = "";
    feat.append("ACH_WPAIRA=").append(ch1_word).append("_").append(ch2_pos);
    Add(feat, dir, fv);
    feat = "";
    feat.append("ACH_WPAIRB=").append(ch1_pos).append("_").append(ch2_word);
    Add(feat, dir, fv);


	if(poptions->flag_coarsepos){
	feat = "";
    feat.append("SIB=CDIST=").append(distBool).append(ch1_cpos).append("_").append(ch2_cpos).append(dir);
    Add(feat, fv);
	feat = "";
    feat.append("ACH_CPAIR=").append(ch1_cpos).append("_").append(ch2_cpos).append(dir);
    Add(feat,  fv);
    feat = "";
    feat.append("ACH_CWPAIRA=").append(ch1_word).append("_").append(ch2_cpos).append(dir);
    Add(feat,  fv);
    feat = "";
    feat.append("ACH_CWPAIRB=").append(ch1_cpos).append("_").append(ch2_word).append(dir);
    Add(feat,  fv);	
	}
	if(poptions->flag_cluster){
	feat = "";
    feat.append("SIBCL4=DIST=").append(distBool).append(ch1_c4).append("_").append(ch2_c4).append(dir);
    Add(feat, fv);
    feat = "";
    feat.append("ACHCL4_PAIR=").append(ch1_c4).append("_").append(ch2_c4).append(dir);
    Add(feat,  fv);
    feat = "";
    feat.append("ACHCL4_FPAIRA=").append(ch1_cF).append("_").append(ch2_c4).append(dir);
    Add(feat,  fv);
    feat = "";
    feat.append("ACHCL4_FPAIRB=").append(ch1_c4).append("_").append(ch2_cF).append(dir);
    Add(feat,  fv);
	feat = "";
    feat.append("ACHCL4_WPAIRA=").append(ch1_cW).append("_").append(ch2_c4).append(dir);
    Add(feat,  fv);
    feat = "";
    feat.append("ACHCL4_WPAIRB=").append(ch1_c4).append("_").append(ch2_cW).append(dir);
    Add(feat,  fv);
	feat = "";
    feat.append("SIBCL4=TDIST=").append(distBool).append(ch1_pos).append("_").append(ch2_c4).append(dir);
    Add(feat, fv);
    feat = "";
    feat.append("ACHCL4_TPAIR=").append(ch1_pos).append("_").append(ch2_c4).append(dir);
    Add(feat,  fv);
	feat = "";
    feat.append("SIBCL4=TDIST2=").append(distBool).append(ch1_c4).append("_").append(ch2_pos).append(dir);
    Add(feat, fv);
    feat = "";
    feat.append("ACHCL4_TPAIR2=").append(ch1_c4).append("_").append(ch2_pos).append(dir);
    Add(feat,  fv);
	//c6
	feat = "";
    feat.append("SIBCL6=DIST=").append(distBool).append(ch1_c6).append("_").append(ch2_c6).append(dir);
    Add(feat, fv);
    feat = "";
    feat.append("ACHCL6_PAIR=").append(ch1_c6).append("_").append(ch2_c6).append(dir);
    Add(feat,  fv);
    feat = "";
    feat.append("ACHCL6_FPAIRA=").append(ch1_cF).append("_").append(ch2_c6).append(dir);
    Add(feat,  fv);
    feat = "";
    feat.append("ACHCL6_FPAIRB=").append(ch1_c6).append("_").append(ch2_cF).append(dir);
    Add(feat,  fv);
    feat = "";
    feat.append("ACHCL6_WPAIRA=").append(ch1_cW).append("_").append(ch2_c6).append(dir);
    Add(feat,  fv);
    feat = "";
    feat.append("ACHCL6_WPAIRB=").append(ch1_c6).append("_").append(ch2_cW).append(dir);
    Add(feat,  fv);
	feat = "";
    feat.append("SIBCL6=TDIST=").append(distBool).append(ch1_pos).append("_").append(ch2_c6).append(dir);
    Add(feat, fv);
    feat = "";
    feat.append("ACHCL6_TPAIR=").append(ch1_pos).append("_").append(ch2_c6).append(dir);
    Add(feat,  fv);
    feat = "";
    feat.append("SIBCL6=TDIST2=").append(distBool).append(ch1_c6).append("_").append(ch2_pos).append(dir);
    Add(feat, fv);
    feat = "";
    feat.append("ACHCL6_TPAIR2=").append(ch1_c6).append("_").append(ch2_pos).append(dir);
    Add(feat,  fv);
	}
	//surrounding
	feat = "";
    feat.append("SIBLIN=s1=").append(ch1_pos).append("_").append(ch1_pos1).append("_").append(ch2_pos).append(dir);
    Add(feat,  fv);
	feat = "";
    feat.append("SIBLIN=s_1=").append(ch1_pos).append("_").append(ch1_pos_1).append("_").append(ch2_pos).append(dir);
    Add(feat,  fv);
	feat = "";
	feat.append("SIBLIN=d1=").append(ch1_pos).append("_").append(ch2_pos1).append("_").append(ch2_pos).append(dir);
    Add(feat,  fv);
	feat = "";
    feat.append("SIBLIN=d_1=").append(ch1_pos).append("_").append(ch2_pos_1).append("_").append(ch2_pos).append(dir);
    Add(feat,  fv);

	feat = "";
    feat.append("SIBLIN=s1d_1=").append(ch1_pos).append("_").append(ch1_pos1).append("_").append(ch2_pos_1).append("_").append(ch2_pos).append(dir);
    Add(feat,  fv);
	feat = "";
    feat.append("SIBLIN=s_1d_1=").append(ch1_pos).append("_").append(ch1_pos_1).append("_").append(ch2_pos_1).append("_").append(ch2_pos).append(dir);
    Add(feat,  fv);
	feat = "";
    feat.append("SIBLIN=s1d1=").append(ch1_pos).append("_").append(ch1_pos1).append("_").append(ch2_pos1).append("_").append(ch2_pos).append(dir);
    Add(feat,  fv);
    feat = "";
    feat.append("SIBLIN=s_1d1=").append(ch1_pos).append("_").append(ch1_pos_1).append("_").append(ch2_pos1).append("_").append(ch2_pos).append(dir);
    Add(feat, fv);
}
void PipeO2::AddTripFeatures(Instance &instance,
				       int par,
				       int ch1, int ch2,
				       FeatureVector &fv) {


	//return;
	if(!((ch1 >= par && ch2 > ch1) || (par >= ch1 && ch1 > ch2)))
		return;


	int word_num = instance.word_num;
	vector<WordItem >::iterator it = instance.word_items.begin();
	string whd = (it+par)->form;
    if(!IsGoodHead( whd))
        return;//dont generate the features for some zerohd words

	if(poptions->flag_subtree)
		AddTripSubtreeFeatures(instance, par, ch1, ch2, fv);
	//return;
	string dir = par > ch2? "RA" : "LA";
	//ch2 ch1 par or par ch1 ch2
	string par_pos = (it+par)->postag;
	string ch1_pos = (ch1 == par)? "STPOS":(it+ch1)->postag;
	string ch2_pos = (it+ch2)->postag;

	string par_cpos = (it+par)->cpostag;
    string ch1_cpos = (ch1 == par)? "STCPOS":(it+ch1)->cpostag;
    string ch2_cpos = (it+ch2)->cpostag;

	string par_pos1 = par+1 < word_num? (it+par+1)->postag: "ENDP";
	string par_pos_1 = par-1 >= 0? (it+par-1)->postag: "STARTP";

	string ch1_pos1 = (ch1+1 < word_num)? (it+ch1+1)->postag:"ENDP";
	string ch1_pos_1 = (ch1-1 >= 0)? (it+ch1-1)->postag:"STARTP";

	bool isST = (ch1==par)?true:false;
	string par_c4 = (it+par)->c4;
	string par_c6 = (it+par)->c6;
	string par_cF = (it+par)->cF;
	string par_cW = (it+par)->cW;
    string ch1_c4 = (isST)? "STC4":(it+ch1)->c4;
    string ch2_c4 = (it+ch2)->c4;
    string ch1_c6 = (isST)? "STC6":(it+ch1)->c6;
    string ch2_c6 = (it+ch2)->c6;
    string ch1_cF = (isST)? "STCF":(it+ch1)->cF;
    string ch2_cF = (it+ch2)->cF;
    string ch1_cW = (isST)? "STCW":(it+ch1)->cW;
    string ch2_cW = (it+ch2)->cW;


	string feat;
	feat  = "TRIP=HC12=";
	feat.append(par_pos).append("_").append(ch1_pos).append("_").append(ch2_pos);	
	Add(feat, dir,fv);
	//
	feat = "TRIP=HC1=";
	feat.append(par_pos).append("_").append(ch1_pos);
	//Add(feat, dir, fv);

	if(poptions->flag_coarsepos){
    feat  = "TRIP=CHC12=";
    feat.append(par_cpos).append("_").append(ch1_cpos).append("_").append(ch2_cpos).append(dir);
    Add(feat, fv);
    //
    feat = "TRIP=CHC1=";
    feat.append(par_cpos).append("_").append(ch1_cpos).append(dir);
    //Add(feat,  fv);
	}
	if(poptions->flag_cluster){
	feat  = "TRIPC4=HC12=";
    feat.append(par_c4).append("_").append(ch1_c4).append("_").append(ch2_c4);
    Add(feat, dir,fv);
	feat  = "TRIPC4=HC12A=";
    feat.append(par_c4).append("_").append(ch1_pos).append("_").append(ch2_pos);
    Add(feat, dir,fv);
	feat  = "TRIPC4=HC12B=";
    feat.append(par_pos).append("_").append(ch1_c4).append("_").append(ch2_pos);
    Add(feat, dir,fv);
	feat  = "TRIPC4=HC12C=";
    feat.append(par_pos).append("_").append(ch1_pos).append("_").append(ch2_c4);
    Add(feat, dir,fv);
	//c6
	feat  = "TRIPC4=HC12=";
    feat.append(par_c6).append("_").append(ch1_c6).append("_").append(ch2_c6);
    Add(feat, dir,fv);
    feat  = "TRIPC6=HC12A=";
    feat.append(par_c6).append("_").append(ch1_pos).append("_").append(ch2_pos);
    Add(feat, dir,fv);
    feat  = "TRIPC6=HC12B=";
    feat.append(par_pos).append("_").append(ch1_c6).append("_").append(ch2_pos);
    Add(feat, dir,fv);
    feat  = "TRIPC6=HC12C=";
    feat.append(par_pos).append("_").append(ch1_pos).append("_").append(ch2_c6);
    Add(feat, dir,fv);
	}
	//Surrounding POS
	feat = "TRIPLIN=s1=";
    feat.append(par_pos).append("_").append(ch1_pos).append("_").append(ch1_pos1).append(dir);
    Add(feat, fv);
	feat = "TRIPLIN=s_1=";
    feat.append(par_pos).append("_").append(ch1_pos).append("_").append(ch1_pos_1).append(dir);
    Add(feat,  fv);
	feat = "TRIPLIN=h1=";
    feat.append(par_pos).append("_").append(ch1_pos).append("_").append(par_pos1).append(dir);
    Add(feat,  fv);
	feat = "TRIPLIN=h_1=";
    feat.append(par_pos).append("_").append(ch1_pos).append("_").append(par_pos_1).append(dir);
    Add(feat,  fv);

	feat = "TRIPLIN=s1h_1=";
    feat.append(par_pos_1).append("_").append(ch1_pos).append("_").append(ch1_pos1).append(dir);
    Add(feat,  fv);
    feat = "TRIPLIN=s_1h_1=";
    feat.append(par_pos_1).append("_").append(ch1_pos).append("_").append(ch1_pos_1).append(dir);
    Add(feat,  fv);
    feat = "TRIPLIN=41=";
    feat.append(par_pos).append("_").append(par_pos_1).append("_").append(ch1_pos).append("_").append(ch1_pos1).append(dir);
    Add(feat,  fv);
    feat = "TRIPLIN=42=";
    feat.append(par_pos).append("_").append(par_pos1).append("_").append(ch1_pos).append("_").append(ch1_pos_1).append(dir);
    Add(feat,  fv);

}

void PipeO2::AddTripSubtreeFeatures(Instance &instance,
                       int par,
                       int ch1, int ch2,
                       FeatureVector &fv) {
	//if((poptions->debug_level/100) % 10 != 1)
	//    return;
    vector<WordItem >::iterator it = instance.word_items.begin();
    string dir = par > ch2? "RA" : "LA";
	bool attR = par > ch2?true:false;
    //ch2 ch1 par or par ch1 ch2
    string par_pos = (it+par)->postag;
    string ch1_pos = (ch1 == par)? "STPOS":(it+ch1)->postag;
    string ch2_pos = (it+ch2)->postag;

	string par_word = (it+par)->form;
    string ch1_word = (ch1 == par)? "STWORD":(it+ch1)->form;
    string ch2_word = (it+ch2)->form;
	string prefix = "STSB";
	string newprefix = "";
	string st;
	string val;
	
	if(par == ch1){
		if(par < ch2){
			st = "";
			newprefix = prefix + "=R";
			st.append("TRI1 ").append(par_word).append(":0 ").append(ch1_word).append(":1 ").append(ch2_word).append(":1");
		}
		else{
			st = "";
            newprefix = prefix + "=L";
            st.append("TRI1 ").append(par_word).append(":3 ").append(ch1_word).append(":3 ").append(ch2_word).append(":0");
		}
	}
	else{
		//par != ch1
		if(par < ch2){
			st = "";
            newprefix = prefix + "!R";
            st.append("TRI ").append(par_word).append(":0 ").append(ch1_word).append(":1 ").append(ch2_word).append(":1");
		}
		else{
			st = "";
            newprefix = prefix + "!L";
            st.append("TRI ").append(par_word).append(":3 ").append(ch1_word).append(":3 ").append(ch2_word).append(":0");
		}
	}
	//if((poptions->debug_level/1000) % 10 == 1)
	{
       
	mydict.GetDictItemVal(subtree_list, st, val);
    AddSubtreeFeaturesItems(newprefix, attR, val, par_word, par_pos, fv);
	//cout << newprefix << " "<< st << "\t" << val << endl;
	}
	
	//sibling
	if(ch1 < ch2){
		st = "";
        newprefix = prefix + "SR";
        st.append("SIB SIB:0 ").append(ch1_word).append(":1 ").append(ch2_word).append(":1");
	}
	else{
		st = "";
        newprefix = prefix + "SL";
        st.append("SIB ").append(ch1_word).append(":3 ").append(ch2_word).append(":3 SIB:0");
	}

	//if((poptions->debug_level/10000) % 10 == 1)
	{
	string feat;
	mydict.GetDictItemVal(subtree_list, st, val);
    feat = "";
    feat.append(newprefix).append(val);
    Add(feat, fv);
	}
	//cout << newprefix << " "<< st << "\t" << val << endl;

	return;
}

void PipeO2::AddGCFeatures(Instance &instance,
                      int par, int ch,
                      int gc,
                      FeatureVector &fv) {
	//return;
	if(ch == gc) return;
	if(gc >= 0 && !((par > ch && par > gc) || (par < ch && par < gc)))
		return;

	int word_num = instance.word_num;
	vector<WordItem >::iterator it = instance.word_items.begin();
	string whd = (it+par)->form;
    if(!IsGoodHead( whd))
        return;//dont generate the features for some zerohd words
	if(gc > 0){//ch has a child
		string wch = (it+ch)->form;
		if(!IsGoodHead( wch))
			return;
	} 

	if(poptions->flag_subtree)
		AddGCSubtreeFeatures(instance, par, ch, gc, fv);
    string dir = par > ch? "RA" : "LA";
    string dirpd = par > ch? "RA" : "LA";
    string dirgc = gc < 0 ? "NOGC" : ch> gc? "RA": "LA";

	string type;
	type = gc == -1?"-1":"-2";
    dir.append( "_").append(dirgc);
	if(gc < 0){
		dir.append(type);
	}

    string par_pos = (it+par)->postag;
    string ch_pos =  (it+ch)->postag;
    string gc_pos = (gc < 0)?"STPOS": (it+gc)->postag;
	string par_cpos = (it+par)->cpostag;
    string ch_cpos =  (it+ch)->cpostag;
    string gc_cpos = (gc < 0)?"STCPOS": (it+gc)->cpostag;

    string par_word = (it+par)->form;
    string ch_word =  (it+ch)->form;
    string gc_word = (gc < 0)?"STWORD": (it+gc)->form;
	//surrounding
	string par_pos1 = par+1 < word_num?(it+par+1)->postag:"ENDP";
	string par_pos_1 = par-1 >= 0 ?(it+par-1)->postag:"STARTP";
    string ch_pos1 = ch+1 < word_num? (it+ch+1)->postag:"ENDP";
    string ch_pos_1 = ch-1 >= 0 ? (it+ch-1)->postag:"STARTP";
    string gc_pos1 = (gc < 0)?"STPOS1": (gc+1 < word_num? (it+gc+1)->postag:"ENDP");
    string gc_pos_1 = (gc < 0)?"STPOS_1": (gc-1 >=0? (it+gc-1)->postag:"STARTP");
	
	string par_c4 = (it+par)->c4;
    string ch_c4 =  (it+ch)->c4;
    string gc_c4 = (gc < 0)?"STC4": (it+gc)->c4;
	string par_c6 = (it+par)->c6;
    string ch_c6 =  (it+ch)->c6;
    string gc_c6 = (gc < 0)?"STC6": (it+gc)->c6;
	string par_cF = (it+par)->cF;
    string ch_cF =  (it+ch)->cF;
    string gc_cF = (gc < 0)?"STCF": (it+gc)->cF;
	string par_cW = (it+par)->cW;
    string ch_cW =  (it+ch)->cW;
    string gc_cW = (gc < 0)?"STCW": (it+gc)->cW;

	string feat;
	
	feat = "";
	feat.append("GC_POS_pcg=").append(par_pos).append("_").append(ch_pos).append("_").append(gc_pos).append("_").append(dir);
	Add(feat,  fv);
		
	feat = "";
    feat.append("GC_POS_pg=").append(par_pos).append("_").append(gc_pos).append("_").append(dir);
    Add(feat,fv);
	feat = "";
    feat.append("GC_POS_cg=").append(ch_pos).append("_").append(gc_pos).append("_").append(dir);
    Add(feat,fv);
    //
	feat = "";
    feat.append("GC_WORD_pg=").append(par_word).append("_").append(gc_word).append("_").append(dir);
    Add(feat,fv);
    feat = "";
    feat.append("GC_WORD_cg=").append(ch_word).append("_").append(gc_word).append("_").append(dir);
    Add(feat,fv);
	feat = "";
    feat.append("GC_PW_pg=").append(par_pos).append("_").append(gc_word).append("_").append(dir);
    Add(feat,fv);
    feat = "";
    feat.append("GC_WP_cg=").append(ch_word).append("_").append(gc_pos).append("_").append(dir);
    Add(feat,fv);

	if(poptions->flag_coarsepos){
	feat = "";
    feat.append("GC_CPOS_pcg=").append(par_cpos).append("_").append(ch_cpos).append("_").append(gc_cpos).append("_").append(dir);
    Add(feat,  fv);

    feat = "";
    feat.append("GC_CPOS_pg=").append(par_cpos).append("_").append(gc_cpos).append("_").append(dir);
    Add(feat,fv);
    feat = "";
    feat.append("GC_CPOS_cg=").append(ch_cpos).append("_").append(gc_cpos).append("_").append(dir);
    Add(feat,fv);
    //
    feat = "";
    feat.append("GC_CPW_pg=").append(par_cpos).append("_").append(gc_word).append("_").append(dir);
    Add(feat,fv);
    feat = "";
    feat.append("GC_CWP_cg=").append(ch_word).append("_").append(gc_cpos).append("_").append(dir);
    Add(feat,fv);
	}
	if(poptions->flag_cluster){
	feat = "";
    feat.append("GC_C4_pcg=").append(par_c4).append("_").append(ch_c4).append("_").append(gc_c4).append("_").append(dir);
    Add(feat,  fv);

    feat = "";
    feat.append("GC_C4_pg=").append(par_c4).append("_").append(gc_c4).append("_").append(dir);
    Add(feat,fv);
    feat = "";
    feat.append("GC_C4_cg=").append(ch_c4).append("_").append(gc_c4).append("_").append(dir);
    Add(feat,fv);
    //
    feat = "";
    feat.append("GC_C4PW_pg=").append(par_c4).append("_").append(gc_cW).append("_").append(dir);
    Add(feat,fv);
    feat = "";
    feat.append("GC_C4WP_cg=").append(ch_cW).append("_").append(gc_c4).append("_").append(dir);
    Add(feat,fv);
	feat = "";
    feat.append("GC_C4PF_pg=").append(par_c4).append("_").append(gc_cF).append("_").append(dir);
    Add(feat,fv);
    feat = "";
    feat.append("GC_C4FP_cg=").append(ch_cF).append("_").append(gc_c4).append("_").append(dir);
    Add(feat,fv);
	//c6
	feat = "";
    feat.append("GC_C6_pcg=").append(par_c6).append("_").append(ch_c6).append("_").append(gc_c6).append("_").append(dir);
    Add(feat,  fv);

    feat = "";
    feat.append("GC_C6_pg=").append(par_c6).append("_").append(gc_c6).append("_").append(dir);
    Add(feat,fv);
    feat = "";
    feat.append("GC_C6_cg=").append(ch_c6).append("_").append(gc_c6).append("_").append(dir);
    Add(feat,fv);
    //
    feat = "";
    feat.append("GC_C6PW_pg=").append(par_c6).append("_").append(gc_cW).append("_").append(dir);
    Add(feat,fv);
    feat = "";
    feat.append("GC_C6WP_cg=").append(ch_cW).append("_").append(gc_c6).append("_").append(dir);
    Add(feat,fv);
    feat = "";
    feat.append("GC_C6PF_pg=").append(par_c6).append("_").append(gc_cF).append("_").append(dir);
    Add(feat,fv);
    feat = "";
    feat.append("GC_C6FP_cg=").append(ch_cF).append("_").append(gc_c6).append("_").append(dir);
    Add(feat,fv);
	}

	//others
	dir = dirpd;//same dir of h,d
	feat = "GCLIN=g1=";
    feat.append(ch_pos).append("_").append(gc_pos).append("_").append(gc_pos1).append("_").append(dir);
    Add(feat,fv);
	feat = "GCLIN=g_1=";
    feat.append(ch_pos).append("_").append(gc_pos).append("_").append(gc_pos_1).append("_").append(dir);
    Add(feat,fv);
	feat = "GCLIN=d1=";
    feat.append(ch_pos).append("_").append(gc_pos).append("_").append(ch_pos1).append("_").append(dir);
    Add(feat,fv);
	feat = "GCLIN=d_1=";
    feat.append(ch_pos).append("_").append(gc_pos).append("_").append(ch_pos_1).append("_").append(dir);
    Add(feat,fv);
	feat = "GCLIN=g1d_1=";
    feat.append(ch_pos).append("_").append(gc_pos).append("_").append(ch_pos_1).append("_").append(gc_pos1).append("_").append(dir);
    Add(feat,fv);
	feat = "GCLIN=g_1d1=";
    feat.append(ch_pos).append("_").append(gc_pos).append("_").append(ch_pos1).append("_").append(gc_pos_1).append("_").append(dir);
    Add(feat,fv);
	feat = "GCLIN=g1d1=";
    feat.append(ch_pos).append("_").append(gc_pos).append("_").append(ch_pos1).append("_").append(gc_pos1).append("_").append(dir);
    Add(feat,fv);
    feat = "GCLIN=g_1d_1=";
    feat.append(ch_pos).append("_").append(gc_pos).append("_").append(ch_pos_1).append("_").append(gc_pos_1).append("_").append(dir);
    Add(feat,fv);
	feat = "GCLIN=hg1=";
    feat.append(par_pos).append("_").append(gc_pos).append("_").append(gc_pos1).append("_").append(dir);
    Add(feat,fv);
	feat = "GCLIN=hg_1=";
    feat.append(par_pos).append("_").append(gc_pos).append("_").append(gc_pos_1).append("_").append(dir);
    Add(feat,fv);
	feat = "GCLIN=gh1=";
    feat.append(par_pos).append("_").append(gc_pos).append("_").append(par_pos1).append("_").append(dir);
    Add(feat,fv);
	feat = "GCLIN=gh_1=";
    feat.append(par_pos).append("_").append(gc_pos).append("_").append(par_pos_1).append("_").append(dir);
    Add(feat,fv);
	feat = "GCLIN=4g1h_1=";
    feat.append(par_pos).append("_").append(gc_pos).append("_").append(par_pos_1).append("_").append(gc_pos1).append("_").append(dir);
    Add(feat,fv);
	feat = "GCLIN=4g_1h_1=";
    feat.append(par_pos).append("_").append(gc_pos).append("_").append(par_pos_1).append("_").append(gc_pos_1).append("_").append(dir);
    Add(feat,fv);
	feat = "GCLIN=4g1h1=";
    feat.append(par_pos).append("_").append(gc_pos).append("_").append(par_pos1).append("_").append(gc_pos1).append("_").append(dir);
    Add(feat,fv);
	feat = "GCLIN=4g_1h1=";
    feat.append(par_pos).append("_").append(gc_pos).append("_").append(par_pos1).append("_").append(gc_pos_1).append("_").append(dir);
    Add(feat,fv);
}

void PipeO2::AddGCSubtreeFeatures(Instance &instance,
                      int par, int ch,
                      int gc,
                      FeatureVector &fv){
	//if((poptions->debug_level/10) % 10 != 1)
	//	return;
    vector<WordItem >::iterator it = instance.word_items.begin();
    string dir = par > ch? "RA" : "LA";
	bool attR = par < ch?true:false;

    string dirgc = gc < 0 ? "NOGC" : ch> gc? "RA": "LA";

    string type;
    type = gc == -1?"-1":"-2";
    dir.append( "_").append(dirgc);
    if(gc < 0){
        dir.append(type);
    }

    string par_pos = (it+par)->postag;
    string ch_pos =  (it+ch)->postag;
    string gc_pos = (gc < 0)?"STPOS": (it+gc)->postag;
    string par_word = (it+par)->form;
    string ch_word =  (it+ch)->form;
    string gc_word = (gc < 0)?"STWORD": (it+gc)->form;
	string st = "";
	string val;
	string prefix = "STGC";
	string newprefix;
	newprefix = prefix + dir;
    if(gc < 0){
        if(par < ch)
            st +=  "GC1 "+par_word+":0 "+ch_word+":1 "+gc_word+":"+type;
        else
            st += "GC1 "+ch_word+":2 "+par_word+":0 "+gc_word+":"+type;
    }
    else{
        if(par < ch && ch < gc)
            st += "GC "+par_word+":0 "+ch_word+":1 "+gc_word+":2";
        else if(par < ch && ch > gc)
            st += "GC "+par_word+":0 "+gc_word+":3 "+ch_word+":1";
        else if(par > ch && ch > gc)
            st += "GC "+gc_word+":2 "+ch_word+":3 "+par_word+":0";
        else if(par > ch && ch < gc)
            st += "GC "+ch_word+":3 "+gc_word+":1 "+par_word+":0";
        else{cout << ("CANT BE Here!") << endl;}
    }
	mydict.GetDictItemVal(subtree_list, st, val);
    AddSubtreeFeaturesItems(newprefix, attR, val, par_word, par_pos, fv);
	//cout << newprefix << " "<< st<< "\t" << val << endl;

}

void PipeO2::TagFeatureVector2(Instance &instance, vector<int> &tag_heads, vector<FeatureVector> &fvs,  vector<double> &probs,
							vector<FeatureVector> &fvs_trip,  vector<double> &probs_trip,
                            vector<FeatureVector> &fvs_sib,  vector<double> &probs_sib,
                            vector<FeatureVector> &fvs_gc,  vector<double> &probs_gc,
                            vector<int> &trip_ids,vector<int> &sib_ids,vector<int> &gc_ids,
                            vector<FeatureVector> &nt_fvs,vector<double> &pnt_probs,
                            FVLink &fvlink) {
	TagFeatureVector(instance, tag_heads, fvs,probs, nt_fvs,pnt_probs, fvlink);
	int inst_length = instance.word_num;
	vector<int>::iterator it = tag_heads.begin();
	vector<int>::iterator itb = tag_heads.begin();

	for(int i = 0; i < inst_length; i++, ++it) {
		int hd = *(it);
        if(hd == -1 && i != 0){
            continue;
        }
	    // right 
	    int prev = i;
		vector<int>::iterator itj = it+1;
	    for(int j = i+1; j < inst_length; j++, ++itj) {
		int hdj = *(itj);
		if(hdj == i) {
			int idx3_1 = GetFVTripIdx(inst_length,  i, prev, j);
            int idx2_1 = GetFVSIBIdx(inst_length,   prev, j, 0);
            int idx3 = trip_ids[idx3_1];
            int idx2 = sib_ids[idx2_1];
			FeatureVector *fv_trip = &fvs_trip.at(idx3);
            FeatureVector *fv_sib = prev == i? &fvs_sib.at(idx2):&fvs_sib.at(idx2+1);
			fvlink.Add(fv_trip);
			fvlink.Add(fv_sib);
		    //AddTripFeatures(instance,i,prev,j,fv);
		    //AddSiblingFeatures(instance,prev,j,prev==i,fv);
		    prev = j;
		}
	    }
		//left	
	    prev = i;
		itj = it-1;
	    for(int j = i-1; j >= 0; j--, --itj) {
		int hdj = *(itj);
        if(hdj == i) {
			int idx3_1 = GetFVTripIdx(inst_length,  i, prev, j);
            int idx2_1 = GetFVSIBIdx(inst_length,   prev, j, 0);
            int idx3 = trip_ids[idx3_1];
            int idx2 = sib_ids[idx2_1];
            FeatureVector *fv_trip = &fvs_trip.at(idx3);
            FeatureVector *fv_sib = prev == i? &fvs_sib.at(idx2):&fvs_sib.at(idx2+1);
            fvlink.Add(fv_trip);
            fvlink.Add(fv_sib);
		    //AddTripFeatures(instance,i,prev,j,fv);
		    //AddSiblingFeatures(instance,prev,j,prev==i,fv);
		    prev = j;
		}
	    }
	}
	//gc
	it = itb;
    for(int i = 0; i < inst_length; i++, ++it) {
        int hd = *(it);
        if(hd == -1 && i != 0){
            continue;
        }

        // right children
		vector<int>::iterator itj = it+1;
        for(int j = i+1; j < inst_length; j++, ++itj) {
		int hdj = *(itj);
        if(hdj < i) break;
        if(hdj == i) {
            bool flag = false;
            //cmi
			int gc = -1;
			vector<int>::iterator itk = it+1;
            for(int k = i+1; k < j;k++, ++itk){
				int hdk = *itk;
                if(hdk == j){
                    //AddGCFeatures(instance,i,j,k,fv);
					gc = k;
                    flag = true;
                    break;
                }
            }
            //if(flag == false) AddGCFeatures(instance,i,j,-1,fv);
			{
			int idx4_1 = GetFVGCIdx(inst_length,   i, j, gc);
            int idx4 = gc_ids[idx4_1];
            FeatureVector *fv_gc = &fvs_gc.at(idx4);
			fvlink.Add(fv_gc);
			}
            //cmo
            flag = false;
			itk = itb+inst_length-1;
			gc = -2;
            for(int k = inst_length-1; k > j;k--, --itk){
				int hdk = *itk;
                if(hdk == j){
                    //AddGCFeatures(instance,i,j,k,fv);
					gc = k;
                    flag = true;
                    break;
                }
            }
            //if(flag == false) AddGCFeatures(instance,i,j,-2,fv);
			{
			int idx4_1 = GetFVGCIdx(inst_length,   i, j, gc);
            int idx4 = gc_ids[idx4_1];
            FeatureVector *fv_gc = &fvs_gc.at(idx4);
            fvlink.Add(fv_gc);
			}
        }
        }
		//left
		itj = it-1;
        for(int j = i-1; j >= 0; j--,--itj) {
		int hdj = *itj;
        if(hdj > i) break;
        if(hdj == i) {
            bool flag = false;
            //cmi
			int gc = -1;
			vector<int>::iterator itk = it-1;
            for(int k = i-1; k > j;k--, --itk){
				int hdk = *itk;
                if(hdk == j){
                    //AddGCFeatures(instance,i,j,k,fv);
					gc = k;
                    flag = true;
                    break;
                }
            }
            //if(flag == false) AddGCFeatures(instance,i,j,-1,fv);
			{
            int idx4_1 = GetFVGCIdx(inst_length,   i, j, gc);
            int idx4 = gc_ids[idx4_1];
            FeatureVector *fv_gc = &fvs_gc.at(idx4);
            fvlink.Add(fv_gc);
            }
            flag = false;
            //cmo
			itk = itb;
			gc = -2;
            for(int k = 0; k < j; k++,++itk){
				int hdk = *itk;
                if(hdk == j){
                    //AddGCFeatures(instance,i,j,k,fv);
					gc = k;
                    flag = true;
                    break;
                }
            }
            //if(flag == false) AddGCFeatures(instance,i,j,-2,fv);
			{
            int idx4_1 = GetFVGCIdx(inst_length,   i, j, gc);
            int idx4 = gc_ids[idx4_1];
            FeatureVector *fv_gc = &fvs_gc.at(idx4);
            fvlink.Add(fv_gc);
            }

        }
        }
    }
}
void PipeO2::FillFeatureVectors2(Instance &instance,
                            vector<FeatureVector> &fvs,  vector<double> &probs,
							vector<FeatureVector> &fvs_trip,  vector<double> &probs_trip,
                            vector<FeatureVector> &fvs_sib,  vector<double> &probs_sib,
                            vector<FeatureVector> &fvs_gc,  vector<double> &probs_gc,
							vector<int> &trip_ids,vector<int> &sib_ids,vector<int> &gc_ids,
                            vector<FeatureVector> &nt_fvs,vector<double> &pnt_probs,
                             Parameters &params){
	FillFeatureVectors(instance, fvs, probs, nt_fvs, pnt_probs, params);
	int instance_length = instance.word_items.size();

	fvs_trip.reserve(instance_length*instance_length*instance_length);
	probs_trip.reserve(instance_length*instance_length*instance_length);
	trip_ids.reserve(instance_length*instance_length*instance_length);
	fvs_sib.reserve(2*instance_length*instance_length);
	probs_sib.reserve(2*instance_length*instance_length);
	sib_ids.reserve(2*instance_length*instance_length);
	fvs_gc.reserve((instance_length+2)*instance_length*instance_length);
	probs_gc.reserve((instance_length+2)*instance_length*instance_length);
	gc_ids.reserve((instance_length+2)*instance_length*instance_length);

	int idx = 0;
	int maxsize = 2;
    FeatureVector fv;
	for(int w1 = 0; w1 < instance_length; w1++) {
        //for(int w2 = w1; w2 < instance_length; w2++) {
        for(int w2 = 0; w2 < instance_length; w2++) {
        //for(int w3 = w2+1; w3 < instance_length; w3++) {
        for(int w3 = 0; w3 < instance_length; w3++) {
			if(IsTrip(w1,w2, w3)){
			fv.Clear();
			fv.Reserve(maxsize);
            AddTripFeatures(instance,w1,w2,w3,fv);
			if(fv.GetFeatSize() > maxsize)
				maxsize = fv.GetFeatSize();			
            double prob = fv.GetScore(params);
			//fvs_trip.push_back(fv);
            probs_trip.push_back(prob);
			trip_ids.push_back(idx);
			idx++;
			}
			else
				trip_ids.push_back(-1);
        }
        }
    }
	idx = 0;
	maxsize = 12;
    for(int w1 = 0; w1 < instance_length; w1++) {
        for(int w2 = 0; w2 < instance_length; w2++) {
        for(int wh = 0; wh < 2; wh++) {
            //if(w1 != w2)
            if(IsSIB(w1, w2)){
            //FeatureVector fv;
			fv.Clear();
			fv.Reserve(maxsize);
            AddSiblingFeatures(instance,w1,w2,wh == 0,fv);
			if(fv.GetFeatSize() > maxsize)
                maxsize = fv.GetFeatSize();
            double prob = fv.GetScore(params);
			//fvs_sib.push_back(fv);
            probs_sib.push_back(prob);
			sib_ids.push_back(idx);
            idx++;
            }
			else
                sib_ids.push_back(-1);
        }
        }
    }
	idx = 0;
	maxsize = 7;
    for(int w1 = 0; w1 < instance_length; w1++) {
        for(int w2 = 0; w2 < instance_length; w2++) {
        for(int w3 = -2; w3 < instance_length; w3++) {
			if(IsGC(w1, w2, w3)){
            //FeatureVector fv;
			fv.Clear();
			fv.Reserve(maxsize);
            AddGCFeatures(instance,w1, w2, w3, fv);
			if(fv.GetFeatSize() > maxsize)
                maxsize = fv.GetFeatSize();
            double prodProb = fv.GetScore(params);
            //fvs_gc.push_back(fv);
            probs_gc.push_back(prodProb);
			gc_ids.push_back(idx);
			idx++;
			}
			else
				gc_ids.push_back(-1);
        }
        }
    }

}


void PipeO2::WriteExtendedFeatures(Instance &instance, ofstream &feat_wd){
	//cout << "PipeO2-WriExtendedFeatures" << endl;
	//return;
	int instance_length = instance.word_items.size();
    // Get production crap.a
	//boost::archive::binary_oarchive oa(feat_wd);
    int idx = 0;
	vector<int> keys;
	int maxsize = 2;
	FeatureVector fv;
	for(int w1 = 0; w1 < instance_length; w1++) {
	    //for(int w2 = w1; w2 < instance_length; w2++) {
	    for(int w2 = 0; w2 < instance_length; w2++) {
		//for(int w3 = w2+1; w3 < instance_length; w3++) {
		for(int w3 = 0; w3 < instance_length; w3++) {
			if(IsTrip(w1,w2,w3)){
				fv.Clear();
				fv.Reserve(maxsize);
		    	AddTripFeatures(instance,w1,w2,w3,fv);
				if(fv.GetFeatSize() > maxsize)
                	maxsize = fv.GetFeatSize();
				fv.GetKeys(keys);
				idx++;
			}
		}
	    }
	}
			
	maxsize = 12;
	for(int w1 = 0; w1 < instance_length; w1++) {
	    for(int w2 = 0; w2 < instance_length; w2++) {
		for(int wh = 0; wh < 2; wh++) {
		    //if(w1 != w2) 
			if(IsSIB(w1,w2)){
				//FeatureVector fv;
				fv.Clear();
				fv.Reserve(maxsize);
				AddSiblingFeatures(instance,w1,w2,wh == 0,fv);
				if(fv.GetFeatSize() > maxsize)
                    maxsize = fv.GetFeatSize();
				fv.GetKeys(keys);
                idx++;
		    }
		}
	    }
	}
	maxsize = 10;
	for(int w1 = 0; w1 < instance_length; w1++) {
        for(int w2 = 0; w2 < instance_length; w2++) {
        for(int w3 = -2; w3 < instance_length; w3++) {
			if(IsGC(w1,w2,w3)){
            //FeatureVector fv;
			fv.Clear();
			fv.Reserve(maxsize);
            AddGCFeatures(instance,w1, w2, w3, fv);
			if(fv.GetFeatSize() > maxsize)
                    maxsize = fv.GetFeatSize();
			fv.GetKeys(keys);
            idx++;
			}
        }
        }
    }
	//int ids_len = fv_ids.size();
	//oa << ids_len;
	//oa << fv_ids;
	MyTool::VectorWrite(feat_wd, keys);
//	int keys_len = keys.size();
//	oa << keys_len;
//	oa << keys;
}

void PipeO2::ReadInstance2(ifstream &feat_rd, Instance &instance,
                            vector<FeatureVector> &fvs,  vector<double> &probs,
                            vector<FeatureVector> &fvs_trip,  vector<double> &probs_trip,
                            vector<FeatureVector> &fvs_sib,  vector<double> &probs_sib,
                            vector<FeatureVector> &fvs_gc,  vector<double> &probs_gc,
							vector<int> &trip_ids,vector<int> &sib_ids,vector<int> &gc_ids,
                            vector<FeatureVector> &nt_fvs,vector<double> &nt_probs,
                             Parameters &params){
	//cout << "PipeO2-ReadExtendedFeatures" << endl;
	//read O1 
	ReadInstance(feat_rd, instance, fvs, probs, nt_fvs, nt_probs, params);
	//return;
    int instance_length = instance.word_items.size();

    fvs_trip.reserve(instance_length*instance_length*instance_length);
    probs_trip.reserve(instance_length*instance_length*instance_length);
    trip_ids.reserve(instance_length*instance_length*instance_length);
    fvs_sib.reserve(2*instance_length*instance_length);
    probs_sib.reserve(2*instance_length*instance_length);
    sib_ids.reserve(2*instance_length*instance_length);
    fvs_gc.reserve((instance_length+2)*instance_length*instance_length);
    probs_gc.reserve((instance_length+2)*instance_length*instance_length);
    gc_ids.reserve((instance_length+2)*instance_length*instance_length);



    int idx = 0;
    //boost::archive::binary_iarchive ia(feat_rd);

	vector<int> keys;
	MyTool::VectorRead(feat_rd, keys);

//	int keys_len = 0;
//	ia >> keys_len;
//	keys.reserve(keys_len);
//	ia >> keys;
	int keys_idx = 0;
	int keys_size = 0;
	FeatureVector fv;
    for(int w1 = 0; w1 < instance_length; w1++) {
        for(int w2 = 0; w2 < instance_length; w2++) {
        for(int w3 = 0; w3 < instance_length; w3++) {
			if(IsTrip(w1, w2, w3)){
			//FeatureVector fv;
			fv.Clear();
            keys_size = keys.at(keys_idx++);
            //cout << keys_idx << " " << keys_size << endl;
            if(keys_size > 0){
                fv.Reserve(keys_size);
                fv.Add(keys, keys_idx, keys_size);
                keys_idx += keys_size;
            }
            //fvs_trip.push_back(fv);
            double prob = fv.GetScore(params);
            probs_trip.push_back(prob);
			trip_ids.push_back(idx);
            idx++;
			}
			else
				trip_ids.push_back(-1);
        }
        }
    }

	idx = 0;
    for(int w1 = 0; w1 < instance_length; w1++) {
        for(int w2 = 0; w2 < instance_length; w2++) {
        for(int wh = 0; wh < 2; wh++) {
            //if(w1 != w2) 
			if(IsSIB(w1,w2)){
			//FeatureVector fv;
			fv.Clear();
            keys_size = keys.at(keys_idx++);
            //cout << keys_idx << " " << keys_size << endl;
            if(keys_size > 0){
                fv.Reserve(keys_size);
                fv.Add(keys, keys_idx, keys_size);
                keys_idx += keys_size;
            }
            //fvs_sib.push_back(fv);
            double prob = fv.GetScore(params);
            probs_sib.push_back(prob);
			sib_ids.push_back(idx);
            idx++;

            }
			else
				sib_ids.push_back(-1);
        }
        }
    }
	idx = 0;
	for(int w1 = 0; w1 < instance_length; w1++) {
        for(int w2 = 0; w2 < instance_length; w2++) {
        for(int w3 = -2; w3 < instance_length; w3++) {
			if(IsGC(w1, w2, w3)){
            //FeatureVector fv;
			fv.Clear();
			keys_size = keys.at(keys_idx++);
            //cout << keys_idx << " " << keys_size << endl;
            if(keys_size > 0){
                fv.Reserve(keys_size);
                fv.Add(keys, keys_idx, keys_size);
                keys_idx += keys_size;
            }
            //fvs_gc.push_back(fv);
            double prob = fv.GetScore(params);
            probs_gc.push_back(prob);
			gc_ids.push_back(idx);
            idx++;
			}
			else
				gc_ids.push_back(-1);
        }
        }
    }
}


bool PipeO2::IsSIB(int ch1, int ch2){
	if(ch1 == ch2) return false;
	return true;
}
bool PipeO2::IsTrip(int par, int ch1, int ch2){
	if(!((ch1 >= par && ch2 > ch1) || (par >= ch1 && ch1 > ch2)))
        return false;
	return true;
}

bool PipeO2::IsGC(int par, int ch, int gc){
	if(ch == gc) return false;
    if(gc >= 0 && !((par > ch && par > gc) || (par < ch && par < gc)))
        return false;
	return true;
}



int PipeO2::GetFVTripIdx(int instance_length, int s, int r, int t){
	int idx = 0;
	idx = (s)*instance_length*instance_length + (r)*instance_length + t;
	return idx;
}
int PipeO2::GetFVTripIdx2(int instance_length, int s, int r, int t){
		int idx = 0;
		int idx1 = 0;
		int idx2 = 0;
	    for(int w1 = 0; w1 < instance_length; w1++) {
        for(int w2 = 0; w2 < instance_length; w2++) {
        for(int w3 = 0; w3 < instance_length; w3++) {
			if(w1 == s && w2 == r && w3 == t)
				return idx;
			idx2 = GetFVTripIdx(instance_length, w1, w2, w3);
			if(idx2 != idx)
				cout << "idx:" << idx2 << " vs " << idx << endl;
			idx++;
			idx1++;
        }
        }
		}
		return idx;
 
}
int PipeO2::GetFVSIBIdx2(int instance_length, int r, int t, int type){
	int idx = 0;
	    for(int w1 = 0; w1 < instance_length; w1++) {
        for(int w2 = 0; w2 < instance_length; w2++) {
        for(int wh = 0; wh < 2; wh++) {
            //if(w1 != w2) 
			{
				if(w1 == r && w2 == t && type == wh){
					return idx;
				}
				int idx1 = GetFVSIBIdx(instance_length, w1, w2, wh);
				if(idx1 != idx)
					cout << idx1 << " != " << idx << endl;
				idx++;
			}
		}}}
	return idx;
}
int PipeO2::GetFVSIBIdx(int instance_length, int r, int t, int type){
    int vect_idx = r*instance_length*2 + t*2 + type;//   2*(r*instance_length -(r+1) + t)+type;
	return vect_idx;
 }
int PipeO2::GetFVGCIdx(int instance_length, int k1, int k2, int k3){
	int vect_idx = k1*instance_length*(instance_length+2)+k2*(instance_length+2)+k3+2;
	return vect_idx;

}
int PipeO2::GetFVGCIdx2(int instance_length, int k1, int k2, int k3){
	int idx = 0;
	for(int w1 = 0; w1 < instance_length; w1++) {
        for(int w2 = 0; w2 < instance_length; w2++) {
        for(int w3 = -2; w3 < instance_length; w3++) {
			int idx1 = GetFVGCIdx(instance_length, w1,w2,w3);
			if(idx != idx1){
				cout << idx1 << " != " << idx << endl;
			}
			idx++;
		}}
	}
	return 0;
}
void PipeO2::TestIdx(){
	GetFVGCIdx2(100,0,0,0);
	return;
	for(int t = 20; t < 30; t++){
	int idx1 = GetFVSIBIdx(100, 10, t, 0);
	int idx2 = GetFVSIBIdx2(100, 10, t, 0);
	//cout << idx1 << " vs "<< idx2 << endl;
	}
	for(int t = 30; t < 35; t++){
    int idx1 = GetFVTripIdx(100, 35,35, t);
    int idx2 = GetFVTripIdx2(100, 35,35, t);
    //cout << idx1 << " vs "<< idx2 << endl;
    }
	for(int t = 20; t < 30; t++){
    int idx1 = GetFVIdx(100, 10, t);
    int idx2 = GetFVIdx2(100, 10, t);
    //cout << idx1 << " vs "<< idx2 << endl;
    }

}
