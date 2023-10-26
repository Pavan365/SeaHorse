#include "seahorse/include/Steppers.h"

//Constructor
Stepper:: Stepper(double dt, CVec psi_0) : m_dt(dt), m_psi_0(psi_0) {}


// Constructor
SplitStepper::SplitStepper(double dt, HamiltonianFn& H, CVec psi_0) : Stepper(dt,psi_0), m_V(H.V)
{
    if ((psi_0.norm()-1.0)>1e-10) {S_INFO("PSI_0 automatically normalised (from ",psi_0.norm(),")");}
    m_spacebuf = psi_0/psi_0.norm(); // initialise with starting state
    m_fft.fwd(m_mombuf,m_spacebuf);
    m_T_exp_2 = (-0.5i * dt * H.T_p.array()).exp();
    // m_T_exp = m_T_exp_2.array().square(); uses e^2a = (e^a)^2 to avoid exp again
    m_T_exp = (-1.0i * dt * H.T_p.array()).exp();
    n = psi_0.size();
}

// Discard any internal state changed to date
void SplitStepper::reset() { m_spacebuf = m_psi_0; }
void SplitStepper::reset(CVec psi_0)
{
    if ((psi_0.norm()-1.0)>1e-10) {S_INFO("PSI_0 automatically normalised (from ",psi_0.norm(),")");}
    m_psi_0 = psi_0/psi_0.norm();
    SplitStepper::reset();
}

void SplitStepper::step(double u) // Move forward a single step
{
    // check the fftw plan shit going on? profiler t2_32 seems to be linked to ffting (9% of time)
    m_fft.fwd(m_mombuf,m_spacebuf);
    m_mombuf = m_mombuf.array().cwiseProduct(m_T_exp_2.array()).matrix();
    m_fft.inv(m_spacebuf,m_mombuf);
    m_spacebuf = m_spacebuf.array().cwiseProduct((-1.0i * m_dt * m_V(u).array()).exp()).matrix();
    m_fft.fwd(m_mombuf,m_spacebuf);
    m_mombuf = m_mombuf.array().cwiseProduct(m_T_exp_2.array()).matrix();
    m_fft.inv(m_spacebuf,m_mombuf);
}

// Slower to step but can save state data about each step
void SplitStepper::evolve(RVec control, std::function<void(CVec)> save_data){
    for (double u : control) {
            m_fft.fwd(m_mombuf,m_spacebuf);
        m_mombuf = m_mombuf.array().cwiseProduct(m_T_exp_2.array()).matrix();
        m_fft.inv(m_spacebuf,m_mombuf);
        m_spacebuf = m_spacebuf.array().cwiseProduct((-1.0i * m_dt * m_V(u).array()).exp()).matrix();
        m_fft.fwd(m_mombuf,m_spacebuf);
        m_mombuf = m_mombuf.array().cwiseProduct(m_T_exp_2.array()).matrix();
        m_fft.inv(m_spacebuf,m_mombuf);

        save_data(m_spacebuf);
    }
}

// Optimised steps but can't provide intermediate step wavefunctions
// This combines T/2 ifft fft T/2 between steps to save computation.
void SplitStepper::evolve(RVec control){
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
    // Special treatment for the last component
    double last = control[control.size()-1];
    control.resize(control.size()-1);
    
    // Initial half step T/2
    m_fft.fwd(m_mombuf,m_spacebuf);
    m_mombuf = m_mombuf.array().cwiseProduct(m_T_exp_2.array()).matrix();

    // Main loop V,T full steps
    for (double u : control) {
        m_fft.inv(m_spacebuf,m_mombuf);
        m_spacebuf = m_spacebuf.array().cwiseProduct((-1.0i * m_dt * m_V(u).array()).exp()).matrix();
        m_fft.fwd(m_mombuf,m_spacebuf);
        m_mombuf = m_mombuf.array().cwiseProduct(m_T_exp.array()).matrix();
    }

    // Finishing out the last V,T/2
    m_fft.inv(m_spacebuf,m_mombuf);
    m_spacebuf = m_spacebuf.array().cwiseProduct((-1.0i * m_dt * m_V(last).array()).exp()).matrix();
    m_fft.fwd(m_mombuf,m_spacebuf);
    m_mombuf = m_mombuf.array().cwiseProduct(m_T_exp_2.array()).matrix();
    m_fft.inv(m_spacebuf,m_mombuf);

S_INFO("Stepped ",control.size()+1," times in ",((std::chrono::system_clock::now() - start).count()) / 1e6, " seconds");

}

CVec SplitStepper::state() {return m_spacebuf;}
// Current state in shifted momentum space
CVec SplitStepper::state_p() { return m_mombuf;}