#pragma once

#include "libs/seahorse/include/Utils/Globals.h"

// Represents a cost component that is only a function of the control
class ControlCost {
public:
    friend class Cost;

    double weight = 1;

    ControlCost(std::function<double(const RVec&)> cost)
        : cost(std::move(cost)) {};

    inline double operator()(const RVec& u) const { return weight * cost(u); };

private:
    std::function<double(const RVec&)> cost;
};

ControlCost operator*(double weight, ControlCost c);
ControlCost operator*(ControlCost c, double weight);

ControlCost makeRegularisation();
ControlCost makeBoundaries(double minBound, double maxBound);
ControlCost makeBoundaries(double bound);