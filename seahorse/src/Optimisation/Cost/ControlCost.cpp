#include "include/Optimisation/Cost/ControlCost.hpp"
#include "include/Utils/Globals.hpp"

ControlCost makeRegularisation()
{
    // We normalise by the number of steps to make the penalty independent of the control size
    return ControlCost([=](const RVec& u) {
        return u.cwiseAbs2().mean();
    });
}

ControlCost makeBoundaries(double minBound, double maxBound)
{
    // We normalise by the number of steps to make the penalty independent of the control size
    return ControlCost([=](const RVec& u) { return ((u.array() - maxBound).cwiseMax(0) + (u.array() - minBound).cwiseMin(0)).cwiseAbs2().mean(); });
}

ControlCost makeBoundaries(double bound)
{
    return makeBoundaries(-bound, bound);
}

ControlCost operator*(double weight, ControlCost c)
{
    c.weight *= weight;
    return c;
}

ControlCost operator*(ControlCost c, double weight)
{
    c.weight *= weight;
    return c;
}