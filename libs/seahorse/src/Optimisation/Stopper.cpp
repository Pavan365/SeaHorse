#pragma once

#include "libs/seahorse/include/Optimisation/Stopper.h"

// Stopper
Stopper::Stopper(double fid, int num_iters)
    : fid(fid)
    , num_iters(num_iters) {};

bool Stopper::operator()(Optimiser& opt)
{
    return opt.bestControl.fid > fid || opt.num_iters() > num_iters;
};
