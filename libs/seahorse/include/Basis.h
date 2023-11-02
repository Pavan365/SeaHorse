#pragma once

#include "libs/seahorse/include/Globals.h"

class Basis {
private:
    std::function<RVec()> m_basis_maker;

    // Holds the basis vectors in it's columns
    RMat m_basis;

public:
    Basis(std::function<RVec()> basis_maker, int num_basis_vectors);

    void expand_basis(int num_to_add);
    RVec control(RVec coeffs);
    int size();
};