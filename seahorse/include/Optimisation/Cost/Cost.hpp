#pragma once

#include "include/Optimisation/Cost/ControlCost.hpp"
#include "include/Optimisation/Cost/EvaluatedControl.hpp"
#include "include/Optimisation/Cost/StateTransferCost.hpp"
#include "include/Utils/Globals.hpp"

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
