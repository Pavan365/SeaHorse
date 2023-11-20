#pragma once

#include "include/Physics/Potential.hpp"

RVec Potential::AmplitudeModulatedV(double amp) const { return m_V * amp; };
// we use linear interpolation to translate the vector
RVec Potential::ShakenV(double x0) const
{
    static double xSize = m_x.size();
    static double xRange = m_x.maxCoeff() - m_x.minCoeff();
    // Start of our desired window [the 1.0 is due to us tripling our range]
    double full_offset = xSize * (1.0 - x0 / xRange);
    // Avoid OOB errors
    full_offset = std::max(std::min(full_offset, 2 * xSize - 1), 0.0);

    int offset = std::floor(full_offset);
    double frac = full_offset - offset;

    RVec newV = (1 - frac) * m_V.segment(offset, xSize) + frac * m_V.segment(offset + 1, xSize);
    return newV;
};

RVec Potential::operator()(double control) const
{
    switch (m_type) {
    case Type::CONSTANT:
        return m_V;
    case Type::AMPLITUDE:
        return AmplitudeModulatedV(control);
    case Type::SHAKEN:
        return ShakenV(control);
    case Type::CUSTOM:
        return m_Vfn(control);
    };
};

// Specific control methods
Potential::Potential(HilbertSpace& hs, const RVec& V, Type type)
    : m_x(hs.x())
    , m_V(V)
    , m_type(type)
{
    // If we are shaking we need to extend V
    if (m_type == Type::SHAKEN) {
        int xSize = m_x.size();
        m_V = RVec::Zero(xSize * 3);
        m_V.segment(0, xSize) = RVec::Constant(xSize, V[0]);
        m_V.segment(xSize, xSize) = V;
        m_V.segment(2 * xSize, xSize) = RVec::Constant(xSize, V[V.size() - 1]);
    };
};

Potential ShakenPotential(HilbertSpace& hs, const RVec& V) { return Potential(hs, V, Potential::Type::SHAKEN); };
Potential AmplitudePotential(HilbertSpace& hs, const RVec& V) { return Potential(hs, V, Potential::Type::AMPLITUDE); };
Potential ConstantPotential(HilbertSpace& hs, const RVec& V) { return Potential(hs, V, Potential::Type::CONSTANT); };