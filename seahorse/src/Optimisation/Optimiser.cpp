#include "include/Optimisation/Optimiser.h"

// Constructor
Optimiser::Optimiser(Stopper stopper, Cost cost, SaveFn saver)
    : stopper(stopper)
    , cost(cost)
    , saver(saver)
{
    num_iterations = 0;
    steps_since_improvement = 0;
};

Optimiser::~Optimiser() { }
