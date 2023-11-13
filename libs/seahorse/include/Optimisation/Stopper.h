#pragma once

#include "libs/seahorse/include/Physics/SplitStepper.h"
#include "libs/seahorse/include/Utils/Globals.h"

class Optimiser;

struct Stopper {
    double fid;
    int num_iters;

public:
    // Constructor
    Stopper(double fid, int num_iters);
    // Evaluate if we stop optimising
    bool operator()(Optimiser& opt);
};
