// #include "libs/seahorse/src/seahorse.cpp" // use for full integration [slow build, but might fix weird errors]
#include "libs/seahorse/seahorse.h"

int main()
{

    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

    const int dim = 1 << 11;
    const auto k = sqrt(2);
    const auto xlim = PI / k / 2 * 4;
    const double dt = 0.001;

    auto hs = HilbertSpace(dim, xlim);
    const RVec x = hs.x();
    const auto depth = 400;

    // // DO NOT USE AUTO HERE OR THE VECTOR DATA IS FREED INSIDE THE SCOPE!
    // std::function<RVec(double)> V0 = [&, x](double phase){return -0.5 * depth * (cos(2 * k * (x - phase)) + 1) * box(x - phase, -PI / k / 2, PI / k / 2);};
    std::function<RVec(double)> V0 = [&, x](double phase){return -0.5 * depth * (cos(2 * k * (x - phase)) + 1) ;};

    HamiltonianFn H(hs,V0);
    Hamiltonian H0 = H(0);

    CVec psi_0 = H0[0]+H0[1];
    SplitStepper stepper = SplitStepper(dt,H,psi_0);

    stepper.evolve(RVec::Zero(1e4));
    S_INFO(((std::chrono::system_clock::now() - start).count()) / 1e6, " seconds");

    return 0;
}
