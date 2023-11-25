#include "include/Optimisation/Basis/Basis.hpp"
#include "include/Physics/Vectors.hpp"

Basis::Basis(std::function<RVec()> basis_maker, int num_basis_vectors)
{
    // used to find the size of the basis vectors which we assume is constant
    RVec temp = basis_maker();
    m_basis.resize(temp.size(), num_basis_vectors);

    // fill out the full basis
    for (int i = 0; i < num_basis_vectors; i++) {
        // applying the planck-taper window
        m_basis.col(i) = planck_taper(basis_maker());
    }
}

// Normalized basis functions [-1,1]
Basis Basis::SIN(const RVec& t, double maxFreq, int num_basis_vectors)
{
    return Basis([t, maxFreq]() -> RVec { return sin(rands() * maxFreq * t); }, num_basis_vectors);
}
Basis Basis::COS(const RVec& t, double maxFreq, int num_basis_vectors)
{
    return Basis([t, maxFreq]() -> RVec { return cos(rands() * maxFreq * t); }, num_basis_vectors);
}
Basis Basis::TRIG(const RVec& t, double maxFreq, int num_basis_vectors)
{
    return Basis([t, maxFreq]() -> RVec { return sin(rands() * maxFreq * t + 2 * PI * rands()); }, num_basis_vectors);
}

RVec Basis::control(RVec coeffs) { return m_basis * coeffs; }

int Basis::size() { return m_basis.cols(); }