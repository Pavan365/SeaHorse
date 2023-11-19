#include "include/Physics/Stepper.hpp"

// Constructor
Stepper::Stepper() { }
Stepper::Stepper(double dt, double dx)
    : m_dt(dt)
    , m_dx(dx)
    , m_psi_f(CVec::Zero(1))
{
}

std::unique_ptr<Stepper> Stepper::clone() const { return std::unique_ptr<Stepper>(this->clone_impl()); }

const CVec Stepper::state() const { return m_psi_f; };

const double Stepper::dt() const { return m_dt; }
const double Stepper::dx() const { return m_dx; }
