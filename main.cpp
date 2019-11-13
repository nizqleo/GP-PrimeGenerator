#include <iostream>
#include <cmath>
#include <algorithm>    
#include <vector>
#include <set>
#include <time.h>
#include "tree.h"
#include "parameters.h"
#include "auxiliary.h"

using namespace std;

vector<Node*> population(population_number);
vector<float> fitness(population_number);
vector<int> notInTheIntermediate;

float largestNFitness(int i){
    // anti bloat
    float sizePanel = 1;
    if(population[i]->size > maxsize && uint_dist(rng)% 2 == 0){
        sizePanel = 1.0/(population[i]->size - maxsize);
    }
    int k;
    int lastResult = population[i]->compute(0);
    if(!isPrime(lastResult))
        return 0; 
    for(k = 1; k<1000; k++){
        int result = population[i]->compute(k);
        if(!isPrime(result) || result == lastResult)
            break;
        lastResult = result;
    }

    return k*sizePanel;
}

float intervalFitness(int i){
    int count = 0;

    // anti bloat
    float sizePanel = 1;
    if(population[i]->size > maxsize && uint_dist(rng)% 2 == 0){
        sizePanel = 1.0/(population[i]->size - maxsize);
    }
    set<int> resultSet;
    for(int k = interval_begin; k < interval_end; k++){
        int result = population[i]->compute(k);
        if(isPrime(result)){
            resultSet.insert(result);
        }
    }

    return sizePanel*resultSet.size();
}

float combinedFitness(int i, float alpha){
    // anti bloat already done in each fitness function.

    return alpha*intervalFitness(i)+(1-alpha)*largestNFitness(i);
}

float combinedFitness(int i){
    // anti bloat already done in each fitness function.

    return intervalFitness(i)*largestNFitness(i);
}

pair<float, int> generationAssess(){
    cout<<"Assessing...\n";
    float totalFitness = 0;
    int maxFitness = 0;
    int maxItem = -1;
    for(int i = 0; i < population_number; i++){
        fitness[i] = combinedFitness(i, 0.1);
        totalFitness+=fitness[i];
        if(maxFitness < fitness[i]){
            maxFitness = fitness[i];
            maxItem = i;
        }
    }
    float average = totalFitness/population_number;
    cout<<"Assessment result: \n Average fitness: "<<average<<endl;
    return make_pair(average,maxItem);
}

//Roulette Wheel Selection
void selection(pair<float, int> assessResult){
    cout<<"selecting...\n";
    float average = assessResult.first;
    notInTheIntermediate.clear();

    for(int i = 0; i < population_number; i++){
        if(fitness[i]<average && float_dist(rng) > float(fitness[i])/average){
            notInTheIntermediate.push_back(i);
        }
    }

    random_shuffle (notInTheIntermediate.begin(), notInTheIntermediate.end());

    int p = 0;
    for(int i = 0; i < population_number; i++){
        if(fitness[i] >= average){
            int numOfSpring = int(fitness[i]/average);
            if(float_dist(rng) < float(fitness[i])/average - numOfSpring)
                numOfSpring+=1;
            for(int j = 0; j < numOfSpring-1; j++){
                if(p < notInTheIntermediate.size()){
                    int ind = notInTheIntermediate[p];
                    delete (population[ind]);
                    population[ind] = NULL;
                    population[i]->duplicate(population[ind], NULL);
                    p++;
                }
            }
            if(p >= notInTheIntermediate.size())
                break;
        }
    }
    random_shuffle (population.begin(), population.end());

}

void crossover(){
    cout<<"crossover"<<endl;
    for(int i = 0; i < population_number; i+=2){
        if(float_dist(rng) < crossoverRate){
            Node* node1 = population[i]->randomNode();
            Node* node2 = population[i+1]->randomNode();

            if(node1->pa != NULL){
                if(node1->pa->ch[0] == node1) node1->pa->ch[0]=node2;
                else node1->pa->ch[1]=node2;

                node1->pa->refresh();
            }
            else{
                population[i] = node2;
                population[i]->refresh();
            } 
            if(node2->pa != NULL){
                if(node2->pa->ch[0] == node2) node2->pa->ch[0]=node1;
                else node2->pa->ch[1]=node1;

                node2->pa->refresh();
            }
            else{
                population[i+1] = node1;
                population[i+1]->refresh();
            }

            Node* temp = node2->pa;
            node2->pa = node1->pa;
            node1->pa = temp;
        }
    }
}

//mutation
void mutation(){
    cout<<"mutation"<<endl;
    for(int i = 0; i < population_number; i++){
        float rate = float_dist(rng);
        if(rate < hugeMutationRate){
            Node* node = population[i]->randomNode();
  
            Node* pa = node->pa;
            if(pa == NULL){
                if(float_dist(rng) < p_func)
                    population[i] = new FuncNode(pa);
                else population[i] = new TermNode(pa);
            }
            else{
                int chInd = pa->ch[0] == node?0:1;
                if(float_dist(rng) < p_func)
                    pa->ch[chInd] = new FuncNode(pa);
                else pa->ch[chInd] = new TermNode(pa);
                pa->refresh();
            }
            delete node;
        }
        else if(rate < hugeMutationRate+smallMutationRate){
            Node* node = population[i]->randomNode();
  
            if(node->isTerm){
                if(float_dist(rng) < p_k){
                    node->setContent(-1);
                    node->hasK = true;
                }
                else {
                    node->setContent(uint_dist(rng));
                    node->hasK = false;
                }
            }
            else{
                node->setContent(uint_dist(rng)%3);
            }
        }
    }
}

void printGeneration(){
    for(int i = 0; i < population_number; i++){
        if(population[i] == NULL) 
            cout<<"NULL"<<endl;
        else{
            cout<<fitness[i]<<"\t";
            population[i]->print(); 
            cout<<endl;
        }
    }
}

void refresh(){
    for(auto p = population.begin(); p != population.end(); p++){
        (*p)->refresh();
    }   
}

int main(){
    uint32_t seed_val;                                                                                                                                                                                                                                              
    rng.seed(seed_val);

    //initialize a population 

    for(int i = 0; i < population_number; i++){
        population[i] = new FuncNode(NULL);
    }
    //benchmark
    // population[population_number+0] = new SpecialNode(17);
    // population[population_number+1] = new SpecialNode(41);

    for(int g = 0; g < maxGenerations; g++){
        rng.seed(time(NULL));
        cout<<"\n\nGeneration "<<g<<":\n";
        
        //assess the population
        pair<float,int> assessResult = generationAssess();
        if(fitness[assessResult.second] == interval_size){
            cout<<"best result get at "<<g<<" generation:\n";
            population[assessResult.second]->print();
            cout<<endl;
            break;
        }
        printGeneration();

        //selection
        selection(assessResult);

        //recombination
        crossover();
        mutation();
        int a;
        cin>>a;
    }
}