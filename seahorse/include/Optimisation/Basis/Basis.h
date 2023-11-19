#pragma once

#include "include/Utils/Globals.h"
#include "include/Utils/Random.h"
#include "src/Physics/Vectors.cpp"

class Basis {
private:
    // Holds a control basis which we can access by providing a vector of coefficients
    RMat m_basis;

public:
    Basis(std::function<RVec()> basis_maker, int num_basis_vectors);

    static Basis SIN(const RVec& t, double maxFreq, int num_basis_vectors);
    static Basis COS(const RVec& t, double maxFreq, int num_basis_vectors);
    static Basis TRIG(const RVec& t, double maxFreq, int num_basis_vectors);

    RVec control(RVec coeffs);

    int size();
};