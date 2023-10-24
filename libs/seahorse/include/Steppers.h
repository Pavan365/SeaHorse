#pragma once

#include "seahorse/src/Globals.cpp"
#include "Hamiltonian.h"
#include <iostream>
#include <eigen/Eigen/Core>
#include <eigen/unsupported/Eigen/FFT>
#include <fftw3.h>

// General class to evolve wavefunctions: either by a single `step(u)` or multiple `evolve(control)`.
class Stepper
{
protected:
    double m_dt;
    CVec m_psi_0; // initial state
public:
    //Constructor
    Stepper(double dt, CVec psi_0);

    // Revert any internal state to orignal state or to new psi_0
    virtual void reset() = 0;
    virtual void reset(CVec psi_0) = 0;

    // Evolve by a step or a number of steps and return the final wavefunction
    virtual void step(double u) = 0;
    virtual void evolve(RVec control) = 0;

    // Return the current state
    virtual CVec state() = 0;
    // Return the current state in shifted mom space
    virtual CVec state_p() = 0;
};

class SplitStepper : Stepper
{
private:
    // V is defined on real space, and is the actual potential
    // T is defined on shifted frequency space and is the kinetic energy operator
    std::function<RVec(double)> m_V;
    CVec m_T_exp;
    CVec m_T_exp_2;

    // FFT and buffers
    Eigen::FFT<double> m_fft;
    CVec m_spacebuf;
    CVec m_mombuf;

    int n = 0;

public:
    // Constructor
    SplitStepper(double dt, HamiltonianFn& H, CVec psi_0);

    // Discard any internal state changed to date
    void reset();
    void reset(CVec psi_0);
    
    void step(double u);

    // Slower to step but can save state data
    void evolve(RVec control, std::function<void(CVec)> save_data);

    // Optimised steps but can't provide intermediate step wavefunctions
    // This combines T/2 ifft fft T/2 between steps to save computation.
    void evolve(RVec control);

    CVec state();
    // Current state in shifted momentum space
    CVec state_p();
};