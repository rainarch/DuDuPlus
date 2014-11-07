#ifndef MYTOOL_H
#define MYTOOL_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

class MyTool{
public:


public:

public:
	static void VectorRead(ifstream &model_rd, vector<double> & v){
	    int psize = 0;
	    model_rd.read((char*)&psize, sizeof(int));
	    double *p = new double[psize];
	    model_rd.read((char*)p, sizeof(double)*psize);
	    v.clear();
	    v.reserve(psize);
	    //cout << "Rsize:" << psize << endl;
	    for(int i = 0; i < psize; i++){
	        v.push_back(p[i]);
	      //  if(i < 5)
	        //    cout << "RD:"<< p[i] << endl;
	    }
	    delete p;
	
	};
	static void VectorWrite(ofstream &model_wd, vector<double> & v){
		int psize = v.size();
		model_wd.write((char*)&psize, sizeof(int));
	    double *p = new double[psize];
	    for(int i = 0; i < psize; i++){
	        p[i] = v[i];
	        //if(i < 5)
	          //  cout << p[i] << endl;
	    }
	    model_wd.write((char*)p, sizeof(double)*psize);
	    delete p;
	};
	static void VectorRead(ifstream &model_rd, vector<long> & v){
	    int psize = 0;
	    model_rd.read((char*)&psize, sizeof(int));
	    long *p = new long[psize];
	    model_rd.read((char*)p, sizeof(long)*psize);
	    v.clear();
	    v.reserve(psize);
	    //cout << "Rsize:" << psize << endl;
	    for(int i = 0; i < psize; i++){
	        v.push_back(p[i]);
	      //  if(i < 5)
	        //    cout << "RD:"<< p[i] << endl;
	    }
	    delete p;
	
    };
	static void VectorWrite(ofstream &model_wd, vector<long> & v){
        int psize = v.size();
        model_wd.write((char*)&psize, sizeof(int));
        long *p = new long[psize];
        for(int i = 0; i < psize; i++){
            p[i] = v[i];
            //if(i < 5)
              //  cout << p[i] << endl;
        }
        model_wd.write((char*)p, sizeof(long)*psize);
        delete p;
    };
	static void VectorRead(ifstream &model_rd, vector<int> & v){
        int psize = 0;
        model_rd.read((char*)&psize, sizeof(int));
        int *p = new int[psize];
        model_rd.read((char*)p, sizeof(int)*psize);
        v.clear();
        v.reserve(psize);
        //cout << "Rsize:" << psize << endl;
        for(int i = 0; i < psize; i++){
            v.push_back(p[i]);
          //  if(i < 5)
            //    cout << "RD:"<< p[i] << endl;
        }
        delete p;

    };
    static void VectorWrite(ofstream &model_wd, vector<int> & v){
        int psize = v.size();
        model_wd.write((char*)&psize, sizeof(int));
        int *p = new int[psize];
        for(int i = 0; i < psize; i++){
            p[i] = v[i];
            //if(i < 5)
              //  cout << p[i] << endl;
        }
        model_wd.write((char*)p, sizeof(int)*psize);
        delete p;
    };
	static void VectorWrite(ofstream &model_wd, vector<string> & v){
        int psize = v.size();
        model_wd.write((char*)&psize, sizeof(int));
		char *p = NULL;
        for(int i = 0; i < psize; i++){
			int len = v[i].length();
			p = new char[len+1];
			strcpy(p, v[i].c_str());
			model_wd.write((char*)&len, sizeof(int));
			model_wd.write(p, sizeof(char)*len);
			delete p;
            //if(i < 5)
              //  cout << v[i] << endl;
        }
    };
	static void VectorRead(ifstream &model_rd, vector<string> & v){
        int psize = 0;
        model_rd.read((char*)&psize, sizeof(int));
        v.clear();
        v.reserve(psize);
        //cout << "Rsize:" << psize << endl;
		int len;
		char *p = NULL;
        for(int i = 0; i < psize; i++){
			model_rd.read((char*)&len, sizeof(int));
			p = new char[len+1];
			p[len] = '\0';
			model_rd.read(p, sizeof(char)*len);
            v.push_back(p);
            //if(i < 5)
              //  cout << "RD:"<< p << endl;
			delete p;
        }

    };

	static void RemoveUnshownChar(char *p){
		//remove \r\n
		while(*p){
			if(*p == '\r' || *p == '\n')
				break;
			p++;
		}
		*p = '\0';
	}

	void SegSent2Char(const char *sent, char *charlines, const char sepchar){
		int len = strlen(sent);
		for(int i = 0; i < len; ){
			if(sent[i] > 0){
				*charlines = sent[i];
				*(charlines+1) = sepchar;
				i++;
				charlines += 2;
			}
			else{
				*charlines = sent[i];
				*(charlines+1) = sent[i+1];
				*(charlines+2) = sepchar;
				i += 2;
				charlines += 3;
			}
		}
		*(charlines) = '\0';
	}
	static void SegSent2CharVect(string &sent, vector<string> &v){
        int len = sent.length();
		char *p = new char[len+1];
		v.clear();
		v.reserve(len);
        for(int i = 0; i < len; ){
			
            if(sent.at(i) > 0){
                *p = sent.at(i);
                *(p+1) = '\0';
                i++;
            }
            else{
                *p = sent.at(i);
                *(p+1) = sent.at(i+1);
                *(p+2) = '\0';
                i += 2;
            }
			v.push_back(p);
        }
		delete p;
    }
};

#endif
