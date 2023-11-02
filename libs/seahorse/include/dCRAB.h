#pragma once

#include "libs/seahorse/include/Globals.h"
#include "libs/seahorse/include/Optimiser.h"
#include "libs/seahorse/include/SplitStepper.h"
#include "libs/seahorse/include/Basis.h"
#include <tuple>

// Implementation of a dressed CRAB optimiser
class dCRAB : Optimiser {
private:
    // Holds the basis states as columns
    std::unique_ptr<Basis> basis;
    RVec basis_coeffs;

public:
    dCRAB(Basis& basis,
        SplitStepper& stepper, Stopper& stopper, CostFn cost, SaveFn saver);
    // No Saver
    dCRAB(Basis& basis,
        SplitStepper& stepper, Stopper& stopper, CostFn cost);

    // Run optimisation - Basis size inferred from basis
    void optimise();
    void optimise(int num_bases = 1);
};