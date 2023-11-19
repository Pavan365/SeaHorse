#pragma once

#include "include/Utils/Globals.h"

// This object contains the information from evolving a state with a control
struct EvaluatedControl {
    RVec control;
    double cost;
    double fid;

    inline bool operator<(const EvaluatedControl& other) const
    {
        return cost < other.cost;
    }
};
