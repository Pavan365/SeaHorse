#pragma once

#include <iostream>

#include "libs/seahorse/include/Physics/Hamiltonian.h"
#include "libs/seahorse/include/Utils/Globals.h"

#include <libs/eigen/Eigen/Core>
#include <libs/eigen/unsupported/Eigen/FFT>

#include <fftw3.h>

// General class to evolve wavefunctions: either by a single `step(u)` or multiple `evolve(control)`.
class Stepper {

  protected:
    // Implementing the clone pattern within derived classes
    virtual Stepper* clone_impl() const = 0;
    double m_dt = 0;
    double m_dx = 0;
    CVec m_psi_0; // initial state
    CVec m_psi_f; // current state

public:
    // Constructor
    Stepper();
    Stepper(double dt, double dx, const CVec& psi_0);

    virtual ~Stepper() {};
  // implementing the clone pattern
    std::unique_ptr<Stepper> clone() const;

    // Revert any internal state to orignal state or to new psi_0
    virtual void reset() = 0;
    virtual void reset(const CVec& psi_0) = 0;

    // Evolve by a step or a number of steps
    virtual void step(double u) = 0;
    virtual void evolve(const RVec& control) = 0;

    // Return the initial state
    const CVec initial_state() const;
    const CVec state() const;
    const double dt() const;
    const double dx() const;
};