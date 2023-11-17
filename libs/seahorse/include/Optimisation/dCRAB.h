#pragma once

#include "libs/seahorse/include/Optimisation/Basis.h"
#include "libs/seahorse/include/Optimisation/Cost/Cost.h"
#include "libs/seahorse/include/Optimisation/Optimiser.h"
#include "libs/seahorse/include/Physics/SplitStepper.h"
#include "libs/seahorse/include/Utils/Globals.h"
#include <tuple>

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

public:
    // Holds the basis states as columns
    std::unique_ptr<Basis> basis;

    dCRAB(Basis& basis, Stopper& stopper, Cost& cost, SaveFn saver);
    // No Saver
    dCRAB(Basis& basis, Stopper& stopper, Cost& cost);

    // Run optimisation - Basis size inferred from basis
    void optimise() override;
    void optimise(int dressings = 1);

    void updateBest(EvaluatedControl&) override;

    void init() override;
    void step() override;
};