#ifndef WORDITEM_H
#define WORDITEM_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>

using namespace std;

class WordItem{

public:
	string form;
	string lemma;
	//coase tag
	string cpostag;
	//fine-grained tag
	string postag;
	int    head;
	string deprel;
	//cluster
	string cF;
	string c4;
	string c6;
	string cW;//only top N
	//seg
	string chcode;
	//for sequence tagging/ pos/seg
	string seqtag;//
	
public:
	WordItem();

	WordItem(string line, string delim);
	WordItem(string line, string delim, int);
};

#endif
