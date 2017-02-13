#include "syn.h"

syn::syn(string filename, string partfile)
{
    //ctor

    //Cudd *p = &mgr;
    bdd.initialize(filename, partfile);
   // smtbdd2bdd bdd(filename, partfile);
    mgr = *(bdd.mgr);
    //bdd = x;
   // bdd.bdd2dot();
    initializer();
    /*
    BDD test = bdd.bddvars[0]*bdd.bddvars[1]*bdd.bddvars[2]*bdd.bddvars[3];
    BDD c = bdd.bddvars[0]*bdd.bddvars[1];
    BDD t = test.ExistAbstract(c);
    bdd.dumpdot(t, "test");
    cout<<"end"<<endl;*/

}

syn::~syn()
{
    //dtor
}

void syn::initializer(){
    for(int i = 0; i < bdd.nbits; i++){
        BDD b = mgr.bddVar();
        bdd.bddvars.push_back(b);
    }
    BDD tmp = mgr.bddZero();
    for(int i = 0; i < bdd.finalstates.size(); i++){
        BDD ac = state2bdd(bdd.finalstates[i]);
        tmp += ac;
    }
    W.push_back(tmp);
    Wprime.push_back(tmp);
    cur = 0;


}

BDD syn::state2bdd(int s){
    string bin = state2bin(s);
    BDD b = mgr.bddOne();
    int nzero = bdd.nbits - bin.length();
    //cout<<nzero<<endl;
    for(int i = 0; i < nzero; i++){
        b *= !bdd.bddvars[i];
    }
    for(int i = 0; i < bin.length(); i++){
        if(bin[i] == '0')
            b *= !bdd.bddvars[i+nzero];
        else
            b *= bdd.bddvars[i+nzero];
    }
    return b;

}

string syn::state2bin(int n){
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
   return res;
}

bool syn::fixpoint(){
    if(W[cur] == W[cur-1])
        return true;
}

bool syn::realizablity(){
    while(true){
        //cout<<"interative"<<endl;
        //dumpdot(W[cur], "W"+to_string(cur));
        //dumpdot(Wprime[cur], "Wprme"+to_string(cur));
        BDD tmp = W[cur] + univsyn();
        W.push_back(tmp);
        cur++;
        //dumpdot(W[cur], "W"+to_string(cur));
        if(fixpoint()){
            BDD O = mgr.bddOne();
            vector<BDD> S2O;
            for(int i = 0; i < bdd.output.size(); i++){
                //cout<<bdd.output[i]<<endl;
                O *= bdd.bddvars[bdd.output[i]];
            }
            O *= bdd.bddvars[bdd.nbits];
            //dumpdot(O, "outcube");
            W[cur].SolveEqn(O, S2O, outindex(), bdd.output.size()+1);

            break;
        }
        Wprime.push_back(existsyn());
        //assert(cur = (W.size() - 1));
    }
    if((W[cur].Eval(state2bit(bdd.init))).IsOne())
        return true;
    return false;

}

int** syn::outindex(){
    int outlength = bdd.output.size() + 1;
    int out[outlength];
    for(int i = 0; i < bdd.output.size(); i++){
        int a = bdd.output[i];
        //cout<<a<<endl;
        out[i] = bdd.output[i];
    }
    int i = outlength - 1;
    out[i] = bdd.nbits;
    //int** x = *x1;
    int* x = out;
    return &x;
}

int* syn::state2bit(int n){
    string res;
    int s[bdd.nbits+1];
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
    int offset = bdd.nbits - res.length();
    for(int i = 0; i < offset; i++)
        s[i] = 0;
    for(int i = offset; i < res.length(); i++)
        s[i] = (int(res[i]) - 48);
    s[bdd.nbits] = 1;
    return s;
}


BDD syn::univsyn(){
    BDD I = mgr.bddOne();
    BDD tmp = Wprime[cur];
    int index;
    int offset = bdd.nbits + bdd.nvars;
    for(int i = 0; i < bdd.input.size(); i++){
        index = bdd.input[i];
        I *= bdd.bddvars[index];
    }
    //dumpdot(I, "W00");
    tmp = prime(tmp);
    //dumpdot(tmp, "s-s'"+to_string(cur));
    for(int i = 0; i < bdd.nbits; i++){
        tmp = tmp.Compose(bdd.res[i], offset+i);
        //dumpdot(tmp, "s.compose'"+to_string(i));
    }
    //dumpdot(tmp, "W00");

    tmp *= !Wprime[cur];

    BDD eliminput = tmp.UnivAbstract(I);
    //dumpdot(eliminput, "W01");
    //dumpdot(eliminput, "EU"+to_string(cur));
    return eliminput;

}

BDD syn::prime(BDD orign){
    int offset = bdd.nbits + bdd.nvars;
    BDD tmp = orign;
    for(int i = 0; i < bdd.nbits; i++){
        tmp = tmp.Compose(bdd.bddvars[i+offset], i);
    }
    return tmp;
}

BDD syn::existsyn(){
    BDD O = mgr.bddOne();
    BDD tmp = W[cur];
    int index;
    int offset = bdd.nbits + bdd.nvars;
    for(int i = 0; i < bdd.output.size(); i++){
        index = bdd.output[i];
        O *= bdd.bddvars[index];
    }
    BDD elimoutput = tmp.ExistAbstract(O);
    return elimoutput;

}

void syn::dumpdot(BDD &b, string filename){
    FILE *fp = fopen(filename.c_str(), "w");
    vector<BDD> single(1);
    single[0] = b;
	this->mgr.DumpDot(single, NULL, NULL, fp);
	fclose(fp);
}


















