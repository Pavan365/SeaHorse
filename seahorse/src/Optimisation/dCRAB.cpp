#include "include/Optimisation/dCRAB.hpp"
#include "src/Utils/Logger.hpp"

dCRAB::dCRAB(Basis& basis, Stopper& stopper, Cost& cost,
    SaveFn saver)
    : Optimiser(stopper, cost, saver)
    , basis(std::make_unique<Basis>(basis))
{
    dCRAB::init();
}

// No saver
dCRAB::dCRAB(Basis& basis, Stopper& stopper, Cost& cost)
    : dCRAB(basis, stopper, cost, nullptr)
{
}

void dCRAB::init()
{
    num_iterations = 0;
    steps_since_improvement = 0;

    // Initial point - random coeffs between -1 and 1
    // FIXME: Bases should probably provide their own control guesses - maybe a whole simplex of them?
    RVec initialControl = RVec::Random(basis->size());
    bestControl = cost(basis->control(initialControl));

    SimplexPoint eval_initialControl = { initialControl, bestControl.cost };
    simplex.push_back(eval_initialControl);

    // Rest of the simplex around the initial guess (n+1 points for n-dim basis)
    for (auto _ = 0; _ < basis->size(); _++) {
        RVec temp = RVec::Random(basis->size()) * stepSize;
        auto eval = cost(basis->control(temp + initialControl));
        simplex.push_back({ temp, eval.cost });
        updateBest(eval);
    }
}

void dCRAB::step()
{
    num_iterations++;
    steps_since_improvement++;
    // centroid of simplex except worst point
    RVec centroid_pt = RVec::Zero(basis->size());
    for (auto it_pt = simplex.begin(); it_pt != (simplex.end() - 1); it_pt++) {
        centroid_pt += (*it_pt).coeffs;
    }
    centroid_pt /= (simplex.size() - 1);

    // REFLECTION
    RVec reflection_pt = centroid_pt + alpha * (centroid_pt - simplex.back().coeffs);

    // evaluate cost at reflection point
    auto eval = cost(basis->control(reflection_pt));
    SimplexPoint reflected = { reflection_pt, eval.cost };

    // reflection is better than second worst point but worse than best point
    if ((simplex.front() < reflected) && (reflected < simplex[simplex.size() - 2])) {
        // S_LOG("Reflection replaces worst point");
        simplex.pop_back();
        simplex.push_back(reflected);
        updateBest(eval);
        return;
    }

    // EXPANSION
    // reflection is better than best point
    if (reflected < simplex.front()) {
        // we need to save this as we are about to evolve again
        EvaluatedControl reflectedEC = eval;

        RVec expansion_pt = centroid_pt + gamma * (reflection_pt - centroid_pt);
        auto eval = cost(basis->control(expansion_pt));
        SimplexPoint expanded = { expansion_pt, eval.cost };

        simplex.pop_back();
        // expansion is better than reflection
        if (expanded < reflected) {
            // S_LOG("Expansion replaces worst point");
            simplex.push_back(expanded);
            updateBest(eval);
        } else {
            // S_LOG("Reflection replaces worst point after checking expansion");
            simplex.push_back(reflected);
            updateBest(reflectedEC);
        }
        return;
    }

    // CONTRACTION
    // reflection is better than worst point
    if (reflected < simplex.back()) {
        RVec contraction_pt = centroid_pt + rho * (reflection_pt - centroid_pt);
        auto eval = cost(basis->control(contraction_pt));
        SimplexPoint contracted = { contraction_pt, eval.cost };

        // contraction is better than reflection
        if (contracted < reflected) {
            // S_LOG("Contraction around reflection replaces worst point");
            simplex.pop_back();
            simplex.push_back(contracted);
            updateBest(eval);
            return;
        }
        // reflection worse than worst
    } else if (simplex.back() < reflected) {
        RVec contraction_pt = centroid_pt + rho * (simplex.back().coeffs - centroid_pt);
        auto eval = cost(basis->control(contraction_pt));
        SimplexPoint contracted = { contraction_pt, eval.cost };

        // contraction is better than worst
        if (contracted < simplex.back()) {
            // S_LOG("Contraction around worst replaces worst point");
            simplex.pop_back();
            simplex.push_back(contracted);
            updateBest(eval);
            return;
        }
    }

    // SHRINK
    // shrink all points towards best point
    for (size_t i = 1; i < simplex.size(); i++) {
        RVec new_pt = simplex.front().coeffs + sigma * (simplex[i].coeffs - simplex.front().coeffs);
        auto eval = cost(basis->control(new_pt));
        simplex[i] = { new_pt, eval.cost };
        updateBest(eval);
    }
    // S_LOG("Shrink");
    return;
}

void dCRAB::optimise() { optimise(1); }
void dCRAB::optimise(int dressings)
{
    S_LOG("dCRAB optimise using a basis size of ", basis->size(), " divided into ", dressings, " chunks");

    // Early checking of stopper and saving incase we initialise with a good control
    while (true) {
        // order simplex by cost
        std::sort(
            simplex.begin(), simplex.end(),
            [](const auto& a, const auto& b) { return (a.cost < b.cost); });

        // update our number of full path propagations
        fpp = cost.fpp;

        // save data from control if we have a saver
        if (saver) {
            saver(*this);
        }

        // break if we satisfy contraints
        if (stopper(*this)) {
            break;
        }

        step();
    }
    S_LOG("dCRAB finished with {", num_iterations, " iters, ", fpp, " fpps, ",
        bestControl.fid, " fid, ", bestControl.norm, " norm, ", bestControl.cost, " cost}");
}

void dCRAB::updateBest(EvaluatedControl& newBest)
{
    if (newBest < bestControl) {
        bestControl = newBest;
        steps_since_improvement = 0;
    }
}