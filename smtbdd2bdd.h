#include "Common.h"

#ifndef SMTBDD2BDD_H
#define SMTBDD2BDD_H

using namespace std;

typedef vector<int> item;
class smtbdd2bdd
{
    public:
        smtbdd2bdd();
        virtual ~smtbdd2bdd();
        void initialize(string filename, string partfile);
        //void initialize(string filename, string partfile, Cudd& manager);
        vector<item> bdd;
        void print_vec( vector <item> & v );
        void construct_bdd();
        void bdd2dot();
        void dumpdot(BDD &b, string filename);
        int nbits;
        int init;
        int nstates;

        int nvars;
        vector<int> finalstates;
        vector<BDD> res;
        vector<BDD> bddvars;
        vector<int> input;
        vector<int> output;

        Cudd* mgr;

        /*
        inline int get_nbits(){ return nbits; };
        inline int get_initial(){ return init; };
        inline vector<int> get_final(){ return finalstates; };
        inline void getdot(){bdd2dot();};
        inline vector<BDD> get_trans(){ return res; };
        inline vector<int> get_input(){ return input; };
        inline vector<int> get_output(){ return output; };*/

    protected:
    private:
		int nodes;
		vector<int> behaviour;
		vector<string> variables;
		vector<item> smtbdd;
        void read_from_file(string filename);
        void read_partfile(string partfile);
        void get_bdd();
        void recur(int index, item tmp);
        void recur_left(int index, item tmp, int v);
        void recur_right(int index, item tmp, int v);
        void print( vector <string> & v );
        void print_int( vector <int> & v );
        bool strfind(string str, string target);
        void push_states(int i, item& tmp);
        string state2bin(int n);
        BDD var2bddvar(int v, int index);


};

#endif // SMTBDD2BDD_H
