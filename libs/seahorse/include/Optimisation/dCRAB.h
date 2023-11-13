#pragma once

#include "libs/seahorse/include/Optimisation/Basis.h"
#include "libs/seahorse/include/Optimisation/Optimiser.h"
#include "libs/seahorse/include/Physics/SplitStepper.h"
#include "libs/seahorse/include/Utils/Globals.h"
#include <tuple>

// Implementation of a dressed CRAB optimiser
class dCRAB : public Optimiser {
private:
    // Nelder-mead meta-parameters
    static constexpr double stepSize = 0.1;
    static constexpr double alpha = 1.0;
    static constexpr double gamma = 2.0;
    static constexpr double rho = 0.5;
    static constexpr double sigma = 0.5;

    // Holds the simplex of control coefficients
    std::vector<EvaluatedControl> simplex;

public:
    // Holds the basis states as columns
    std::unique_ptr<Basis> basis;

    dCRAB(Basis& basis, SplitStepper& stepper, Stopper& stopper, Cost& cost, SaveFn saver);
    // No Saver
    dCRAB(Basis& basis, SplitStepper& stepper, Stopper& stopper, Cost& cost);

    // Run optimisation - Basis size inferred from basis
    void optimise() override;
    void optimise(int dressings = 1);

    void init() override;
    void step() override;
};