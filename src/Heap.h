#ifndef HEAP_H
#define HEAP_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>

using namespace std;

class HeapItem{
public:
	int i1;
	int i2;
	double val;
	HeapItem(double val, int i1, int i2) {
	this->val = val;
	this->i1 = i1;
	this->i2 = i2;
    }

    int CompareTo(HeapItem &hi) {
	if(val < hi.val)
	    return -1;
	if(val > hi.val)
	    return 1;
	return 0;
    }
	int CopyTo(HeapItem &newhi){
		newhi.val = val;
		newhi.i1 = i1;
		newhi.i2 = i2;
	}
};

class Heap{
	int cur_idx;
public:
	vector<HeapItem> items;
	Heap(int size);
	void Add(HeapItem &newitem);
	void RemoveMax(HeapItem &maxitem);
};
#endif
