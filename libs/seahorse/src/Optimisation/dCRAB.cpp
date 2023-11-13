#pragma once

#include "libs/seahorse/include/Optimisation/dCRAB.h"

dCRAB::dCRAB(Basis& basis, SplitStepper& stepper, Stopper& stopper, Cost& cost,
    SaveFn saver)
    : Optimiser(stepper, stopper, cost, saver)
    , basis(std::make_unique<Basis>(basis))
{
    dCRAB::init();
}

// No saver
dCRAB::dCRAB(Basis& basis, SplitStepper& stepper, Stopper& stopper, Cost& cost)
    : Optimiser(stepper, stopper, cost, nullptr)
    , basis(std::make_unique<Basis>(basis))
{
    dCRAB::init();
}

void dCRAB::init()
{
    num_iterations = 0;

    // Initial point - random coeffs between -1 and 1
    RVec initialControl = RVec::Random(basis->size());
    cost(*stepper, basis->control(initialControl));
    EvaluatedControl eval_initialControl = { initialControl, cost.cost, cost.fid };

    simplex.push_back(eval_initialControl);

    // Simplex around the initial guess
    for (int i = 0; i < basis->size(); i++) {
        RVec temp = eval_initialControl.control + RVec::Random(basis->size()) * stepSize;
        cost(*stepper, basis->control(temp));
        simplex.push_back({ temp, cost.cost, cost.fid });
    }

    // order simplex by cost
    std::sort(simplex.begin(), simplex.end(),
        [](const auto& a, const auto& b) { return (a.cost < b.cost); });

    bestControl = { basis->control(simplex[0].control), simplex[0].cost, simplex[0].fid };
}

void dCRAB::step()
{
    num_iterations++;
    // centroid of simplex except worst point
    RVec centroid_pt = RVec::Zero(basis->size());
    for (auto it_pt = simplex.begin(); it_pt != (simplex.end() - 1); it_pt++) {
        centroid_pt += (*it_pt).control;
    }
    centroid_pt /= (simplex.size() - 1);

    // REFLECTION
    RVec reflection_pt = centroid_pt + alpha * (centroid_pt - simplex.back().control);

    // evaluate cost at reflection point
    cost(*stepper, basis->control(reflection_pt));
    EvaluatedControl reflected = { reflection_pt, cost.cost, cost.fid };

    // reflection is better than second worst point but worse than best point
    if ((simplex.front().cost <= reflected.cost) && (reflected.cost < simplex[simplex.size() - 2].cost)) {
        // S_INFO("Reflection replaces worst point");
        simplex.pop_back();
        simplex.push_back(reflected);
        return;
    }

    // EXPANSION
    // reflection is better than best point
    if (reflected.cost < simplex.front().cost) {
        RVec expansion_pt = centroid_pt + gamma * (reflection_pt - centroid_pt);
        cost(*stepper, basis->control(expansion_pt));
        EvaluatedControl expanded = { expansion_pt, cost.cost, cost.fid };

        simplex.pop_back();
        // expansion is better than reflection
        if (expanded.cost < reflected.cost) {
            // S_INFO("Expansion replaces worst point");
            simplex.push_back(expanded);
        } else {
            // S_INFO("Reflection replaces worst point after checking expansion");
            simplex.push_back(reflected);
        }
        return;
    }

    // CONTRACTION
    // reflection is better than worst point
    if (reflected.cost < simplex.back().cost) {
        RVec contraction_pt = centroid_pt + rho * (reflection_pt - centroid_pt);
        cost(*stepper, basis->control(contraction_pt));
        EvaluatedControl contracted = { contraction_pt, cost.cost, cost.fid };

        // contraction is better than reflection
        if (contracted.cost < reflected.cost) {
            // S_INFO("Contraction around reflection replaces worst point");
            simplex.pop_back();
            simplex.push_back(contracted);
            return;
        }
        // reflection worse than worst
    } else if (reflected.cost >= simplex.back().cost) {
        RVec contraction_pt = centroid_pt + rho * (simplex.back().control - centroid_pt);
        cost(*stepper, basis->control(contraction_pt));
        EvaluatedControl contracted = { contraction_pt, cost.cost, cost.fid };

        // contraction is better than worst
        if (contracted.cost < simplex.back().cost) {
            // S_INFO("Contraction around worst replaces worst point");
            simplex.pop_back();
            simplex.push_back(contracted);
            return;
        }
    }

    // SHRINK
    // shrink all points towards best point
    for (int i = 1; i < simplex.size(); i++) {
        RVec new_pt = simplex.front().control + sigma * (simplex[i].control - simplex.front().control);
        cost(*stepper, basis->control(new_pt));
        EvaluatedControl new_evaluated = { new_pt, cost.cost, cost.fid };
        simplex[i] = new_evaluated;
    }
    // S_INFO("Shrink");
    return;
}

void dCRAB::optimise() { optimise(1); }
void dCRAB::optimise(int dressings)
{
    S_INFO("dCRAB optimise using a basis size of ", basis->size(), " divided into ", dressings, " chunks");

    // Early checking of stopper and saving incase we initialise with a good control
    while (true) {
        // order simplex by cost
        std::sort(
            simplex.begin(), simplex.end(),
            [](const auto& a, const auto& b) { return (a.cost < b.cost); });

        // record best control
        bestControl = { basis->control(simplex[0].control), simplex[0].cost, simplex[0].fid };

        // save data from control
        if (saver) {
            saver(*this);
        }

        // break if we satisfy contraints
        if (stopper(*this)) {
            break;
        }

        step();
    }
    S_INFO("dCRAB finished");
}