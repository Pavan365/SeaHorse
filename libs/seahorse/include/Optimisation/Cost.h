#pragma once

#include "libs/seahorse/include/Utils/Globals.h"
#include "libs/seahorse/include/Physics/SplitStepper.h"

// This object evaluates a control to return some cost value
// We use the stepper object directly to do this
struct Cost {
    CVec psi_t;

    // Store what we calculate for cost incase it's needed later
    double cost;
    double fid;

public:
    // Constructor
    Cost(CVec psi_t);
    // Evaluate the cost function
    void operator()(Stepper& stepper, const RVec& u);
};