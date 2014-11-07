#include "PipeFull.h"
#include <stdlib.h>
#include <ctime>
void PipeFull::AddHigherFeatures(Instance &instance,FeatureVector &fv){
	
	//cout << "PipeFull-AddHigherFeatures" << endl;
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
		int prev1 = i;
		vector<WordItem>::iterator itj = it+1;
	    for(int j = i+1; j < inst_length; j++, ++itj) {
		int hdj = (itj)->head;
		if(hdj == i) {
		    AddTriSibFeatures(instance,i,prev1, prev,j,fv);
			prev1 = prev;
		    prev = j;
		}
	    }
		//left	
	    prev = i;
		prev1 = i;
		itj = it-1;
	    for(int j = i-1; j >= 0; j--, --itj) {
		int hdj = (itj)->head;
        if(hdj == i) {
		    AddTriSibFeatures(instance,i,prev1, prev,j,fv);
			prev1 = prev;
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
			int gc1 = -1;
			int gc = -1;
            for(int k = i+1; k < j;k++, ++itk){
				int hdk = itk->head;
                if(hdk == j){
					gc1 = gc;
					gc = k;
					if(gc1 != -1){
                    flag = true;
                    break;
					}
                }
            }
            AddGSibFeatures(instance,i,j,gc,gc1,fv);
            //cmo
            flag = false;
			gc1 = -2;
			gc = -2;
			itk = itb+inst_length-1;
            for(int k = inst_length-1; k > j;k--, --itk){
				int hdk = itk->head;
                if(hdk == j){
					gc1 = gc;
                    gc = k;
                    if(gc1 != -2){
                    flag = true;
                    break;
                    }

                }
            }
            AddGSibFeatures(instance,i,j,gc,gc1,fv);
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
			int gc1 = -1;
			int gc = -1;
			vector<WordItem>::iterator itk = it-1;
            for(int k = i-1; k > j;k--, --itk){
				int hdk = itk->head;
                if(hdk == j){
					gc1 = gc;
                    gc = k;
                    if(gc1 != -1){
                    flag = true;
                    break;
                    }

                }
            }
            AddGSibFeatures(instance,i,j,gc,gc1,fv);

            flag = false;
            //cmo
			gc1 = -2;
			gc = -2;
			itk = itb;
            for(int k = 0; k < j; k++,++itk){
				int hdk = itk->head;
                if(hdk == j){
					gc1 = gc;
                    gc = k;
                    if(gc1 != -2){
                    flag = true;
                    break;
                    }

                }
            }
            AddGSibFeatures(instance,i,j,gc, gc1,fv);
        }
        }
    }
}

void PipeFull::AddGSibFeatures(Instance &instance,int par, int ch, int gc, int gc1, FeatureVector &fv){
	//returna;
	//par ch gc1 gc
	//gc gc1 ch par
    vector<WordItem >::iterator it = instance.word_items.begin();
	string whd = (it+par)->form;
    if(!IsGoodHead( whd))
        return;//dont generate the features for some zerohd words
	
    string dir = par > ch? "RA" : "LA";
    string dirgc = gc < 0 ? "NOGC" : ch> gc? "RA": "LA";

    string type;
    type = gc1 == -1?"-1":"-2";
    dir.append( "_").append(dirgc);
    if(gc < 0){
        dir.append(type);
    }

    string par_pos = (it+par)->postag;
    string ch_pos =  (it+ch)->postag;
    string gc_pos = (gc < 0)?"STPOS": (it+gc)->postag;
    string gc1_pos = (gc1 < 0)?"STPOS1": (it+gc1)->postag;
	string par_cpos = (it+par)->cpostag;
    string ch_cpos =  (it+ch)->cpostag;
    string gc_cpos = (gc < 0)?"STCPOS": (it+gc)->cpostag;
    string gc1_cpos = (gc1 < 0)?"STCPOS1": (it+gc1)->cpostag;

    string par_word = (it+par)->form;
    string ch_word =  (it+ch)->form;
    string gc_word = (gc < 0)?"STWORD": (it+gc)->form;
    string gc1_word = (gc1 < 0)?"STWORD1": (it+gc1)->form;
	string feat;

    feat = "";
    feat.append("GSB_P2=").append(par_word).append("_").append(ch_pos).append("_").append(gc_pos).append("_").append(gc1_pos).append("_").append(dir);
    Add(feat,  fv);
    feat = "";
    feat.append("GSB_P3=").append(par_pos).append("_").append(ch_word).append("_").append(gc_pos).append("_").append(gc1_pos).append("_").append(dir);
    Add(feat,  fv);
    feat = "";
    feat.append("GSB_P3=").append(par_pos).append("_").append(ch_pos).append("_").append(gc_word).append("_").append(gc1_pos).append("_").append(dir);
    Add(feat,  fv);
    feat = "";
    feat.append("GSB_P4=").append(par_pos).append("_").append(ch_pos).append("_").append(gc_pos).append("_").append(gc1_word).append("_").append(dir);
    Add(feat,  fv);
    feat = "";
    feat.append("GSB_P=").append(par_pos).append("_").append(ch_pos).append("_").append(gc_pos).append("_").append(gc1_pos).append("_").append(dir);
    Add(feat,  fv);
	//backed-off-features
    feat = "";
    feat.append("GSB_P=").append(par_pos).append("_").append(gc_pos).append("_").append(gc1_pos).append("_").append(dir);
    Add(feat,  fv);
	//coordination
	if(gc1_pos == "CC"){
		feat = "";
    	feat.append("GSB_COO=").append(par_pos).append("_").append(gc1_pos).append("_").append(dir);
    	Add(feat,  fv);
	}
	if(poptions->flag_coarsepos){
	feat = "";
    feat.append("GSB_CP2=").append(par_word).append("_").append(ch_cpos).append("_").append(gc_cpos).append("_").append(gc1_cpos).append("_").append(dir);
    Add(feat,  fv);
    feat = "";
    feat.append("GSB_CP3=").append(par_cpos).append("_").append(ch_word).append("_").append(gc_cpos).append("_").append(gc1_cpos).append("_").append(dir);
    Add(feat,  fv);
    feat = "";
    feat.append("GSB_CP3=").append(par_cpos).append("_").append(ch_cpos).append("_").append(gc_word).append("_").append(gc1_cpos).append("_").append(dir);
    Add(feat,  fv);
    feat = "";
    feat.append("GSB_CP4=").append(par_cpos).append("_").append(ch_cpos).append("_").append(gc_cpos).append("_").append(gc1_word).append("_").append(dir);
    Add(feat,  fv);
    feat = "";
    feat.append("GSB_CP=").append(par_cpos).append("_").append(ch_cpos).append("_").append(gc_cpos).append("_").append(gc1_cpos).append("_").append(dir);
    Add(feat,  fv);
    //backed-off-features
    feat = "";
    feat.append("GSB_CP=").append(par_cpos).append("_").append(gc_cpos).append("_").append(gc1_cpos).append("_").append(dir);
    Add(feat,  fv);
	}
}

void PipeFull::AddTriSibFeatures(Instance &instance,int par, int ch1, int ch2, int ch3, FeatureVector &fv){
	//return;
	if(par == ch3) return;
	//cout << par << " " << ch1 << " " << ch2 << " "<< ch3 << endl;
 	vector<WordItem >::iterator it = instance.word_items.begin();

	string whd = (it+par)->form;
    if(!IsGoodHead( whd))
        return;//dont generate the features for some zerohd words

    string dir = ch1 > ch3? "RA" : "LA";
    //ch2 ch1 par or par ch1 ch2
    string ch1_pos = (ch1 == par)? "STPOS1":(it+ch1)->postag;
    string ch2_pos = (ch2 == par)? "STPOS2":(it+ch2)->postag;
    string ch3_pos = (it+ch3)->postag;
	string par_pos = (it+par)->postag;
	string ch1_cpos = (ch1 == par)? "STCPOS1":(it+ch1)->cpostag;
    string ch2_cpos = (ch2 == par)? "STCPOS2":(it+ch2)->cpostag;
    string ch3_cpos = (it+ch3)->cpostag;
    string par_cpos = (it+par)->cpostag;

	
    string ch1_word = (ch1 == par)? "STWD1":(it+ch1)->form;
    string ch2_word = (ch2 == par)? "STWD2":(it+ch2)->form;
    string ch3_word = (it+ch3)->form;
	string par_word = (it+par)->form;

	string feat;
	feat = "";
    feat.append("3SB_P=").append(par_pos).append("_").append(ch1_pos).append("_").append(ch2_pos).append("_").append(ch3_pos).append(dir);
    Add(feat, fv);

	feat = "";
	feat.append("3SB_PW1=").append(par_word).append("_").append(ch1_pos).append("_").append(ch2_pos).append("_").append(ch3_pos).append(dir);
    Add(feat,  fv);
	feat = "";
	feat.append("3SB_PW2=").append(par_pos).append("_").append(ch1_word).append("_").append(ch2_pos).append("_").append(ch3_pos).append(dir);
    Add(feat,  fv);
	feat = "";
    feat.append("3SB_PW3=").append(par_pos).append("_").append(ch1_pos).append("_").append(ch2_word).append("_").append(ch3_pos).append(dir);
    Add(feat,  fv);
	feat = "";
    feat.append("3SB_PW4=").append(par_pos).append("_").append(ch1_pos).append("_").append(ch2_pos).append("_").append(ch3_word).append(dir);
    Add(feat,  fv);
	//backed-off 
	feat = "";
    feat.append("3SB_P0=").append(ch1_pos).append("_").append(ch2_pos).append("_").append(ch3_pos).append(dir);
    Add(feat,  fv);
	feat = "";
    feat.append("3SB_P0=").append(ch1_pos).append("_").append(ch3_pos).append(dir);
    Add(feat,  fv);

	if(poptions->flag_coarsepos){
	feat = "";
    feat.append("3SB_CP=").append(par_cpos).append("_").append(ch1_cpos).append("_").append(ch2_cpos).append("_").append(ch3_cpos).append(dir);
    Add(feat, fv);

    feat = "";
    feat.append("3SB_CPW1=").append(par_word).append("_").append(ch1_cpos).append("_").append(ch2_cpos).append("_").append(ch3_cpos).append(dir);
    Add(feat,  fv);
    feat = "";
    feat.append("3SB_CPW2=").append(par_cpos).append("_").append(ch1_word).append("_").append(ch2_cpos).append("_").append(ch3_cpos).append(dir);
    Add(feat,  fv);
    feat = "";
    feat.append("3SB_CPW3=").append(par_cpos).append("_").append(ch1_cpos).append("_").append(ch2_word).append("_").append(ch3_cpos).append(dir);
    Add(feat,  fv);
    feat = "";
    feat.append("3SB_CPW4=").append(par_cpos).append("_").append(ch1_cpos).append("_").append(ch2_cpos).append("_").append(ch3_word).append(dir);
    Add(feat,  fv);
    //backed-off
    feat = "";
    feat.append("3SB_CP0=").append(ch1_cpos).append("_").append(ch2_cpos).append("_").append(ch3_cpos).append(dir);
    Add(feat,  fv);
    feat = "";
    feat.append("3SB_CP0=").append(ch1_cpos).append("_").append(ch3_cpos).append(dir);
    Add(feat,  fv);
	}
}
int  PipeFull::GetGSibSize(int sent_len){
	return sent_len *(sent_len+2)*(sent_len+2)*(sent_len);
}
int  PipeFull::GetGSibIdx(int sent_len, int par, int ch, int gc, int gc1){
	int idx = 0;
	idx = par*(sent_len+2)*(sent_len+2)*(sent_len) + (gc+2)*(sent_len+2)*(sent_len)+ (gc1+2)*(sent_len)+ch;
	return idx;
}
int  PipeFull::GetTriSibSize(int sent_len){
	return sent_len *(sent_len)*(sent_len)*(sent_len);
}
int  PipeFull::GetTriSibIdx(int sent_len, int par, int ch1, int ch2, int ch3){
	int idx = 0;
    idx = par*(sent_len)*(sent_len)*(sent_len) + (ch1)*(sent_len)*(sent_len)+ (ch2)*(sent_len)+ch3;
    return idx;
}
