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

    Cost(StateTransfer transfer) { this->transfers.push_back(transfer); };
    Cost(ControlCost cc) { this->components.push_back(cc); };

    void addTransferCost(StateTransfer st)
    {
        transfers.push_back(st);
    }

    void addControlCost(ControlCost cc) { components.push_back(cc); }

    Cost operator+(Cost& other);
    Cost operator+(StateTransfer st);
    Cost operator+(ControlCost cc);
    // Cost operator+(RobustCost rc);

private:
    std::vector<StateTransfer> transfers;
    std::vector<ControlCost> components;
};

// self-self
Cost operator+(StateTransfer, StateTransfer);
Cost operator+(ControlCost, ControlCost);
// Cost operator+(RobustCost, RobustCost);

// Cost-self to mirror inside Cost Class
Cost operator+(StateTransfer, Cost);
Cost operator+(ControlCost, Cost);
// Cost operator+(RobustCost, Cost);

// allow for commutativity
Cost operator+(StateTransfer, ControlCost);
// Cost operator+(StateTransfer, RobustCost);
Cost operator+(ControlCost, StateTransfer);
// Cost operator+(ControlCost, RobustCost);
// Cost operator+(RobustCost, StateTransfer);
// Cost operator+(RobustCost, ControlCost);
