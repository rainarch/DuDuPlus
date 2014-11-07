#include "Instance.h"

Instance::Instance(){
}
Instance::~Instance(){
	Clear();
}
void Instance::Clear(){
	act_parsetree = "";
	act_fv.Clear();
	//release
	word_items.clear();
}

void Instance::PrintInst(){
	for(int i = 0; i < word_items.size(); i++){
		cout << i << " " << word_items.at(i).form << endl;
	}
}
