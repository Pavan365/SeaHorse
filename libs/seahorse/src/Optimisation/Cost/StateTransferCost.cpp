#include "libs/seahorse/include/Optimisation/Cost/StateTransferCost.h"

double StateTransfer::operator()(const RVec& u)
{
    double temp = 0;
    for (auto i = 0; i < psi_0.size(); i++) {
        stepper->reset(psi_0[i]);
        stepper->evolve(u);
        temp += fidelity(psi_t[i], stepper->state());
    }

    // We want to maximise the fidelity, so the cost is the negative of the
    // fidelities. Fid is the mean fidelity of transfers
    eval = { .control = u, .cost = -temp, .fid = temp / psi_0.size() };

    return eval.fid;
};

StateTransfer& StateTransfer::operator=(const StateTransfer& other)
{
    eval = other.eval;
    psi_0 = other.psi_0;
    psi_t = other.psi_t;

    stepper = other.stepper->clone();

    return *this;
}