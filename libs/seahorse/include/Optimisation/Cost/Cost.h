#pragma once

#include "libs/seahorse/include/Optimisation/Cost/ControlCost.h"
#include "libs/seahorse/include/Optimisation/Cost/EvaluatedControl.h"
#include "libs/seahorse/include/Optimisation/Cost/StateTransferCost.h"
#include "libs/seahorse/include/Utils/Globals.h"

// The eventual class that will be passed to an optimiser to evaluate the cost
// StateTransfer calculates a cost based on evolving the state
// Components calculate a cost based only on the control
class Cost {
private:
public:
    StateTransfer transfer;
    int fpp = 0;
    std::vector<ControlCost> components;

    Cost(StateTransfer transfer)
        : transfer(transfer)
    {
        this->transfer = transfer;
    };

    EvaluatedControl operator()(const RVec&);
};

Cost operator+(ControlCost, StateTransfer&);
Cost operator+(Cost, ControlCost);
Cost operator+(ControlCost, Cost);
