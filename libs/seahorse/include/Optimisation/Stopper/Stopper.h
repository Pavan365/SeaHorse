#pragma once

#include "libs/seahorse/include/Optimisation/Stopper/StopComponent.h"
#include "libs/seahorse/include/Utils/Globals.h"

class Optimiser;

// Add StopComponents together to get Stopper
class Stopper {
private:
    std::vector<StopComponent> components;

public:
    // Constructor - from StopComponent
    Stopper(StopComponent sc);

    // Add StopComponent to Stopper
    Stopper operator+(StopComponent sc);

    // Check if any component wants to stop
    bool operator()(Optimiser& opt);
};

Stopper operator+(StopComponent lhs, StopComponent rhs);
Stopper operator+(StopComponent sc, Stopper s);