#pragma once

#include "libs/seahorse/include/Optimisation/Cost.h"
#include "libs/seahorse/include/Optimisation/Stopper.h"
#include "libs/seahorse/include/Physics/SplitStepper.h"
#include "libs/seahorse/include/Utils/Globals.h"

typedef std::function<void(const Optimiser&)> SaveFn;

class Optimiser {
public:
    // Stepper - We want polymorphism so need to own a pointer type to the base class
    std::unique_ptr<Stepper> stepper;

    // Stopper
    Stopper stopper;

    // Cost
    Cost cost;

    // {.control,.cost,.fid}
    struct EvaluatedControl {
        RVec control;
        double cost;
        double fid;
    };
    EvaluatedControl bestControl;

    // Saver
    SaveFn saver;

    // Num iterations
    int num_iterations;

public:
    // Constructor - we need one for each stepper derived type
    Optimiser(SplitStepper& stepper, Stopper stopper, Cost cost, SaveFn saver);

    virtual ~Optimiser() = 0;

    // perform any initialisation that we may wish to reset
    virtual void init() = 0;

    // Run optimisation
    virtual void optimise() = 0;
    virtual void step() = 0;

    double num_iters() const;
};
