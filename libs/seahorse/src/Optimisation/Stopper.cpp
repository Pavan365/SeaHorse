#include "libs/seahorse/include/Optimisation/Stopper.h"

// Stopper
Stopper::Stopper(double fid, int num_iters, int steps_since_improvement)
    : fid(fid)
    , num_iters(num_iters)
    , steps_since_improvement(steps_since_improvement) {};

bool Stopper::operator()(Optimiser& opt)
{
    if (opt.bestControl.fid > fid) {
        S_INFO("Stopping: Fidelity reached (", opt.bestControl.fid, " > ", fid,")");
        return true;
    }
    if (opt.num_iterations >= num_iters) {
        S_INFO("Stopping: Max iterations reached (", opt.num_iterations, " > ", num_iters, ")");
        return true;
    }
    if (opt.steps_since_improvement >= steps_since_improvement) {
        S_INFO("Stopping: Steps since improvement reached (", opt.steps_since_improvement, " > ", steps_since_improvement, ")");
        return true;
    }
    return false;
};
