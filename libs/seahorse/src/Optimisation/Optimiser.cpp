#pragma once

#include "libs/seahorse/include/Optimisation/Optimiser.h"
#include "libs/seahorse/include/Physics/SplitStepper.h"

// Constructor
Optimiser::Optimiser(SplitStepper& stepper, Stopper stopper, Cost cost, SaveFn saver)
    : stepper(std::make_unique<SplitStepper>(stepper))
    , stopper(stopper)
    , cost(cost)
    , saver(saver) {};

Optimiser::~Optimiser() { }

double Optimiser::num_iters() const { return num_iterations; }