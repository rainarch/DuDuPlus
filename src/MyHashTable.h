#ifndef MYHASHTABLE_H
#define MYHASHTABLE_H

#include <string>
#include <vector>
#include <iostream>
//#include <boost/archive/binary_oarchive.hpp>
//#include <boost/archive/binary_iarchive.hpp>

#include <unordered_map> 
//#include <boost/serialization/unordered_map.hpp>

#include <map>
//#include <boost/serialization/map.hpp>

using namespace std;
//typedef google::sparse_hash_map<string, long> def_hash_map;
//typedef google::sparse_hash_map<string, long>::iterator def_hash_map_it;
//typedef map<string, long> def_hash_map;
//typedef map<string, long>::iterator def_hash_map_it;
typedef unordered_map<string, long> def_hash_map;
typedef unordered_map<string, long>::iterator def_hash_map_it;

class MyHashTable{

public:
	def_hash_map mytable;
    int num_entries;
    bool growth_stopped;
	
public:
	MyHashTable();
	MyHashTable(int init_num);
	void Init(int init_num);
	long LookupIndex(string );
	void StopGrowth();	
	void AllowGrowth();
	bool IsGrowthStopped();
	int  GetSize();
	void ToArray(vector<string> &, vector<long> &);
	void FromArray(vector<string> &, vector<long> &);
};

#endif
