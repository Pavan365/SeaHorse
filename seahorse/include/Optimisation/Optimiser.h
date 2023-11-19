#pragma once

#include "include/Utils/Globals.h"
#include "include/Optimisation/Cost/Cost.h"
#include "include/Optimisation/Stopper/Stopper.h"
#include "include/Physics/SplitStepper.h"

class Optimiser;
typedef std::function<void(const Optimiser&)> SaveFn;

class Optimiser {
private:
    // Stopper
    Stopper stopper;
    // Cost
    Cost cost;
    // Saver
    SaveFn saver;

public:
    friend class dCRAB;

    EvaluatedControl bestControl;
    // Num iterations
    int num_iterations;
    // steps since improvement
    int steps_since_improvement;
    // Full path propagations
    int fpp = 0;

    // Constructor
    Optimiser(Stopper stopper, Cost cost, SaveFn saver);

    virtual ~Optimiser() = 0;

    // perform any initialisation that we may wish to reset
    virtual void init() = 0;

    // Run optimisation
    virtual void optimise() = 0;
    virtual void step() = 0;

private:
    virtual void updateBest(EvaluatedControl&) = 0;
};
