#include "BaseReader.h"
#include "MyTool.h"

BaseReader::BaseReader(string &filename, string &format, string &type){
	Init();
	SetIO(filename);
	SetFormat(format);
	SetType(type);
	return;
}

BaseReader::~BaseReader(){
	CloseFile();
}
BaseReader::BaseReader(){
	Init();
}

void BaseReader::Init(){
	filename = "-";
    isstdIO = true;
    format = "CONLL";
    type = "DP";
}
void BaseReader::SetIO(string filename){
	this->filename = filename;
    if(filename == "-")
        isstdIO = true;
    else
        isstdIO = false;
}
void BaseReader::SetFormat(string format){
	if(format == "CONLL" || format == "SENT")
        this->format = format;
}
void BaseReader::SetType(string type){
	if(type == "DP" || type == "POS" || type == "SEG")
        this->type = type;
}

int BaseReader::InitFile(string &filename){
	if(filename == "-"){
	//input_rd = NULL;//for standard input
	}
	else{
    input_rd.open(filename.c_str());
    if(!input_rd.is_open()){
        cout << "Err in BaseReader: opening file "<< filename << endl;
		return -1;
    }
	}
	SetIO(filename);
 	return 0;
}
int BaseReader::ReadNextLine(string &line){
	//cout << "Reading..." << endl;
	if(isstdIO){
       getline(cin, line);
    }
    else{
      if(!input_rd.good())
         return -1;
      getline (input_rd,line);
    }
	return 0;
}

void BaseReader::CloseFile(){
	if(input_rd.is_open())
        input_rd.close();
}
void BaseReader::GetNext(Instance &inst){
	if(format == "CONLL")
		GetNextCONLL(inst);
	else
		GetNextSent(inst);
}

void BaseReader::GetNextSent(Instance &inst){
    inst.Clear();
    string line;

    while ( true )   {
        if(ReadNextLine(line) < 0)
            break;
        if( line.length() > 0)
            break;
    }
    if(line.length() == 0)
        return;
    //One sent
    //cout << line << endl;
	if(type == "DP")
        GenDPInstance(line, inst);
    else if(type == "POS")
        GenPOSInstance(line, inst);
    else if(type == "SEG")
        GenSEGInstance(line, inst);
}

void BaseReader::GenPOSInstance(string &line, Instance &inst){
    vector<string> words;
    boost::split(words, line, boost::is_any_of(string(" ")));
    //set the ROOT
    WordItem word_item("0\tWSTART\tWSTART\tTSTART\tTSTART", "\t",1);
	word_item.seqtag = word_item.postag;
    inst.word_items.push_back(word_item);
    int length = words.size();
    for(int i = 0; i < length; i++){
        //cout << "i:" << words.at(i) << endl;
        if(words.at(i) == "")
            continue;
        string s = "";
        WordItem word_item1;
        word_item1.form = words.at(i);
        word_item1.cpostag = "UKN";
        word_item1.postag = "UKN";
        //WordType(word_item1.form, word_item1.lemma);
        inst.word_items.push_back(word_item1);
    }
    WordItem word_itemE("0\tWEND\tWEND\tTEND\tTEND", "\t", 1);
    inst.word_items.push_back(word_itemE);
    inst.word_num = inst.word_items.size();
    //cout << "Size=" << inst.word_items.size() << endl;
}
void BaseReader::GenDPInstance(string &line, Instance &inst){
	cerr << "Error in BaseReader: DP parser has to take the CONLL input" << endl;
}
void BaseReader::GenSEGInstance(string &line, Instance &inst){
	vector<string> vc;
    MyTool::SegSent2CharVect(line, vc);
    //set the ROOT
    WordItem word_item("0\tWSTART\tWSTART\tTSTART\tTSTART", "\t",1);
	word_item.seqtag = "TSTART";
    inst.word_items.push_back(word_item);
    int length = vc.size();
    for(int i = 0; i < length; i++){
        //cout << "i:" << words.at(i) << endl;
        if(vc.at(i) == "")
            continue;
        WordItem word_item1;
        word_item1.form = vc.at(i);
        word_item1.seqtag = "UKN";
        inst.word_items.push_back(word_item1);
    }
    WordItem word_itemE("0\tWEND\tWEND\tTEND\tTEND", "\t", 1);
    inst.word_items.push_back(word_itemE);
    inst.word_num = inst.word_items.size();
}
void BaseReader::GetNextCONLL(Instance &inst){
    inst.Clear();
    vector<string> lines;
    string line;

    while ( true )   {
        if(ReadNextLine(line) < 0)
            break;
        if( line.length() == 0)
            break;
 //         cout << line << endl;

        lines.push_back(line);
    }
    int length = lines.size();
    if(length == 0)
        return;
	if(type == "DP")
		GenDPInstance(lines, inst);
	else if(type == "POS")
		GenPOSInstance(lines, inst);
	else if(type == "SEG")
		GenSEGInstance(lines, inst);

		
}

void BaseReader::GenDPInstance(vector<string> &lines, Instance &inst){
	//set the ROOT
    WordItem word_item("0\t_root\t_root\t_rootpos\t_rootcpos\tnull\t-1\t_rootrel", "\t");
    inst.word_items.push_back(word_item);
	int length = lines.size();
    for(int i = 0; i < length; i++){
        WordItem word_item1(lines.at(i), "\t");

        inst.word_items.push_back(word_item1);
    }
    inst.word_num = inst.word_items.size();

}

void BaseReader::GenPOSInstance(vector<string> &lines, Instance &inst){
	WordItem word_item("0\tWSTART\tWSTART\tTSTART\tTSTART", "\t",1);
	word_item.seqtag = word_item.postag;
    inst.word_items.push_back(word_item);
	int length = lines.size();
    for(int i = 0; i < length; i++){
        WordItem word_item1(lines.at(i), "\t", 1);
        if(word_item1.form == ""){
            word_item1.form = "=EMPTY";//input has an empty word, to keep the number of words unchanged
        }
		word_item1.seqtag = word_item1.postag;
		//move to Preprocessing
        //WordType(word_item1.form, word_item1.lemma);
        inst.word_items.push_back(word_item1);
    }
    WordItem word_itemE("0\tWEND\tWEND\tTEND\tTEND", "\t", 1);
	word_itemE.seqtag = word_itemE.postag;
    inst.word_items.push_back(word_itemE);
    inst.word_num = inst.word_items.size();
}

void BaseReader::GenSEGInstance(vector<string> &lines, Instance &inst){
	//set the ROOT
    WordItem word_item("0\tWSTART\tWSTART\tTSTART\tTSTART", "\t",1);
    inst.word_items.push_back(word_item);
	word_item.seqtag = "TSTART";
	int length = lines.size();
    for(int i = 0; i < length; i++){
        WordItem word_item1(lines.at(i), "\t", 1);
        if(word_item1.form == ""){
            word_item1.form = "=EMPTY";//input has an empty word, to keep the number of words unchanged
        }
        //seg word into char
        vector<string> vc;
        MyTool::SegSent2CharVect(word_item1.form, vc);
        for(int j = 0; j < vc.size(); j++){
            if(vc[j] == "") continue;
            WordItem ch_item;
            ch_item.form = vc[j];
            if(vc.size() == 1)
                ch_item.seqtag = "S";
            else if(j == 0)
                ch_item.seqtag = "B";
            else if(j == (vc.size()-1))
                ch_item.seqtag = "E";
            //else if(j == 1)
            //    ch_item.seqtag = "B2";
            //else if(j == 2)
            //    ch_item.seqtag = "B3";
            else
                ch_item.seqtag = "M";
        //  cout << ch_item.form << ch_item.seqtag <<  endl;
            inst.word_items.push_back(ch_item);
        }

    }
    WordItem word_itemE("0\tWEND\tWEND\tTEND\tTEND", "\t", 1);
    word_itemE.seqtag = "TEND";
    inst.word_items.push_back(word_itemE);
    inst.word_num = inst.word_items.size();
}
