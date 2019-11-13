#include <random>

typedef mt19937 MyRNG;  // the Mersenne Twister with a popular choice of parameters
//uint32_t seed_val;           // populate somehow
MyRNG rng;   
int termNumMax = 50;
uniform_int_distribution<uint32_t> uint_dist(1,termNumMax);
uniform_real_distribution<float> float_dist(0, 1);


float p_func = 0.5;
float p_k = 0.4;
int maxGenerations = 100;
int population_number = 50;
int layerNumber = 4;
int maxsize = 10;
int interval_begin = 0;
int interval_end = 20;
int interval_size = interval_end-interval_begin;
float crossoverRate = 0.9;
float hugeMutationRate = 0.01;
float smallMutationRate = 0.4;
