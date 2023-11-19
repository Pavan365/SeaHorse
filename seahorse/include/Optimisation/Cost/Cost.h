#pragma once

#include "include/Optimisation/Cost/ControlCost.h"
#include "include/Optimisation/Cost/EvaluatedControl.h"
#include "include/Optimisation/Cost/StateTransferCost.h"
#include "include/Utils/Globals.h"

// Evaluates a control based on the state to state transfers and control penalties
class Cost {
public:
    EvaluatedControl operator()(const RVec&);
    int fpp = 0;

    Cost(StateTransfer transfer)
        : transfer(transfer)
    {
        this->transfer = transfer;
    };

    void addCost(ControlCost cc)
    {
        components.push_back(cc);
    }

private:
    StateTransfer transfer;
    std::vector<ControlCost> components;
};

Cost operator+(ControlCost, StateTransfer&);
Cost operator+(Cost, ControlCost);
Cost operator+(ControlCost, Cost);
