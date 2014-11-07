#ifndef TYPECONVERT_H
#define TYPECONVERT_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>

using namespace std;

class TypeConvert{

public:
	const static int Str2Int(string s){
		int i = 0;
	    try {
	        i = boost::lexical_cast<int>(s);
    	}
    	catch( boost::bad_lexical_cast const& ) {
        	std::cout << "Error: input string was not valid " << s << std::endl;
    	}
		return i;

	};
	const static bool Str2Bool(string s){
		bool i = false;
       /* try {
            i = boost::lexical_cast<bool>(s);
        }
        catch( boost::bad_lexical_cast const& ) {
            std::cout << "Error: input string was not valid " << s << std::endl;
        }
       */ return i;

	}
};

#endif
