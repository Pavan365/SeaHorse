#pragma once

#include "libs/seahorse/include/Basis.h"

Basis::Basis(std::function<RVec()> basis_maker, int num_basis_vectors)
    : m_basis_maker(basis_maker)
{
    // used to find the size of the basis vectors which we assume is constant
    RVec temp = m_basis_maker();
    m_basis.col(0) = temp;

    m_basis.resize(temp.size(), num_basis_vectors);
    for (int i = 1; i < num_basis_vectors; i++) {
        m_basis.col(i) = m_basis_maker();
    }
}

void Basis::expand_basis(int num_to_add)
{
    m_basis.conservativeResize(m_basis.rows(), m_basis.cols() + num_to_add);
    for (int i = 0; i < num_to_add; i++) {
        m_basis.col(m_basis.cols() - 1) = m_basis_maker();
    }
}

RVec Basis::control(RVec coeffs) { return m_basis * coeffs; }

int Basis::size() { return m_basis.cols(); }