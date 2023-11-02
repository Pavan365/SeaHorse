#pragma once

#include "libs/seahorse/include/Globals.h"
#include "libs/seahorse/include/SplitStepper.h"

class Optimiser;
typedef std::function<double(const Optimiser&)> CostFn;
typedef std::function<void(const Optimiser&)> SaveFn;
struct Stopper {
    std::function<bool(const Optimiser&)> stopfn;

public:
    Stopper(double fidelity, int num_iters);
    bool operator()(const Optimiser& opt);
};

class Optimiser {
protected:
    // Stepper
    std::unique_ptr<Stepper> stepper;

    // Stopper - nb. this could not be undefined
    Stopper stopper;

    // Cost
    CostFn cost;

    // Saver
    SaveFn saver;

    // Optimiser current state
    // Target/Current state/control
    CVec psi_t;
    CVec psi_f;
    RVec u;

    // Num iterations
    int num_iterations;

public:
    // Constructor
    Optimiser(SplitStepper& stepper, Stopper stopper, CostFn cost, SaveFn saver);

    virtual ~Optimiser() = 0;
    // Run optimisation
    virtual void optimise() = 0;

    double fidelity() const;
    double num_iters() const;
    CVec best_psi_f() const;
    RVec best_u() const;
};
