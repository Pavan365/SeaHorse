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
    virtual CVec step(double u) = 0;
    virtual CVec evolve(RVec control) = 0;

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
    
    CVec step(double u) // Move forward a single step
    {
        _fft.fwd(_mombuf,_spacebuf);

        // _mombuf.array()*= _T_exp_2.array();
        _mombuf = _mombuf.array().cwiseProduct(_T_exp_2.array()).matrix();

        _fft.inv(_spacebuf,_mombuf);

        // _spacebuf.array()*= (-1.0i * _dt * _V(u).array()).exp().array();
        _spacebuf = _spacebuf.array().cwiseProduct((-1.0i * _dt * _V(u).array()).exp()).matrix();
        _fft.fwd(_mombuf,_spacebuf);

        // _mombuf.array()*=_T_exp_2.array();
        _mombuf = _mombuf.array().cwiseProduct(_T_exp_2.array()).matrix();

        _fft.inv(_spacebuf,_mombuf);

        return _spacebuf;
    }

    CVec evolve(RVec control){
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
        int n = control.size();

        // First half step
        _fft.fwd(_mombuf,_spacebuf);
        // _mombuf.array()*=_T_exp_2.array();
        _mombuf.array().cwiseProduct(_T_exp_2.array()).matrix();

        // Loop through main chunk for all but the last control value
        for (int i = 0; i < n - 1; i++){
            _fft.inv(_spacebuf,_mombuf);
            // NB this is expensive - can we cache it somehow?
            // _spacebuf.array()*= (-1.0i * _dt * _V(control[i]).array()).exp().array();
            _spacebuf.array().cwiseProduct((-1.0i * _dt * _V(control[i]).array()).exp()).matrix();
            _fft.fwd(_mombuf,_spacebuf);
            // _mombuf.array()*=_T_exp.array();
            _mombuf.array().cwiseProduct(_T_exp.array()).matrix();
        } 
        // Finish the last loop
        _fft.inv(_spacebuf,_mombuf);
        // _spacebuf.array()*= (-1.0i * _dt * _V(control[n-1]).array()).exp().array();
        _spacebuf.array().cwiseProduct((-1.0i * _dt * _V(control[n-1]).array()).exp()).matrix();
        _fft.fwd(_mombuf,_spacebuf); 
        // _mombuf.array()*=_T_exp_2.array();
        _mombuf.array().cwiseProduct(_T_exp_2.array()).matrix();
        _fft.inv(_spacebuf,_mombuf);

        S_INFO("Evolving for ",control.size()," steps: ",((std::chrono::system_clock::now() - start).count()) / 1e6, " seconds");
        return _spacebuf;
    }

    CVec state() {return _spacebuf;}
    // Current state in shifted momentum space
    CVec state_p() { return _mombuf;}
};





#endif // SEAHORSE_SOLVERS