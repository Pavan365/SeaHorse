#include <random>

struct randGen
{
    // construct a mersenne twister random generator with an associated uniform distribution to draw from [0-1)
    std::mt19937_64 gen;
    std::uniform_real_distribution<double> dist;

    randGen(double start, double stop) : dist(std::uniform_real_distribution<double>(start,stop))
    {
        auto since_epoch = std::chrono::high_resolution_clock::now().time_since_epoch();
        auto since_epoch_nano = std::chrono::duration_cast< std::chrono::microseconds >(since_epoch);
        double rand_seed = since_epoch_nano.count();
        gen = std::mt19937_64(rand_seed);
    }
    randGen(): randGen(0.0,1.0) {}

    double operator()() {return dist(gen);}
};

#ifndef randGen
randGen rands(0.0,1.0);
#define randGen
#endif