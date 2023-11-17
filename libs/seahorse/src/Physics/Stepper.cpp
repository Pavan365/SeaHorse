#include "libs/seahorse/include/Physics/Stepper.h"

// Constructor
Stepper::Stepper() { }
Stepper::Stepper(double dt, double dx, const CVec &psi_0)
    : m_dt(dt), m_dx(dx), m_psi_0(psi_0.normalized()), m_psi_f(m_psi_0) {}

std::unique_ptr<Stepper> Stepper::clone() const { return std::unique_ptr<Stepper>(this->clone_impl()); }

const CVec Stepper::initial_state() const { return m_psi_0; };
const CVec Stepper::state() const { return m_psi_f; };

const double Stepper::dt() const { return m_dt; }
const double Stepper::dx() const { return m_dx; }
