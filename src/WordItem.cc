#include "WordItem.h"

WordItem::WordItem(){
	head = -1;
	seqtag = "ST";
}
//for parsing
WordItem::WordItem(string line, string delim){
	seqtag = "ST";
	vector<string> strs;
	boost::split(strs, line, boost::is_any_of(delim));
	if(strs.size() < 5){
		std::cerr << "Error: input string was not valid (Should be CoNLL format): "<< line << std::endl;
		return;
	}
	form = strs.at(1);
	lemma = strs.at(2);
	cpostag = strs.at(3);
	postag = strs.at(4);
	deprel = "_";
	head = -1;
	if(postag.length() > 1 && cpostag == postag && (postag.compare("PRP") != 0 && postag.compare("PRP$") != 0)){
		cpostag = postag.substr(0,2);
	}
	if(strs.size() < 7)
		return;//
	//
	try {
    	head = boost::lexical_cast<int>(strs.at(6));
	}
	catch( boost::bad_lexical_cast const& ) {
    	std::cerr << "Error: input string was not valid (head should a number): "<< line << std::endl;
	}
	if(strs.size() < 8)
        return;//
	deprel = strs.at(7);
}
//POS
WordItem::WordItem(string line, string delim, int t){
	seqtag = "ST";
    vector<string> strs;
    boost::split(strs, line, boost::is_any_of(delim));
    if(strs.size() < 2){
		std::cerr << "Error: input string was not valid : "<< line << std::endl;
        return;
    }
    form = strs.at(1);
	if(strs.size() < 4)
		return;
    lemma = strs.at(2);
    cpostag = strs.at(3);
	postag = strs.at(3);
	deprel = "_";
	head = -1;

}

