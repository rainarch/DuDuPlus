#include "TagParser.h"

TagParser::TagParser(Segger *psegger, Tagger *ptagger, Parser *pparser){
	this->psegger = psegger;
	this->ptagger = ptagger;
	this->pparser = pparser;
}

void TagParser::Test(string &testfile){
	time_t start_time, end_time;
    start_time = time(NULL);
	string type = "DP";//final step
	string format = "CONLL";
    BaseWriter result_wd(pparser->poptions->outfile, format, type);
    if(!result_wd.IsOpen())
        return;
	if(pparser->poptions->lang == "cn"){
    if(psegger->ppipe->InitFile(testfile) < 0){
        cerr << "Err: opening testfile " << testfile << endl;
        return;
    }
	}
	else{
    if(ptagger->ppipe->InitFile(testfile) < 0){
        cerr << "Err: opening testfile " << testfile << endl;
        return;
    }
	}
    cerr << "Processing sentences ..." << endl;
	ptagger->decoder.types_num = ptagger->ppipe->types_num;
	psegger->decoder.types_num = psegger->ppipe->types_num;
    int i = 0;
    for(i = 0; ; i++) {
        if((i+1) % 100 == 0) {
        cerr << (i+1) << "," << flush;
        //  end_time = time(NULL);
        //  cerr << (i+1) << "("<< (end_time-start_time)<< ")," << flush;
        //  break;
        }
		
        Instance instance;
		if(pparser->poptions->lang == "cn"){
        	psegger->ppipe->NextInstance(instance);
		}
		else
        	ptagger->ppipe->NextInstance(instance);
		if(instance.word_items.size() == 0)
            break;
		//instance.PrintInst();
		//if(i < 22600)
		//	continue;
		if(instance.word_items.size() > (pparser->poptions->max_sentlen)){
			cerr << "Err: Too long! Skipped! INFO:Len="<<instance.word_items.size() 
                 << " Max_len="<<pparser->poptions->max_sentlen<< endl;
			continue;
		}
		if(pparser->poptions->lang == "cn"){
			psegger->TestOneSent(instance);
		}
		//instance.PrintInst();

		//tagging
		ptagger->TestOneSent(instance);
		//instance.PrintInst();
		//reform the first and lastest words //cpos vs pos mark here
		WordItem word_item("0\t_root\t_root\t_rootpos\t_rootcpos\tnull\t-1\t_rootrel", "\t");
    	instance.word_items.erase(instance.word_items.begin()) ;//remove first
		instance.word_items.insert(instance.word_items.begin(), word_item);
		instance.word_items.pop_back();//remove the lastest WEND
		instance.word_num = instance.word_items.size();
		//parsing
		int K = pparser->poptions->testK;
		vector<ResultItem > d;
		pparser->TestOneSent(instance, d);
		//instance.PrintInst();
        if(d.size() > 0){
            for(int k = 0; k < d.size(); ++k){
                pparser->ppipe->TypeID2Str(d[k].parse_tree);
            }
            result_wd.OutputInstance( instance, d, K);
        }
		//break;
 	}
	if(pparser->poptions->lang == "cn")
	psegger->ppipe->CloseFile();
	else
	ptagger->ppipe->CloseFile();
    result_wd.CloseFile();
    end_time = time(NULL);
    cerr << (i) << "(Time:"<< (end_time-start_time)<< ")" << endl;
}
