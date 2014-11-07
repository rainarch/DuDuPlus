#include "MyDict.h"



void MyDict::Load(string &lang){
	//LoadWordPosList();
	//LoadPWHList();
	string fn = (string)"res/"+lang+".word.freqs";
	LoadDictList(fn, wordfreq_list, 1000);
	if(lang == "cn"){
		fn = (string)"res/"+lang+".PU";
    	LoadDictList(fn, PU_list, -1);
	}
}
	

void MyDict::LoadWordPosList(){
	ifstream input_rd;
	string fn = "res/wordpos.list";
    input_rd.open(fn.c_str(), ios::binary);
    if(!input_rd.is_open()){
        cerr << "Err: opening wordposlistFile: "<< fn << endl;
        //return -1;
    }
	string line;
	while ( input_rd.good() )   {
        getline (input_rd,line);
        if( line.length() == 0)
            break;
        //cerr << line << endl;
		vector<string> strs;
	    boost::split(strs, line, boost::is_any_of("\t"));
		if(strs.size() != 2)
			continue;
		//cerr << strs[0] << "==" << strs[1] << endl;
 		wordpos_list[strs[0]] = strs[1];       
    }

	input_rd.close();
}

int MyDict::LoadDictList(string &fn, unordered_map<string, string> &dict_list, int firstN){
    ifstream input_rd;
    input_rd.open(fn.c_str(), ios::binary);
    if(!input_rd.is_open()){
        cerr << "Err: opening DictFile: "<< fn << endl;
        return -1;
    }
	cerr << "Info:Loading dictfile:" << fn << endl;
    string line;
	dict_list.clear();
	int n = 0;
    while ( input_rd.good() )   {
        getline (input_rd,line);
        if( line.length() == 0)
            continue;
        //cerr << line << endl;
        vector<string> strs;
        boost::split(strs, line, boost::is_any_of("\t"));
        if(strs.size() != 2)
            continue;
	//	if(n < 4)
      //  cerr << strs[0] << "==" << strs[1] << endl;
        dict_list[strs[0]] = strs[1];
		n++;
		if(n % 100000 == 0){
			cerr <<"\rN=" << n << " Loading<-" << flush;
		}
		if(firstN != -1 && n > firstN)
			break;
		
    }

    input_rd.close();
	cerr << n << " Loaded ... Done" << endl;
	return 0;
}
string MyDict::GetDictItemVal(unordered_map<string, string> &dict_list, string &item, string &val){
	unordered_map<string, string>::iterator it = dict_list.find(item);
	if(it == dict_list.end())
		val = "O";
	else
		val = it->second;
	//cerr << item << " " << val<< endl;
	return val;
}
bool MyDict::IsInList(unordered_map<string, string> &dict_list, string &item){
	unordered_map<string, string>::iterator it = dict_list.find(item);
	bool val = true;
    if(it == dict_list.end())
        val = false;
	//cerr << item << "\t" << val << endl;
    return val;
}
void MyDict::LoadPWHList(){
    ifstream input_rd;
    string fn = "res/poswordhead.list";
    input_rd.open(fn.c_str(), ios::binary);
    if(!input_rd.is_open()){
        cerr << "Err: opening poswordheadlistFile: "<< fn << endl;
        //return -1;
    }
    string line;
    while ( input_rd.good() )   {
        getline (input_rd,line);
        if( line.length() == 0)
            break;
        //cerr << line << endl;
        vector<string> strs;
        boost::split(strs, line, boost::is_any_of("\t"));
        if(strs.size() != 2)
            continue;
        //cerr << strs[0] << "==" << strs[1] << endl;
		long freq = 0;
		try {
        freq = boost::lexical_cast<long>(strs[1]);
	    }
    	catch( boost::bad_lexical_cast const& ) {
        	std::cerr << "Error: input string was not valid "<< line << std::endl;
    	}

        poswordhead_list[strs[0]] = freq;
    }

    input_rd.close();
}

