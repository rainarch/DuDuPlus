#include "POSWriter.h"

POSWriter::POSWriter(string filename){
	InitFile(filename);
	return;
}
POSWriter::~POSWriter(){
	CloseFile();
}
POSWriter::POSWriter(){
}

int POSWriter::InitFile(string filename){
	
    output_wd.open(filename.c_str());
    if(!output_wd.is_open()){
        cout << "Err: opening output file "<< filename << endl;
		return -1;
    }
 	return 0;
}

bool POSWriter::IsOpen(){
	return output_wd.is_open();
}

void POSWriter::CloseFile(){
	if(output_wd.is_open())
        output_wd.close();
}

void POSWriter::OutputTags(Instance &instance, vector<ResultItem> &d){
    string formated_text = "";
    for(int i = 0; i < d.size(); i++){
        String2POS(instance, d[i].parse_tree, formated_text);
        output_wd << formated_text << endl;
	//	cout << d[i].second << endl;
	//	cout << formated_text << endl;
    }
}

void POSWriter::String2POS(Instance &instance, string &tag, string &sc){
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

