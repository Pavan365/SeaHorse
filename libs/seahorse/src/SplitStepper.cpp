#include "libs/seahorse/include/SplitStepper.h"

// Constructor
SplitStepper::SplitStepper() { }

SplitStepper::SplitStepper(double dt, HamiltonianFn& H, const CVec& psi_0)
    : Stepper(dt, H.hs.dx(), psi_0)
    , m_V(H.V)
{
    if ((psi_0.norm() - 1.0) > 1e-10) {
        S_INFO("PSI_0 automatically normalised (from ", psi_0.norm(), ")");
    }
    m_psi_f = psi_0 / psi_0.norm(); // initialise with starting state
    m_fft.fwd(m_mombuf, m_psi_f);
    m_T_exp_2 = (-0.5i * dt * H.T_p.array()).exp();
    // m_T_exp = m_T_exp_2.array().square(); uses e^2a = (e^a)^2 to avoid exp again
    m_T_exp = (-1.0i * dt * H.T_p.array()).exp();
}

// Discard any internal state changed to date
void SplitStepper::reset()
{
    m_psi_f = m_psi_0;
}

void SplitStepper::reset(const CVec& psi_0)
{
    if ((psi_0.norm() - 1.0) > 1e-10) {
        S_INFO("PSI_0 automatically normalised (from ", psi_0.norm(), ")");
    }
    m_psi_0 = psi_0 / psi_0.norm();
    SplitStepper::reset();
}

void SplitStepper::step(double u) // Move forward a single step
{
    m_fft.fwd(m_mombuf, m_psi_f);
    m_mombuf = m_mombuf.array().cwiseProduct(m_T_exp_2.array());
    m_fft.inv(m_psi_f, m_mombuf);
    m_psi_f = m_psi_f.array().cwiseProduct((-1.0i * m_dt * m_V(u).array()).exp());
    m_fft.fwd(m_mombuf, m_psi_f);
    m_mombuf = m_mombuf.array().cwiseProduct(m_T_exp_2.array());
    m_fft.inv(m_psi_f, m_mombuf);
}

// Optimised steps but can't provide intermediate step wavefunctions
// This combines T/2 ifft fft T/2 between steps to save computation.
void SplitStepper::evolve(const RVec& control)
{
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

    // Initial half step T/2
    m_fft.fwd(m_mombuf, m_psi_f);
    m_mombuf = m_mombuf.array().cwiseProduct(m_T_exp_2.array());

    // Main loop V,T full steps
    for (int i = 0; i < control.size() - 1; i++) {
        m_fft.inv(m_psi_f, m_mombuf);
        m_psi_f = m_psi_f.array().cwiseProduct((-1.0i * m_dt * m_V(control[i]).array()).exp());
        m_fft.fwd(m_mombuf, m_psi_f);
        m_mombuf = m_mombuf.array().cwiseProduct(m_T_exp.array());
    }

    // Finishing out the last V,T/2
    m_fft.inv(m_psi_f, m_mombuf);
    m_psi_f = m_psi_f.array().cwiseProduct((-1.0i * m_dt * m_V(control[control.size()]).array()).exp());
    m_fft.fwd(m_mombuf, m_psi_f);
    m_mombuf = m_mombuf.array().cwiseProduct(m_T_exp_2.array());
    m_fft.inv(m_psi_f, m_mombuf);

    S_INFO("Stepped ", control.size() + 1, " times in ",
        ((std::chrono::system_clock::now() - start).count()) / 1e6,
        " seconds");
}