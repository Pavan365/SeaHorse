#pragma once

#include "libs/seahorse/include/Optimisation/Cost/Cost.h"
#include "libs/seahorse/include/Optimisation/Stopper.h"
#include "libs/seahorse/include/Physics/SplitStepper.h"
#include "libs/seahorse/include/Utils/Globals.h"

typedef std::function<void(const Optimiser&)> SaveFn;

class Optimiser {
public:
    // Stopper
    Stopper stopper;
    // Cost
    Cost cost;
    // Saver
    SaveFn saver;

    EvaluatedControl bestControl;
    // Num iterations
    int num_iterations;
    // steps since improvement
    int steps_since_improvement;
    // Full path propagations
    int fpp = 0;

public:
    // Constructor - we need one for each stepper derived type
    Optimiser(Stopper stopper, Cost cost, SaveFn saver);

    virtual ~Optimiser() = 0;

    // perform any initialisation that we may wish to reset
    virtual void init() = 0;

    // Run optimisation
    virtual void optimise() = 0;
    virtual void step() = 0;

    virtual void updateBest(EvaluatedControl&) = 0;
};
