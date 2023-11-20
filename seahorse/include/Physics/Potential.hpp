#pragma once

#include "include/Physics/HilbertSpace.hpp"
#include "include/Physics/Vectors.hpp"

// Potentials hold some potential function V(x) defined on real space
// They have three flavours currently - Constant, Shaken, and AmplitudeModulated
// Otherwise a custom one can be supplied

// Non-custom ones support serialisation
// This is implemented by the operator() which transforms and returns their internal V(x)
class Potential {
public:
    enum class Type {
        // Always V
        CONSTANT,
        // V * control
        AMPLITUDE,
        // V(x + x0)
        SHAKEN,
        // Defined by provided std::function<RVec(double)>
        CUSTOM
    };

private:
    RVec m_x;
    RVec m_V;
    Type m_type;

    // Slow fallback to this for custom potentials
    std::function<RVec(double)> m_Vfn = nullptr;

    RVec AmplitudeModulatedV(double control) const;
    // we use linear interpolation to translate the vector
    RVec ShakenV(double x0) const;

public:
    RVec operator()(double control) const;

    // Custom control methods
    Potential(std::function<RVec(double)> V)
        : m_type(Type::CUSTOM)
        , m_Vfn(V) {};
    // Specific control methods
    Potential(HilbertSpace& hs, const RVec& V, Type type);
};

// helpers to avoid specifying the type

// NOTE: ShakenPotentials suffer from interpolation errors on the order of ~0.1%
// for dim=2048
Potential ShakenPotential(HilbertSpace& hs, const RVec& V);
Potential AmplitudePotential(HilbertSpace& hs, const RVec& V);
Potential ConstantPotential(HilbertSpace& hs, const RVec& V);