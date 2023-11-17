#pragma once

#include "libs/seahorse/include/Physics/SplitStepper.h"
#include "libs/seahorse/include/Utils/Globals.h"

class Optimiser;

struct Stopper {
    double fid;
    int num_iters;
    int steps_since_improvement;

public:
    // Constructor
    Stopper(double fid, int num_iters, int steps_since_improvement);
    // Evaluate if we stop optimising
    bool operator()(Optimiser& opt);
};
