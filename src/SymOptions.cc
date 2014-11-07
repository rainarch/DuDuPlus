#include "TypeConvert.h"
#include "SymOptions.h"

SymOptions::SymOptions(int argc, char* argv[]){
	//init
	//for featfile
	featfile = "tmp/feat.tmp";
	help = false;
    train = false;
    eval = false;
    test = false;
	testfile = "-";
	outfile = "-";
    model_name = "tmp/test.model";
    posmodel_name = "tmp/test.posmodel";
    segmodel_name = "tmp/test.segmodel";
    loss_type = "punc";
    decode_type = "proj";
    num_iters = 10;
    trainK = 1;
    testK =1 ;
	order = 1;
	beamfeat = false;	

	usefeat = false;
	feat_memo = false;
	format = "CONLL";	

	lang = "en";
	task = "Parser";

	labeled_parsing = false;
	flag_coarsepos =  true;
	flag_subtree = false;
	flag_cluster = false;
	subtreefile = "NONE";
	clusterfile = "NONE";
	runtype = "NONE";
	
	max_sentlen = 500;
	debug_level = 0;
	DEBUG = false;
	for(int i = 0; i < argc; i++) {
		vector<string> strs;
		string namevalue = argv[i];
    	boost::split(strs, namevalue, boost::is_any_of(":"));
		if(strs.size() == 0 || strs.size() > 2){
			cerr << "Err: parser options ="<< argv[i] << endl;
			continue;
		}
		string name = strs.at(0);
		
		string value = "";
		if(strs.size() >= 2)
			value = strs.at(1);
		if (name.compare("help") == 0) {
        help = true;
        }
	    if (name.compare("train") == 0) {
		train = (value == "")?true:((value == "true"))? true:false;
	    }
	    if (name.compare("eval")==0) {
		eval = (value == "")?true:((value == "true"))? true:false;
	    }
	    if (name.compare("test")==0) {
		test = (value == "")?true:((value == "true"))? true:false;
	    }
	    if (name.compare("iters")==0) {
		num_iters = TypeConvert::Str2Int(value);
	    }
	    if (name.compare("output-file")==0) {
		outfile = value;
	    }
	    if (name.compare("gold-file")==0) {
		goldfile = value;
	    }
	    if (name.compare("train-file")==0) {
		trainfile = value;
	    }
	    if (name.compare("test-file")==0) {
		testfile = value;
	    }
		if (name.compare("feat-file")==0) {
        featfile = value;
        }

	    if (name.compare("model-name")==0) {
		model_name = value;
	    }
		if (name.compare("posmodel-name")==0) {
        posmodel_name = value;
        }	
		if (name.compare("segmodel-name")==0) {
        segmodel_name = value;
        }

	    if (name.compare("training-k")==0) {
		trainK = TypeConvert::Str2Int(value);
		//cerr << "value:" << value << " vs trainK:" << trainK << endl;
	    }
		if (name.compare("max-sentlen")==0) {
        max_sentlen = TypeConvert::Str2Int(value);
        //cerr << "value:" << value << " vs trainK:" << trainK << endl;
        }

	    if (name.compare("test-k")==0) {
		testK = TypeConvert::Str2Int(value);
	    }
	    if (name.compare("loss-type")==0) {
		loss_type = value;
	    }			
	    if (name.compare("decode-type")==0) {
		decode_type = value;
	    }			
	    if (name.compare("order") ==0 ) {
		order = TypeConvert::Str2Int(value);
	    }
			
	    if (name.compare("use-featforest")==0) {
		usefeat = (value == "true")? true:false;
	    }			
		if (name.compare("feat-memo")==0) {
        feat_memo = (value == "true")? true:false;
        }
		if (name.compare("task")==0) {
        task = value;
        }
		if (name.compare("lang")==0) {
        lang = value;
        }
		if (name.compare("format")==0) {
        format = value;
        }
		if (name.compare("subtree-file")==0) {
        subtreefile = value;
        }
        if (name.compare("cluster-file")==0) {
        clusterfile = value;
        }

		if (name.compare("debug-level") ==0 ) {
        debug_level = TypeConvert::Str2Int(value);
        }
		if (name.compare("DEBUG")==0) {
        DEBUG = (value == "true")? true:false;
        }
		if (name.compare("beamfeat")==0) {
        beamfeat = (value == "true")? true:false;
        }
		if (name.compare("flag-subtree")==0) {
        flag_subtree = (value == "true")? true:false;
        }
	 	if (name.compare("flag-cluster")==0) {
        flag_cluster = (value == "true")? true:false;
        }
		if (name.compare("flag-coarsepos")==0) {
        flag_coarsepos = (value == "true")? true:false;
        }
		if (name.compare("labeled-parsing")==0) {
        labeled_parsing = (value == "true")? true:false;
        }
		if (name.compare("runtype")==0) {
        runtype = value;
        }


	}
	//set runtype
	SetRunType(runtype);
	//
	if(lang == "cn"){//if Chinese, dont use coarsepos
		cerr << "Lang:cn ignore the features of coarsepos" << endl;
		flag_coarsepos = false;
	}
	if(subtreefile == "NONE")
	subtreefile = (string)"res/"+lang+".ST";
	if(clusterfile == "NONE")
    clusterfile = (string)"res/"+lang+".word.clusters";

	if(help)
		PrintHelp();
}

void SymOptions::SetRunType(string &runtype){ //
	if(runtype.find("LB") == 0)
		labeled_parsing = true;
	
	if(runtype.find("BASIC") != string::npos){
		flag_subtree = false;
		flag_cluster = false;
	}
	else if(runtype.find("ST") != string::npos){
        flag_subtree = true;
        flag_cluster = false;
    }
	else if(runtype.find("CL") != string::npos){
        flag_subtree = false;
        flag_cluster = true;
    }
	else if(runtype.find("FULL") != string::npos){
        flag_subtree = true;
        flag_cluster = true;
    }
}

void SymOptions::PrintOptions(){
	cerr << "====Options====" << endl;
	cerr << "task:" << task << endl;
	cerr << "train_file:" << trainfile << "\ttest_file:" << testfile << "\tfeat_file:" << featfile << endl;
	cerr << "model_name:" << model_name << "\tout_file:"<< outfile << "\tgold_file:" << goldfile << endl;
	cerr << "loss_type:" << loss_type << "\tdecode_type:" << decode_type << "\tnum_iters:" << num_iters<< endl; 
	cerr << "train :" << train <<  "\ttest :" << test <<  "\teval :" << eval << endl;
	cerr << "training-k:" << trainK << "\ttest-k:" << testK << "\torder:" << order <<  endl;
	cerr << "use-featforest:" << usefeat << "\tdebug_level:" << debug_level << endl;
	cerr << "feat_memo:" << feat_memo <<"\tlang:"<< lang <<"\tbeamfeat:"<<beamfeat<< endl;
	cerr << "flag_coarsepos:"<< flag_coarsepos << endl;
	cerr << "====EndOfOpt===" << endl;

}
void SymOptions::PrintHelp(){
	cerr << "====Help for DuDuPlus===" << endl;
	cerr << "Usage: ./DuDuPlus parameter1:value1 parameter2:value2 ...\n" << endl;
	cerr << "Parameter(defaultvalue)\tcomment" << endl;
	cerr << "task(Parser)\tParser, Tagger, or TagParser" << endl;
	cerr << "train(false)\tTo train a new model" << endl;
	cerr << "test(false)\tTo test a trained model" << endl;
	cerr << "test-file(-)\tThe name of test file. (-): standard input" << endl;
	cerr << "output-file(-)\tThe name of output file. (-): standard output" << endl;
	cerr << "test-k(1)\tK-best" << endl;
	cerr << "order(1)\tFirst-order(1) or second-order(2) graph-based model" << endl;
	cerr << "lang(en)\tLanguage. English(en); Chinese(cn)" << endl;
	cerr << "beamfeat(false)\tTo use third-order features (will be slow) " << endl;
	cerr << "labeled-parsing(false)\tLabeled/UnLabeled parsing" << endl;
	cerr << "flag-subtree(false)\tTo use subtree-based features" << endl;
	cerr << "flag-cluster(false)\tTo use cluster-based features" << endl;
	cerr << "max-sentlen(500)\tTo set the maximum length of the input sentence" << endl;
	cerr << "runtype(NONE)\tLBBASIC: Labeled basic parsing; LBST: Labeled basic+subtree parsing; LBFULL:all" << endl;
	cerr << "format(CONLL)\tFile format. CONLL for CONLL-format; SENT for sent-format(only for tagging)" << endl;
	cerr << "model-name\tTo set the parsing model file name" << endl;
	cerr << "segmodel-name\tTo set the segmentation model file name" << endl;
	cerr << "posmodel-name\tTo set the tagging model file name" << endl;
	cerr << "feat-file\tTo set a name for the temp feat file (only for training) " << endl;
	cerr << "help(false)\tPrint this help!!" << endl;
    //eval = false;
    //loss_type = "punc";
    //decode_type = "proj";
    //num_iters = 10;
    //trainK = 1;
    //flag_coarsepos =  true;
    //subtreefile = "res/en.ST";
    //clusterfile = "res/en.word.clusters";

}
