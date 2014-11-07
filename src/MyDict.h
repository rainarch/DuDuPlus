#ifndef MYDICT_H
#define MYDICT_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include "MyHashTable.h"
#include "SymOptions.h"
#include <unordered_map>
using namespace std;

class MyDict{
public:


public:
	unordered_map<string, string> wordpos_list;
	unordered_map<string, string> wordfreq_list;
	unordered_map<string, string> PU_list;
	unordered_map<string, long> poswordhead_list;

public:
	void Load(string &lang);
	void LoadWordPosList();
	void LoadPWHList();
	int LoadDictList(string &fn, unordered_map<string, string> &dict_list, int firstN = -1);
	string GetDictItemVal(unordered_map<string, string> &dict_list, string &item, string &val);
	bool IsInList(unordered_map<string, string> &dict_list, string &item);
};

#endif
