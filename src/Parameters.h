#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
//#include <boost/archive/binary_oarchive.hpp>
//#include <boost/archive/binary_iarchive.hpp>
//#include <boost/serialization/vector.hpp>

using namespace std;

class Parameters{

public:
    vector<double> parameters;
    vector<double> total;
	int    params_size;
    string loss_type;

public:
	void Init(int size);
	Parameters();
	~Parameters();

	void SetLoss(string lt);
	
	void AverageParams(double av);

	//
	double GetNumErrors(string tag, string gold);
	//define other error functions
	

};

#endif
