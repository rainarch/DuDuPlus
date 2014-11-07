#include "Heap.h"
//**************
//class Heap
//**************
Heap::Heap(int size){
	items.reserve(2*size);
	cur_idx = 0;
}

void Heap::Add(HeapItem &newitem){
	if(items.size() == 0){
		items.push_back(newitem);
		return;
	}
	int i = 0;
	for(i = items.size()-1; i >= 0; --i){
		if(items.at(i).CompareTo(newitem) >= 0)
			break;
	}
	i++;
	items.insert(items.begin()+i, newitem);
}

void Heap::RemoveMax(HeapItem &maxitem){
	maxitem.i1 = -1;
	maxitem.i2 = -1;
	
	if(items.size() > 0 && cur_idx < items.size()){
		items.at(cur_idx).CopyTo(maxitem);
		//items.erase(items.begin());
		cur_idx++;
	}
}

