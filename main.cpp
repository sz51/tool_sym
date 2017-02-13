#include <iostream>
#include <string>
#include "syn.h"

using namespace std;

int main(int argc, char ** argv){
	//cout<<"hello world!"<<endl;
	string filename;
    string partfile;
    assert(argc == 3);
    filename = argv[1];
    partfile = argv[2];
    syn test(filename, partfile);
    bool res = test.realizablity();
    if(res)
        cout<<"realizable"<<endl;
    else
        cout<<"unrealizable"<<endl;
    return 0;

	//smtbdd2bdd x();


    /*smtbdd2bdd x("test.dfa");
    x.construct_bdd();
    x.getdot();
    vector<int> Input;
    vector<int> Output;
    int nbits = x.get_nbits();
    int init = x.get_initial();
    vector<int> finalstates = x.get_final();
    Input.push_back(0);
    Output.push_back(1);
    syn Synthesis(x.get_trans(), init, nbits, finalstates, Input, Output);*/

}
//solveeqn
