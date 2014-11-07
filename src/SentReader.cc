#include "SentReader.h"

SentReader::SentReader(string filename){
	InitFile(filename);
	return;
}
SentReader::~SentReader(){
	CloseFile();
}
SentReader::SentReader(){
}

int SentReader::InitFile(string filename){
	
    input_rd.open(filename.c_str());
    if(!input_rd.is_open()){
        cout << "Err: opening ["<< filename << "]"<< endl;
		return -1;
    }
 	return 0;
}

void SentReader::CloseFile(){
	if(input_rd.is_open())
        input_rd.close();
}

void SentReader::GetNext(Instance &inst){
	inst.Clear();
	string line;
	
	while ( input_rd.good() )   {
      	getline (input_rd,line);
		if( line.length() > 0)
			break;
    }
	if(line.length() == 0)
		return;
	//One sent
	vector<string> words;
	boost::split(words, line, boost::is_any_of(string(" ")));
	//set the ROOT 
	WordItem word_item("0\tWSTART\tWSTART\tTSTART\tTSTART", "\t",1);
	inst.word_items.push_back(word_item);
	int length = words.size();
	for(int i = 0; i < length; i++){
		string s = "";
		WordItem word_item1;
		
		word_item1.form = words.at(i);
		WordType(word_item1.form, word_item1.lemma);
		inst.word_items.push_back(word_item1);
	}
	WordItem word_itemE("0\tWEND\tWEND\tTEND\tTEND", "\t", 1);
	inst.word_items.push_back(word_itemE);
	inst.word_num = inst.word_items.size();
	//cout << "Size=" << inst.word_items.size() << endl;
}
void SentReader::WordType(string &w0, string &type){
    //contains number/UPPERCASE/ hyphen

    if(w0.at(0) <= 'Z' && w0.at(0) >= 'A'){
        type = "TAZ";
    }
    else if(w0.at(0) <= '9' && w0.at(0) >= '0'){
        type = "T09";
    }
    else if(w0.find("-") != string::npos){
        type = "THP";
    }
    else{
        type = "T0";
    }
	//cout << w0 << "\t" << type << endl;
}
