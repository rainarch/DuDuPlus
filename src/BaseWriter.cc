#include "BaseWriter.h"

BaseWriter::BaseWriter(string &filename, string &format, string &type){
	Init();
    this->filename = filename;
    if(filename == "-")
        isstdIO = true;
    else
        isstdIO = false;
    if(format == "CONLL" || format == "SENT")
        this->format = format;
    if(type == "DP" || type == "POS" || type == "SEG")
        this->type = type;
	InitFile(filename);
	return;
}
BaseWriter::~BaseWriter(){
	CloseFile();
}
BaseWriter::BaseWriter(){
	Init();
}

void BaseWriter::Init(){
	filename = "-";
    isstdIO = true;
    format = "CONLL";
    type = "DP";
}

int BaseWriter::InitFile(string &filename){
	this->filename = filename;
	if(filename != "-"){
 	   output_wd.open(filename.c_str());
 	   if(!output_wd.is_open()){
 	       cout << "Err in BaseWriter: opening output file "<< filename << endl;
 	   	return -1;
 	   }
	}//else standard output
	this->filename = filename;
    if(filename == "-")
        isstdIO = true;
    else
        isstdIO = false;
 	return 0;
}

void BaseWriter::WriteBuf(string s){
	if(!isstdIO){
		output_wd << s << flush;
	}
	else
		cout << s << flush;
}

bool BaseWriter::IsOpen(){
	if(isstdIO)
		return true;
	return output_wd.is_open();
}

void BaseWriter::CloseFile(){
	if(output_wd.is_open())
        output_wd.close();
}
void BaseWriter::OutputInstance(Instance &instance, vector<ResultItem > &d, int K){
	if(type == "DP")
		OutputDP(instance, d, K);
	else if(type == "POS")
		OutputPOS(instance, d, K);
	else
		OutputSEG(instance, d, K);
}
void BaseWriter::OutputPOS(Instance &instance, vector<ResultItem > &d, int K){
    string formated_text = "";
    ostringstream oss (ostringstream::out);
    for(int i = 0; i < d.size(); i++){
		if(format == "CONLL")
        	String2POSCONLL(instance, d[i].parse_tree, formated_text);
		else
			String2POSSent(instance, d[i].parse_tree, formated_text);
		if(K > 1){
		oss << "#" << i << " "<< d[i].score << endl;
        WriteBuf(oss.str());
        oss.str("");
		}
        oss << formated_text << endl;
        WriteBuf(oss.str());
		oss.str("");
    //  cout << d[i].second << endl;
    //  cout << formated_text << endl;
    }
	if(K > 1){
        oss << "EOS" << endl;
        WriteBuf(oss.str());
        oss.str("");
        oss << "" << endl;
        WriteBuf(oss.str());
        oss.str("");
    }

}

void BaseWriter::String2POSCONLL(Instance &instance, string &tag, string &sc){
    vector<string> tag_items;
    boost::split(tag_items, tag, boost::is_any_of(" "));
    sc = "";
    ostringstream oss (ostringstream::out);
    for(int i = 0; i < tag_items.size()-1; i++) {
        instance.word_items.at(i+1).postag = tag_items[i].c_str();
        oss << (i+1) << "\t" << instance.word_items.at(i+1).form  << "\t" << "_" << "\t"<< instance.word_items.at(i+1).cpostag
                     << "\t" << instance.word_items.at(i+1).postag << "\n";
    }
    sc = oss.str();

}
void BaseWriter::String2POSSent(Instance &instance, string &tag, string &sc){
    vector<string> tag_items;
    boost::split(tag_items, tag, boost::is_any_of(" "));
    sc = "";
    ostringstream oss (ostringstream::out);
    for(int i = 0; i < tag_items.size()-1; i++) {
        instance.word_items.at(i+1).postag = tag_items[i].c_str();
        oss << instance.word_items.at(i+1).form  << "/" << instance.word_items.at(i+1).postag << " ";
    }
    sc = oss.str();

}


void BaseWriter::OutputSEG(Instance &instance, vector<ResultItem > &d, int K){
    string formated_text = "";
    ostringstream oss (ostringstream::out);
    for(int i = 0; i < d.size(); i++){
		//cerr << d[i].parse_tree << endl;
		if(format == "CONLL")
        	String2SEGCONLL(instance, d[i].parse_tree, formated_text);
		else
			String2SEGSent(instance, d[i].parse_tree, formated_text);
		if(K > 1){
        oss << "#" << i << " "<< d[i].score << endl;
        WriteBuf(oss.str());
        oss.str("");
        }

        oss << formated_text << endl;
        WriteBuf(oss.str());
		oss.str("");
    //  cout << d[i].second << endl;
    //  cout << formated_text << endl;
    }
	if(K > 1){
        oss << "EOS" << endl;
        WriteBuf(oss.str());
        oss.str("");
        oss << "" << endl;
        WriteBuf(oss.str());
        oss.str("");
    }

}

void BaseWriter::String2SEGCONLL(Instance &instance, string &tag, string &sc){
    vector<string> tag_items;
    boost::split(tag_items, tag, boost::is_any_of(" "));
    sc = "";
    ostringstream oss (ostringstream::out);
    for(int i = 0; i < tag_items.size()-1; i++) {
		instance.word_items.at(i+1).seqtag = tag_items.at(i);
        string chtag = instance.word_items.at(i+1).seqtag;
        if(i != 0 && (chtag == "S" || chtag == "B"))
            oss << " ";
        oss << instance.word_items.at(i+1).form;
    }
    sc = oss.str();
	//
	vector<string> new_words;
	boost::split(new_words, sc, boost::is_any_of(" "));
	ostringstream oss1 (ostringstream::out);
	for(int i = 0; i < new_words.size(); i++) {
        oss1 << (i+1) << "\t" << new_words.at(i) << "\n";
    }
	sc = oss1.str();


}
void BaseWriter::String2SEGSent(Instance &instance, string &tag, string &sc){
    vector<string> tag_items;
    boost::split(tag_items, tag, boost::is_any_of(" "));
    sc = "";
    ostringstream oss (ostringstream::out);
    for(int i = 0; i < tag_items.size()-1; i++) {
		instance.word_items.at(i+1).seqtag = tag_items.at(i);
        string chtag = instance.word_items.at(i+1).seqtag;
        if(i != 0 && (chtag == "S" || chtag == "B"))
            oss << " ";
        oss << instance.word_items.at(i+1).form;
		//cerr << instance.word_items.at(i+1).form << chtag << endl;
        //oss << (i+1) << "\t" << instance.word_items.at(i+1).form  << "\t" << instance.word_items.at(i+1).seqtag  << "\n";
    }
    sc = oss.str();

}



void BaseWriter::OutputDP(Instance &instance, vector<ResultItem > &d, int K){
    string formated_text = "";
    ostringstream oss (ostringstream::out);
	
    for(int i = 0; i < d.size(); i++){
        String2DPCONLL(instance, d[i].parse_tree, formated_text);
        if(K > 1){
        //if(i > 0){
        //    oss << "#" << i << endl;
        //    WriteBuf(oss.str());
		//	oss.str("");
        //}
        oss << "#" << i << " "<< d[i].score << endl;
        WriteBuf(oss.str());
		oss.str("");
        }

        oss << formated_text << endl;
        WriteBuf(oss.str());
		oss.str("");
    }
    if(K > 1){
        oss << "EOS" << endl;
        WriteBuf(oss.str());
		oss.str("");
        oss << "" << endl;
        WriteBuf(oss.str());
		oss.str("");
    }
}
void BaseWriter::String2DPCONLL(Instance &instance, string &tag, string &sc){
    vector<string> tag_items;
    boost::split(tag_items, tag, boost::is_any_of(" "));
    sc = "";
    ostringstream oss (ostringstream::out);
    for(int i = 0; i < tag_items.size(); i++) {
        vector<string> p1;
        boost::split(p1, tag_items.at(i), boost::is_any_of("|:"));
        instance.word_items.at(i+1).head = atoi(p1.at(0).c_str());
        instance.word_items.at(i+1).deprel = p1.at(2);
        oss << (i+1) << "\t" << instance.word_items.at(i+1).form  << "\t" << "_" << "\t"<< instance.word_items.at(i+1).postag
                     << "\t" << instance.word_items.at(i+1).postag<< "\t" << "_" << "\t"<< instance.word_items.at(i+1).head
                     << "\t" << instance.word_items.at(i+1).deprel << endl;
    }
    sc = oss.str();

}

