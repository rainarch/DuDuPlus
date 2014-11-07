#include "MyHashTable.h"
//typedef map<string, long>::iterator def_hash_map_it;


MyHashTable::MyHashTable(int init_num){
	Init(init_num);
}
MyHashTable::MyHashTable(){
	Init(10000);
}
void MyHashTable::Init(int init_num){
    //mytable.resize(init_num);
    num_entries = 0;
    growth_stopped = false;

}

int MyHashTable::GetSize(){
	return mytable.size();
}
long MyHashTable::LookupIndex (string entry)   {

	def_hash_map_it it = mytable.find(entry);
	long ret = -1;
	if(it != mytable.end()){
		ret = it->second;
	}
	//cout << entry << endl;
	if (ret == -1 && !growth_stopped) {
	    ret = num_entries;
	    mytable[entry] = ret;
	    num_entries++;
	}
	return ret;
}
void MyHashTable::StopGrowth ()   {
	growth_stopped = true;
}

void MyHashTable::AllowGrowth ()    {
	growth_stopped = false;

}

bool MyHashTable::IsGrowthStopped ()    {
	return growth_stopped;
}
void MyHashTable::ToArray(vector<string> &keys, vector<long> &vals){
	def_hash_map_it it;
	keys.reserve(mytable.size());
	vals.reserve(mytable.size());
	for(it = mytable.begin(); it != mytable.end(); ++it){
		keys.push_back(it->first);
		vals.push_back(it->second);
	//	cout << it->first << "\n";
	}
}
void MyHashTable::FromArray(vector<string> &keys, vector<long> &vals){
	vector<string>::iterator itk = keys.begin();
	vector<long>::iterator itv = vals.begin();
	while(itk != keys.end()){
		mytable[*itk] = *itv;
		++itk;
		++itv;
	}
}

