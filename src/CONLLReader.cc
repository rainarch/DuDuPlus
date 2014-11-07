#include "CONLLReader.h"

CONLLReader::CONLLReader(string filename){
	InitFile(filename);
	return;
}
CONLLReader::~CONLLReader(){
	CloseFile();
}
CONLLReader::CONLLReader(){
}

int CONLLReader::InitFile(string filename){
	
    input_rd.open(filename.c_str());
    if(!input_rd.is_open()){
        cout << "Err: opening "<< filename << endl;
		return -1;
    }
 	return 0;
}

void CONLLReader::CloseFile(){
	if(input_rd.is_open())
        input_rd.close();
}

void CONLLReader::GetNext(Instance &inst){
	inst.Clear();
	vector<string> lines;
	string line;
	
	while ( input_rd.good() )   {
      	getline (input_rd,line);
		if( line.length() == 0)
			break;
      	//cout << line << endl;
		
		lines.push_back(line);
    }
	int length = lines.size();
	if(length == 0)
		return;
	//set the ROOT 
	WordItem word_item("0\t_root\t_root\t_rootpos\t_rootcpos\tnull\t-1\t_rootrel", "\t");
	inst.word_items.push_back(word_item);
	for(int i = 0; i < length; i++){
		WordItem word_item1(lines.at(i), "\t");
		
		inst.word_items.push_back(word_item1);
	}
	inst.word_num = inst.word_items.size();
}
