#pragma once

#include "include/Optimisation/Basis/Basis.hpp"
#include "include/Optimisation/Optimiser.hpp"

// Implementation of a dressed CRAB optimiser
class dCRAB : public Optimiser {
private:
    // Nelder-mead meta-parameters
    static constexpr double stepSize = 0.2;
    static constexpr double alpha = 1.0;
    static constexpr double gamma = 2.0;
    static constexpr double rho = 0.5;
    static constexpr double sigma = 0.5;

    struct SimplexPoint {
        RVec coeffs;
        double cost;

        bool operator<(const SimplexPoint& other) const
        {
            return cost < other.cost;
        }
    };
    // Holds the simplex of control coefficients
    std::vector<SimplexPoint> simplex;

    // Holds the basis states as columns
    std::unique_ptr<Basis> basis;

    void updateBest(EvaluatedControl&) override;

public:
    dCRAB(Basis& basis, Stopper& stopper, Cost& cost, SaveFn saver);
    // No Saver
    dCRAB(Basis& basis, Stopper& stopper, Cost& cost);

    // Run optimisation - Basis size inferred from basis
    void optimise() override;
    void optimise(int dressings = 1);

    void init() override;
    void step() override;
};