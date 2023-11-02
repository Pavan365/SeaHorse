#pragma once

#include "libs/seahorse/include/Optimiser.h"
#include "libs/seahorse/include/SplitStepper.h"

// Constructor
Optimiser::Optimiser(SplitStepper& stepper, Stopper stopper, CostFn cost, SaveFn saver)
    : stepper(std::make_unique<SplitStepper>(stepper))
    , stopper(stopper)
    , cost(cost)
    , saver(saver) {};

Optimiser::~Optimiser() { }

double Optimiser::fidelity() const
{
    // std::norm(cdot(a,b) * dx)
    return std::norm(psi_f.dot(psi_t)) * stepper->dx();
}

double Optimiser::num_iters() const { return num_iterations; }
CVec Optimiser::best_psi_f() const { return psi_f; }
RVec Optimiser::best_u() const { return u; }

// Stopper
Stopper::Stopper(double fidelity, int num_iters) {
    stopfn = [&](const Optimiser& opt) { return opt.fidelity() > fidelity || opt.num_iters() > num_iters;};
};
bool Stopper::operator()(const Optimiser &opt) {
    return stopfn(opt);
};