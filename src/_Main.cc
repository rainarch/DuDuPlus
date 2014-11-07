#include <stdio.h>
#include "WordItem.h"
#include "Instance.h"
#include "SymOptions.h"
#include "Pipe.h"
#include "PipeO2.h"
#include "PipeFull.h"
#include "Parser.h"
#include "POSPipe.h"
#include "Tagger.h"
#include "TagParser.h"
#include "SegPipe.h"
#include "Segger.h"
void PrintHeadInfo(){
	cerr << "=======================================================" << endl;
	cerr << "DuDuPlus: A dependency parser! " << endl;
	cerr << "Copyright(C) 2009-2012 Wenliang, All rights reserved." << endl;
	cerr << "======================================================" << endl;
}
/*
Main
*/
int TagParseMain(SymOptions &options){
	if(!options.test){
		cerr << "Task: TagParse only works for testing." << endl;
		return -1;
	}
    if(options.test){//test a model

        cerr << "Start to test ... loading the model:POS and DP " << endl;
        POSPipe pospipe(&options);
        Tagger tagger(&pospipe, &options);
        if(tagger.LoadModel(options.posmodel_name) < 0)
            return -1;
		cerr << "POS Model Done!" << endl;
		//tagger.Test(options.testfile);
		//return 0;
		Pipe *ppipe;
        if(options.beamfeat){
            ppipe = new PipeFull();
        }
        else if(options.order == 2){
            ppipe = new PipeO2();
        }
        else{//==1
            ppipe = new Pipe();
        }

        ppipe->Init(&options);
        //pipe.CreateInstances(options.trainfile, options.featfile);
        Parser dp(ppipe, &options);
        if(dp.LoadModel(options.model_name) < 0)
            return -1;

		SegPipe segpipe(&options);
        //pipe.CreateInstances(options.trainfile, options.featfile);
        Segger segger(&segpipe, &options);
		if(options.lang == "cn"){
        if(segger.LoadModel(options.segmodel_name) < 0)
            return -1;
        }	

		TagParser tagparser(&segger, &tagger, &dp);

        cerr << "DP Model Done!" << endl;
        cerr << "Start to test the trained model ..." << endl;
        tagparser.Test(options.testfile);
        cerr << "Done!" << endl;
        cerr << "End of test!" << endl;

    }
	return 0;
}
int TagMain(SymOptions &options){
    if(options.train){//train a model

        cerr << "Start to train a model ..." << endl;
        POSPipe pipe(&options);
		if(options.usefeat)
			pipe.CreateAlphabet(options.trainfile);
		else
        	pipe.CreateInstances(options.trainfile, options.featfile);

        Tagger tagger(&pipe, &options);


        tagger.Train(options.trainfile);
        cerr << "Saving model ...";
        tagger.SaveModel(options.posmodel_name);

        cerr << "Done!" << endl;
        cerr << "End of training!" << endl;
        //dp.Test(options.testfile);
        //return 0 ;

    }
    if(options.test){//test a model

        cerr << "Start to test ... loading the model" << endl;
        POSPipe pipe(&options);
        //pipe.CreateInstances(options.trainfile, options.featfile);
        Tagger tagger(&pipe, &options);
        if(tagger.LoadModel(options.posmodel_name) < 0)
            return -1;
        cerr << "Done!" << endl;
        cerr << "Start to test the trained model ..." << endl;
        tagger.Test(options.testfile);
        cerr << "Done!" << endl;
        cerr << "End of test!" << endl;

    }
}

int SegMain(SymOptions &options){
    if(options.train){//train a model

        cerr << "Start to train a model ..." << endl;
        SegPipe pipe(&options);
		if(options.usefeat)
			pipe.CreateAlphabet(options.trainfile);
		else
        	pipe.CreateInstances(options.trainfile, options.featfile);

        Segger segger(&pipe, &options);


        segger.Train(options.trainfile);
        cerr << "Saving model ...";
        segger.SaveModel(options.segmodel_name);

        cerr << "Done!" << endl;
        cerr << "End of training!" << endl;

    }
    if(options.test){//test a model

        cerr << "Start to test ... loading the model" << endl;
        SegPipe pipe(&options);
        //pipe.CreateInstances(options.trainfile, options.featfile);
        Segger segger(&pipe, &options);
        if(segger.LoadModel(options.segmodel_name) < 0)
            return -1;
        cerr << "Done!" << endl;
        cerr << "Start to test the trained model ..." << endl;
        segger.Test(options.testfile);
        cerr << "Done!" << endl;
        cerr << "End of test!" << endl;

    }
}


int ParseMain(SymOptions &options){
    if(options.train){//train a model

        cerr << "Start to train a model ..." << endl;
		
        Pipe *ppipe;
		if(options.beamfeat){
			ppipe = new PipeFull();
		}
		else if(options.order == 2){
			ppipe = new PipeO2();
		}
		else{//==1
			ppipe = new Pipe();
		}
		//ppipe->TestIdx();
		//return 0;
		ppipe->Init(&options);
        if(options.usefeat)
            ppipe->CreateAlphabet(options.trainfile);
        else
            ppipe->CreateInstances(options.trainfile, options.featfile);


        Parser dp(ppipe, &options);

        dp.Train(options.trainfile);
        cerr << "Saving model ...";
        dp.SaveModel(options.model_name);

        cerr << "Done!" << endl;
        cerr << "End of training!" << endl;
		delete ppipe;
        //dp.Test(options.testfile);
        //return 0 ;

    }
    if(options.test){//test a model

        cerr << "Start to test ... loading the model" << endl;
		Pipe *ppipe;
		if(options.beamfeat){
            ppipe = new PipeFull();
        }
        else if(options.order == 2){
            ppipe = new PipeO2();
        }
        else{//==1
            ppipe = new Pipe();
        }

        ppipe->Init(&options);
        //pipe.CreateInstances(options.trainfile, options.featfile);
        Parser dp(ppipe, &options);
        if(dp.LoadModel(options.model_name) < 0)
            return -1;
        cerr << "Done!" << endl;
        cerr << "Start to test the trained model ..." << endl;
        dp.Test(options.testfile);
        cerr << "Done!" << endl;
        cerr << "End of test!" << endl;
		delete ppipe;
    }

}

int main(int argc, char* argv[]) {
	PrintHeadInfo();
	

	SymOptions options(argc, argv);
	if(false){
	options.task = "Tagger";
	options.train = false;
	options.test = true;
	options.posmodel_name = "_Models/POSModel.sec02-21.New";
	options.flag_subtree = false;
	options.trainfile = "ENdata/small.train";
	options.testfile = "ENdata/sec22-24.conll.dep";
	//options.trainfile = "data/train5k.dep";
	//options.testfile = "../dataEN/en.test.autotagMIRA.conll.dep";
	options.featfile = "tmp/feat.dp";
	options.outfile = "tmp/out.result";
	//options.order = 1;
	}
	
	if(false){
    options.task = "Parser";
    options.train = false;
    options.test = true;
    options.model_name = "_Models/DPModel.EN.O1Basic.New";
    options.flag_subtree = false;
    options.trainfile = "ENdata/small.train";
    options.testfile = "../dataEN/en.test.autotagMIRA.conll.dep";
    //options.trainfile = "data/train5k.dep";
    //options.testfile = "../dataEN/en.test.autotagMIRA.conll.dep";
    options.featfile = "tmp/feat.dp";
    options.outfile = "tmp/out.result";
	
    //options.order = 1;
    }
	
	//options.trainK = 3;
	//options.PrintOptions();
	if(options.task == "TagParser"){//only for test
        return TagParseMain(options);
    }
    else if(options.task == "Tagger"){
		return TagMain(options);
	}
	else if(options.task == "Parser"){
		return ParseMain(options);
	}
	else if(options.task == "Seg"){
        return SegMain(options);
    }
	else{
		cerr << "No matched task! Should be TagParser/Tagger/Parser/Seg. Please check " << endl;
	}

	
	return(0);
}

