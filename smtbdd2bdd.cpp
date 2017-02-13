#include "smtbdd2bdd.h"
using namespace std;
using namespace boost;



smtbdd2bdd::smtbdd2bdd(){
    //ctor
}

smtbdd2bdd::~smtbdd2bdd()
{
    //dtor
    delete mgr;// = NULL;
}
/*
void smtbdd2bdd::dumpdot(BDD &b, string filename){
    FILE *fp = fopen(filename.c_str(), "w");
    vector<BDD> single(1);
    single[0] = b;
	mgr.DumpDot(single, NULL, NULL, fp);
	fclose(fp);
}
*/
//void smtbdd2bdd::initialize(string filename, string partfile, Cudd* manager){
void smtbdd2bdd::initialize(string filename, string partfile){
    //ctor
    mgr = new Cudd();
    read_from_file(filename);
    nbits = state2bin(nstates-1).length();
    //cout<<nbits<<endl;

    get_bdd();
    //print_vec(bdd);
    construct_bdd();

    read_partfile(partfile);
}

void smtbdd2bdd::read_partfile(string partfile){
    ifstream f(partfile);
    vector<string> inputs;
    vector<string> outputs;
    string line;
    while(getline(f, line)){
        if(f.is_open()){
            if(strfind(line, "inputs")){
                split(inputs, line, is_any_of(" "));
                //print(inputs);
            }
            else if(strfind(line, "outputs")){
                split(outputs, line, is_any_of(" "));
                //print(outputs);
            }
            else
                cout<<"read partfile error!"<<endl;
		}
    }
    f.close();
    set<string> input_set;
    set<string> output_set;
    for(int i = 1; i < inputs.size(); i++)
        input_set.insert(inputs[i]);
    for(int i = 1; i < outputs.size(); i++)
        output_set.insert(outputs[i]);
    for(int i = 1; i < variables.size(); i++){
        if(input_set.find(variables[i]) != input_set.end())
            input.push_back(nbits+i-1);
        else if(output_set.find(variables[i]) != output_set.end())
            output.push_back(nbits+i-1);
        else if(variables[i] == "ALIVE")
            ;
        else
            cout<<"error"<<endl;
    }
    //cout<<"input: "<<input.size()<<endl;
    //print_int(input);
    //print_int(output);
}

void smtbdd2bdd::read_from_file(string filename){
ifstream f(filename);
	if(f.is_open()){
		bool flag = 0;
		string line;
		item tmp;
        vector <string> fields;

		while(getline(f, line)){
            if(flag == 0){
                if(strfind(line, "ALIVE")){
                    split(variables, line, is_any_of(" "));
                    //print(variables);
                    //cout<<nvars<<endl;
                }
                if(strfind(line, "number of variables")){
                    split(fields, line, is_any_of(" "));
                    nvars = stoi(fields[3]);
                    //cout<<nvars<<endl;
                }
                else if(strfind(line, "states")){
                    split(fields, line, is_any_of(" "));
                    nstates = stoi(fields[1]);
                   // cout<<nstates<<endl;
                }
                else if(strfind(line, "initial")){
                    split(fields, line, is_any_of(" "));
                    init = stoi(fields[1]);
                    //cout<<init<<endl;
                }
                else if(strfind(line, "bdd nodes")){
                    split(fields, line, is_any_of(" "));
                    nodes = stoi(fields[2]);
                    //cout<<nodes<<endl;
                }
                else if(strfind(line, "final")){
                    split(fields, line, is_any_of(" "));
                    int i = 1;
                    while(i < fields.size()){
                        if(fields[i] == "1")
                            finalstates.push_back(i-1);
                        i = i + 1;
                    }
                    //print_int(finalstates);
                }
                else if(strfind(line, "behaviour")){
                    split(fields, line, is_any_of(" "));
                    int i = 1;
                    while(i < fields.size()){
                        behaviour.push_back(stoi(fields[i]));
                        i = i + 1;
                    }
                    //print_int(behaviour);
                }
                else if(strfind(line, "bdd:"))
                    flag = 1;
                else
                    continue;
            }
            else{
                if(strfind(line, "end"))
                    break;
                split(fields, line, is_any_of(" "));
                for(int i = 1; i < fields.size(); i++)
                    tmp.push_back(stoi(fields[i]));
                smtbdd.push_back(tmp);
                tmp.clear();
            }
		}

	}
	f.close();
    //print_vec(smtbdd);
}

void smtbdd2bdd::recur(int index, item tmp){
    if(smtbdd[index][0] == -1){
        while(tmp.size() < nbits + nvars)
            tmp.push_back(2); //0:false 1:true 2:don't care
        push_states(smtbdd[index][1], tmp);
        bdd.push_back(tmp);
        //print_vec(bdd);
        tmp.clear();
    }
    else{
        int left = smtbdd[index][1];
        int right = smtbdd[index][2];
        int v = smtbdd[index][0];
        recur_left(left, tmp, v);
        recur_right(right, tmp, v);
    }
}

void smtbdd2bdd::recur_left(int index, item tmp, int v){
	while(tmp.size() < (nbits + v))
		tmp.push_back(2); //0:false 1:true 2:don't care
	tmp.push_back(0);
	recur(index, tmp);
}

void smtbdd2bdd::recur_right(int index, item tmp, int v){
	while(tmp.size() < (nbits+v))//????????????
		tmp.push_back(2); //0:false 1:true 2:don't care
	tmp.push_back(1);
	recur(index, tmp);
}

void smtbdd2bdd::get_bdd(){
    for(int i = 0; i < nstates; i++){
        int index = behaviour[i];
        item tmp;
        push_states(i, tmp);
        recur(index, tmp);
    }
}

void smtbdd2bdd::push_states(int i, item & tmp){
    string s = state2bin(i);
    for(int j = 0; j < nbits - s.length(); j++)
        tmp.push_back(0);
    for(int j = 0; j < s.length(); j++){
        int t = int(s[j]) - 48;
        tmp.push_back(t);
    }
}

void smtbdd2bdd::print( vector <string> & v )
{
  for (size_t n = 0; n < v.size(); n++)
    cout << v[ n ] << " ";
  cout << endl;
}

void smtbdd2bdd::print_int( vector <int> & v )
{
  for (size_t n = 0; n < v.size(); n++)
    cout<< v[ n ] << " ";
  cout << endl;
}

void smtbdd2bdd::print_vec(vector<item> & v){
    for (size_t n = 0; n < v.size(); n++)
        print_int(v[n]);
  cout << endl;
}

bool smtbdd2bdd::strfind(string str, string target){
	size_t found = str.find(target);
	if(found != string::npos)
		return true;
	else
		return false;
}

string smtbdd2bdd::state2bin(int n){
    string res;
    while (n)
    {
        res.push_back((n & 1) + '0');
        n >>= 1;
    }

    if (res.empty())
        res = "0";
    else
        reverse(res.begin(), res.end());
    //cout<<res<<endl;
   return res;
}

void smtbdd2bdd::bdd2dot(){
    for(int i = 0; i < res.size(); i++){
        string filename = to_string(i);
        dumpdot(res[i], filename);
    }
}



BDD smtbdd2bdd::var2bddvar(int v, int index){
    if(v == 0){
        return !bddvars[index];
    }
    else{
        return bddvars[index];
    }
}


void smtbdd2bdd::construct_bdd(){

    for(int i = 0; i < nbits+nvars; i++){
        BDD b = mgr->bddVar();
        bddvars.push_back(b);
    }

    for(int i = 0; i < nbits; i++){
        BDD d = mgr->bddZero();
        res.push_back(d);
    }
    //cout<<"bddvars.length: "<<bddvars.size()<<endl;

    for(int i = 0; i < bdd.size(); i++){
        for(int j = 0; j < nbits; j++){
            if(bdd[i][nbits+nvars+j] == 1){
                BDD tmp = mgr->bddOne();
                for(int m = 0; m < nbits+nvars; m++)
                {
                    if(bdd[i][m] != 2){
                        tmp *= var2bddvar(bdd[i][m], m);
                    }
                }
                res[j] += tmp;
            }
        }
    }
}

void smtbdd2bdd::dumpdot(BDD &b, string filename){
    FILE *fp = fopen(filename.c_str(), "w");
    vector<BDD> single(1);
    single[0] = b;
	this->mgr->DumpDot(single, NULL, NULL, fp);
	fclose(fp);
}














