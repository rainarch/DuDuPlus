#include "CONLLWriter.h"

CONLLWriter::CONLLWriter(string filename){
	InitFile(filename);
	return;
}
CONLLWriter::~CONLLWriter(){
	CloseFile();
}
CONLLWriter::CONLLWriter(){
}

int CONLLWriter::InitFile(string filename){
	
    output_wd.open(filename.c_str());
    if(!output_wd.is_open()){
        cout << "Err: opening output file "<< filename << endl;
		return -1;
    }
 	return 0;
}

bool CONLLWriter::IsOpen(){
	return output_wd.is_open();
}

void CONLLWriter::CloseFile(){
	if(output_wd.is_open())
        output_wd.close();
}

void CONLLWriter::OutputParseTree(Instance &instance, vector<ResultItem > &d, int K){
    string formated_text = "";
    for(int i = 0; i < d.size(); i++){
        String2CONLL(instance, d[i].parse_tree, formated_text);
		if(K > 1){
		if(i > 0)
			output_wd << "#" << i << endl;
		output_wd << "#" << d[i].score << endl;
		}
        output_wd << formated_text << endl;
    }
	if(K > 1){
		output_wd << "EOS" << endl;
		output_wd << "" << endl;
	}
}

void CONLLWriter::String2CONLL(Instance &instance, string &tag, string &sc){
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

