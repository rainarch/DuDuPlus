#ifndef TAGPARSER_H
#define TAGPARSER_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include "MyHashTable.h"
#include "SymOptions.h"
#include "FeatureVector.h"
#include "Parameters.h"

#include "POSPipe.h"
#include "TagDecoder.h"
#include "BaseWriter.h"
#include "Tagger.h"
#include "Parser.h"
#include "Segger.h"

using namespace std;

class TagParser{

public:
	Tagger *ptagger;
	Parser *pparser;
	Segger *psegger;
public:
	TagParser(Segger *psegger, Tagger *ptagger, Parser *pparser);
	void Test(string &testfile);
};

#endif
