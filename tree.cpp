#include "tree.h"
#include <cstdlib>
#include <assert.h>

typedef mt19937 MyRNG;  // the Mersenne Twister with a popular choice of parameters
extern uint32_t seed_val;           // populate somehow
extern MyRNG rng;   
extern int termNumMax;
extern uniform_int_distribution<uint32_t> uint_dist;
extern uniform_real_distribution<float> float_dist;
extern float p_func;
extern float p_k;
extern int generation_number;
extern int layerNumber;


TermNode::TermNode(Node * p):Node(p){
    if(float_dist(rng) < p_k){
        number = -1;
        hasK = true;
    }
    else {
        number = uint_dist(rng);
        hasK = false;
    }
    isTerm = true;
    size = 1;
}

TermNode::TermNode(Node * p, int k):Node(p){
    number = k;
    if(k == -1){
        hasK = true;
    }
    else {
        hasK = false;
    }
    isTerm = true;
    size = 1;
}

FuncNode::FuncNode(Node * p):Node(p){
    //assert(layerLeft >= 1);
    int func_int = uint_dist(rng)%3;
    
    float p_depth_func = p_func/depth;
    float rate[2];
    rate[0] = float_dist(rng);
    rate[1] = float_dist(rng);

    if(rate[0] > p_depth_func && rate[1] > p_depth_func){
        int ind = uint_dist(rng)%2;
        ch[ind] = new TermNode(this, -1);
        if(float_dist(rng) < p_k*p_k){
            ch[1-ind] = new TermNode(this, -1);
            func_int += float_dist(rng)>0.5?1:-1;
        }
        else
            ch[1-ind] = new TermNode(this);  
    }
    else{
        for(int i  = 0; i < 2; i++){
            if(rate[i] < p_depth_func)
                ch[i] = new FuncNode(this);
            else ch[i] = new TermNode(this);
        }
    }

    hasK = (ch[0]->hasK || ch[1]->hasK);
    isTerm = false;
    size = ch[0]->size+ch[1]->size+1;
    func = FUNCTION(func_int);
}

void TermNode::refresh(){
    isTerm = 1;
    size = 1;
    hasK = (number == -1);
}

void FuncNode::refresh(){
    ch[0]->refresh();
    ch[1]->refresh();
    if(pa != NULL && ch[0]->isTerm && ch[1]->isTerm){
        if(ch[0]->hasK && ch[1]->hasK && func == MINUS){
            int ind = pa->ch[0] == this?0:1;
            pa->ch[ind] = new TermNode(pa, 0);
            //delete this;
            return;
        }
        else if(!ch[0]->hasK && !ch[1]->hasK){
            int ind = pa->ch[0] == this?0:1;
            pa->ch[ind] = new TermNode(pa, compute(0));
            //delete this;
            return;
        }
    }
    size = ch[0]->size + ch[1]->size + 1;
    hasK = (ch[0]->hasK || ch[1]->hasK);
    isTerm = 0;
}

void TermNode::print(){
    if(number == -1)
        cout<<"k";
    else
        cout<<number;    
}

void FuncNode::print(){
    cout<<'(';
    ch[0]->print();
    cout<<')';
    switch (func)
    {
    case ADD:
        cout<<"+";
        break;
    case MINUS:
        cout<<"-";
        break;
    case MULTI:
        cout<<"*";
        break;
    // case DIVIDE:
    //     cout<<"/";
    //     break;
    default:
        cout<<int(func);
        break;
    }
    cout<<'(';
    ch[1]->print();
    cout<<')';
}

int FuncNode::compute(int k){
    int a = ch[0]->compute(k);
    int b = ch[1]->compute(k);

    switch (func)
    {
    case ADD:
        return a+b;
        break;
    case MINUS:
        return a-b;
        break;
    case MULTI:
        return a*b;
        break;
    // case DIVIDE:
    //     if(b != 0)
    //         return a/b;
    //     else return 0;
        break;
    }
}

int TermNode::compute(int k){
    if(number == -1)
        return k;
    else return number;
}

void TermNode::setContent(int i){number = i;}
void FuncNode::setContent(int i){func = FUNCTION(i);}

void FuncNode::duplicate(Node*& target, Node* pa){
    target = new FuncNode(pa);
    target->setContent(func);
    ch[0]->duplicate(target->ch[0], target);
    ch[1]->duplicate(target->ch[1], target);
}

void TermNode::duplicate(Node*& target, Node* pa){
    target = new TermNode(pa);
    target->setContent(number);
}

Node* TermNode::randomNode(){return this;}
Node* FuncNode::randomNode(){
    if(size == 0){
        refresh();
    }
    int r = uint_dist(rng) % size; 
    if (r < ch[0]->size) return ch[0]->randomNode(); 
    else if (r > ch[1]->size) return ch[1]->randomNode(); 
    else return this;
}
