#include "include/Optimisation/Stopper/Stopper.hpp"
#include "include/Optimisation/Optimiser.hpp"

Stopper::Stopper(StopComponent sc) { components.push_back(sc); }

Stopper Stopper::operator+(StopComponent sc)
{
    components.push_back(sc);
    return *this;
}

bool Stopper::operator()(Optimiser& opt)
{
    for (auto& component : components) {
        if (component(opt)) {
            S_INFO("STOPPING: ", component.text);
            return true;
        }
    }
    return false;
}

Stopper operator+(StopComponent lhs, StopComponent rhs)
{
    Stopper stopper(lhs);
    stopper = stopper + rhs;
    return stopper;
}

Stopper operator+(StopComponent sc, Stopper s)
{
    s = s + sc;
    return s;
}
