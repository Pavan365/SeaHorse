#include "include/Optimisation/Cost/Cost.hpp"
#include "src/Optimisation/Cost/ControlCost.cpp"
#include "src/Optimisation/Cost/StateTransferCost.cpp"

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
    c.addCost(cc);
    return c;
}

Cost operator+(ControlCost cc, Cost c)
{
    c.addCost(cc);
    return c;
}