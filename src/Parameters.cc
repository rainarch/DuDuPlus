#include "Parameters.h"
Parameters::Parameters(){
//	parameters = NULL;
//	total = NULL;
}


void Parameters::Init(int size){
	parameters.reserve(size);
	total.reserve(size);
//	parameters = new double[size];
//	total = new double[size];
	loss_type = "INIT";
	params_size = size;
	for(int i = 0; i < size; i++){
		parameters.push_back(0.0);
		total.push_back(0.0);
	}
}

Parameters::~Parameters(){
//	if(parameters != NULL)
//	delete parameters;
//	if(total != NULL)
//	delete total;
}

void Parameters::SetLoss(string lt) {
	loss_type = lt;
}

void Parameters::AverageParams(double av) {
	//cout << total[0] <<  " vs  "<< parameters[0] << endl;
	for(int j = 0; j < params_size; j++){
	    total[j] *= (1.0/((double)av));		
	//	if(j == 0)
	//		cout << total[j] << " " << av << endl;
		parameters[j] = total[j];
	}
	//cout << total[0] <<  " vs  "<< parameters[0] << endl;
}
	
double Parameters::GetNumErrors( string tag, string gold){
    double err;
    vector<string> tag_items;
    vector<string> gold_items;
    boost::split(tag_items, tag, boost::is_any_of(" "));
    boost::split(gold_items, gold, boost::is_any_of(" "));

    int correct = 0;
    int num = 0;
    for(int i = 0; i < tag_items.size(); i++) {
		num++;
		if(tag_items[i] == gold_items[i])
			correct++;
    }
    err = num -  correct;
    if(false){
    cout << "===" << endl;    cout << gold << endl;    cout << tag << endl;    cout << err << endl;
    }
  return err;
}

