#pragma once

#include "include/Optimisation/Cost/EvaluatedControl.hpp"
#include "include/Physics/Stepper.hpp"
#include "include/Utils/Globals.hpp"

template <typename T>
concept Steppable = std::is_base_of<Stepper, T>::value;

// Represents a fidelity cost function which can map multiple initial states to their desired final states
class StateTransfer {
public:
    friend class Cost;

private:
    EvaluatedControl eval;

    std::unique_ptr<Stepper> stepper;
    std::vector<CVec> psi_0;
    std::vector<CVec> psi_t;

public:
    // NB: This is not a true fidelity as we just average the fidelities of each
    // transfer. But it's good enough for our purposes.
    double operator()(const RVec&);

    // Copy constructor
    StateTransfer(const StateTransfer& other)
        : eval(other.eval)
        , stepper(other.stepper->clone())
        , psi_0(other.psi_0)
        , psi_t(other.psi_t) {};

    // Copy assignment operator
    StateTransfer& operator=(const StateTransfer& other);

    // Explicitly defaulted Destructor
    ~StateTransfer() = default;
    // Templated constructor to allow for any type of stepper
    template <Steppable T>
    StateTransfer(T stepper, std::vector<CVec> psi_0s,
        std::vector<CVec> psi_ts)
    {
        this->stepper = std::make_unique<T>(stepper);
        this->psi_0 = psi_0s;
        this->psi_t = psi_ts;
        for (auto i = 0; i < psi_0.size(); i++) {
            this->psi_0[i].normalize();
            this->psi_t[i].normalize();
        }
    }

    // Templated constructor to allow for any type of stepper with single state transfer
    template <Steppable T>
    StateTransfer(T stepper, CVec psi_0, CVec psi_t)
        : StateTransfer(stepper, std::vector { psi_0 }, std::vector { psi_t })
    {
    }
};
