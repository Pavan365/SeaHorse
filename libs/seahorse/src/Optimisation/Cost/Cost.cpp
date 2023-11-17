#include "libs/seahorse/include/Optimisation/Cost/Cost.h"
#include "libs/seahorse/src/Optimisation/Cost/ControlCost.cpp"
#include "libs/seahorse/src/Optimisation/Cost/StateTransferCost.cpp"

EvaluatedControl Cost::operator()(const RVec& u)
{
    // This assumes we propagate once per state transfer
    fpp += transfer.psi_0.size();

    // Fidelity of transfer
    transfer(u);

    // Any other costs we want to add
    for (auto& component : components) {
        transfer.eval.cost += component(u);
    }

    // Return the EvaluatedControl with the cost updated
    return transfer.eval;
}

Cost operator+(ControlCost c, StateTransfer& s)
{
    return Cost(s) + c;
}

Cost operator+(Cost c, ControlCost cc)
{
    c.components.push_back(cc);
    return c;
}

Cost operator+(ControlCost cc, Cost c)
{
    c.components.push_back(cc);
    return c;
}