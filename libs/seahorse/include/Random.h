#pragma once

#include <random>

uint64_t get_rand_seed();

struct randGen {
    // construct a mersenne twister random generator with an associated uniform distribution to draw from [0-1)
    std::mt19937_64 gen;
    std::uniform_real_distribution<double> dist;

    randGen(double start, double stop);

    randGen();

    double operator()();
};

extern randGen rands;