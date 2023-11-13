#pragma once

#include "libs/seahorse/include/Optimisation/Basis.h"
#include "libs/seahorse/include/Utils/Random.h"
#include "libs/seahorse/src/Physics/Vectors.cpp"

Basis::Basis(std::function<RVec()> basis_maker, int num_basis_vectors)
{
    // used to find the size of the basis vectors which we assume is constant
    auto temp = basis_maker();
    m_basis.resize(temp.size(), num_basis_vectors);

    // fill out the full basis
    for (int i = 0; i < num_basis_vectors; i++) {
        m_basis.col(i) = basis_maker();
    }
}

Basis Basis::SIN(const RVec& t, double maxFreq, int num_basis_vectors)
{
    return Basis([t, maxFreq]() { return rands() * sin(rands() * maxFreq * t); }, num_basis_vectors);
}
Basis Basis::COS(const RVec& t, double maxFreq, int num_basis_vectors)
{
    return Basis([t, maxFreq]() { return rands() * cos(rands() * maxFreq * t); }, num_basis_vectors);
}
Basis Basis::TRIG(const RVec& t, double maxFreq, int num_basis_vectors)
{
    return Basis([t, maxFreq]() { return rands() * sin(rands() * maxFreq * t + 2 * PI * rands()); }, num_basis_vectors);
}

RVec Basis::control(RVec coeffs) { return m_basis * coeffs; }

int Basis::size() { return m_basis.cols(); }