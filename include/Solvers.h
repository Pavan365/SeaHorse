#ifndef SEAHORSE_SOLVERS
#define SEAHORSE_SOLVERS
#include "Globals.h"
#include "Hamiltonian.h"
#include <iostream>
#include <eigen/Eigen/Core>
#include <eigen/unsupported/Eigen/FFT>
#include <fftw3.h>

// General class to evolve wavefunctions: either by a single `step(u)` or multiple `evolve(control)`.
class Stepper
{
protected:
    double _dt;
    CVec _psi_0; // initial state
public:
    //Constructor
    Stepper(double dt, CVec psi_0) : _dt(dt), _psi_0(psi_0) {}

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
    std::function<RVec(double)> _V;
    CVec _T_exp;
    CVec _T_exp_2;

    // FFT and buffers
    Eigen::FFT<double> _fft;
    CVec _spacebuf;
    CVec _mombuf;

    int n = 0;

public:
    // Constructor
    SplitStepper(double dt, HamiltonianFn& H, CVec psi_0) : Stepper(dt,psi_0), _V(H.V)
    {
        if ((psi_0.norm()-1.0)>1e-10) {S_INFO("PSI_0 automatically normalised (from ",psi_0.norm(),")");}
        _spacebuf = psi_0/psi_0.norm(); // initialise with starting state
        _fft.fwd(_mombuf,_spacebuf);
        _T_exp_2 = (-0.5i * dt * H.T_p.array()).exp();
        // _T_exp = _T_exp_2.array().square(); uses e^2a = (e^a)^2 to avoid exp again
        _T_exp = (-1.0i * dt * H.T_p.array()).exp();
        n = psi_0.size();
    }

    // Discard any internal state changed to date
    void reset() { _spacebuf = _psi_0; }
    void reset(CVec psi_0)
    {
        if ((psi_0.norm()-1.0)>1e-10) {S_INFO("PSI_0 automatically normalised (from ",psi_0.norm(),")");}
        _psi_0 = psi_0/psi_0.norm();
        _spacebuf = _psi_0;
    }
    
    void step(double u) // Move forward a single step
    {
        // check the fftw plan shit going on? profiler t2_32 seems to be linked to ffting (9% of time)
        _fft.fwd(_mombuf,_spacebuf);
        _mombuf = _mombuf.array().cwiseProduct(_T_exp_2.array()).matrix();
        _fft.inv(_spacebuf,_mombuf);
        _spacebuf = _spacebuf.array().cwiseProduct((-1.0i * _dt * _V(u).array()).exp()).matrix();
        _fft.fwd(_mombuf,_spacebuf);
        _mombuf = _mombuf.array().cwiseProduct(_T_exp_2.array()).matrix();
        _fft.inv(_spacebuf,_mombuf);
    }

    // Slower to step but can save state data
    void evolve(RVec control, std::function<void(CVec)> save_data){
        for (double u : control) {
             _fft.fwd(_mombuf,_spacebuf);
            _mombuf = _mombuf.array().cwiseProduct(_T_exp_2.array()).matrix();
            _fft.inv(_spacebuf,_mombuf);
            _spacebuf = _spacebuf.array().cwiseProduct((-1.0i * _dt * _V(u).array()).exp()).matrix();
            _fft.fwd(_mombuf,_spacebuf);
            _mombuf = _mombuf.array().cwiseProduct(_T_exp_2.array()).matrix();
            _fft.inv(_spacebuf,_mombuf);

            save_data(_spacebuf);
        }
    }

    // Optimised steps but can't provide intermediate step wavefunctions
    // This combines T/2 ifft fft T/2 between steps to save computation.
    void evolve(RVec control){
        // Special treatment for the last component
        double last = control[control.size()-1];
        control.resize(control.size()-1);

        // Initial half step T/2
        _fft.fwd(_mombuf,_spacebuf);
        _mombuf = _mombuf.array().cwiseProduct(_T_exp_2.array()).matrix();

        // Main loop V,T full steps
        for (double u : control) {
            _fft.inv(_spacebuf,_mombuf);
            _spacebuf = _spacebuf.array().cwiseProduct((-1.0i * _dt * _V(u).array()).exp()).matrix();
            _fft.fwd(_mombuf,_spacebuf);
            _mombuf = _mombuf.array().cwiseProduct(_T_exp.array()).matrix();
        }

        // Finishing out the last V,T/2
        _fft.inv(_spacebuf,_mombuf);
        _spacebuf = _spacebuf.array().cwiseProduct((-1.0i * _dt * _V(last).array()).exp()).matrix();
        _fft.fwd(_mombuf,_spacebuf);
        _mombuf = _mombuf.array().cwiseProduct(_T_exp_2.array()).matrix();
        _fft.inv(_spacebuf,_mombuf);
    }

    CVec state() {return _spacebuf;}
    // Current state in shifted momentum space
    CVec state_p() { return _mombuf;}
};





#endif // SEAHORSE_SOLVERS