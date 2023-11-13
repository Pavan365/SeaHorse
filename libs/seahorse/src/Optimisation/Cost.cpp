#pragma once

#include "libs/seahorse/include/Optimisation/Cost.h"

// Cost
Cost::Cost(CVec psi_t)
    : psi_t(psi_t)
{
}

void Cost::operator()(Stepper& stepper, const RVec& u)
{
    stepper.reset();
    stepper.evolve(u);

    // Storing to fetch later if we want to
    fid = fidelity(psi_t, stepper.state());

    // cost is fidelity with slight regularisation and penalty for controls exceeding 1
    // cost = fid + u.squaredNorm() * 1e-5 + (u.cwiseAbs().maxCoeff() > 1);

    cost = 1 - fid;
}
