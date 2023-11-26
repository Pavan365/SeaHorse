#pragma once

#include "src/Physics/Vectors.hpp"

// This object contains the information from evolving a state with a control
struct EvaluatedControl {
    RVec control;
    double cost;
    double fid;
    double norm;

    inline bool operator<(const EvaluatedControl& other) const
    {
        return cost < other.cost;
    }
};
