#include <iostream>
#include <random>
using namespace std;



class Node{
public:
    Node* ch[2];
    Node* pa;

    Node(){}
    Node(Node * p): pa(p){
        if(p == NULL) depth = 0;
        else depth = p->depth+1;
        ch[0] = NULL;
        ch[1] = NULL;
    }

    bool isTerm;
    int size;
    bool hasK;
    int depth;
    virtual void print() = 0;
    virtual int compute(int k) = 0;
    virtual void duplicate(Node*& target, Node* pa)= 0;
    virtual void setContent(int i)=0;
    virtual Node* randomNode() = 0; 
    virtual void refresh()=0;
    ~Node(){
        if(ch[0] != NULL) delete ch[0];
        if(ch[1] != NULL) delete ch[1];
    }
};

class TermNode:public Node{
public:
    TermNode(){}

    TermNode(Node * p);
    TermNode(Node * p, int k);
    int number;
    void print();
    int compute(int k);
    void duplicate(Node*& target, Node* pa);
    void setContent(int i);
    Node* randomNode();
    void refresh();
};

enum FUNCTION {ADD = 0, MINUS = 1, MULTI = 2};
class FuncNode:public Node{
public:

    FuncNode(Node * p);
    FUNCTION func;
    void print();
    int compute(int k);
    void duplicate(Node*& target, Node* pa);
    void setContent(int i);
    Node* randomNode();
    void refresh();
};

class SpecialNode:public Node{
public:
    SpecialNode(int i):number(i){}

    int number;
    void print(){cout<<"k*k+k+"<<number;}
    int compute(int k){return k*k + k + number;}
    void duplicate(Node*& target, Node* pa){}
    void setContent(int i){}
    Node* randomNode(){};
    void refresh(){};
};