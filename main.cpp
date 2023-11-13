// #include "libs/seahorse/src/seahorse.cpp" // use for full integration [slow build, but might fix weird errors]
#include "libs/seahorse/include/Utils/Globals.h"
#include "libs/seahorse/include/seahorse.h"

int main()
{
    Timer timer;
    timer.Start();

    const int dim = 1 << 11;
    const auto k = sqrt(2);
    const auto xlim = PI / k / 2 * 4;
    const double dt = 0.001;
    const int numSteps = 1e3;
    const RVec t = RVec::LinSpaced(numSteps, 0, dt * numSteps);

    auto hs = HilbertSpace(dim, xlim);
    const RVec x = hs.x();
    const auto depth = 400;

    // We MUST return an RVec not an Eigen::Expression or the memory is freed and
    // we get a segfault in some cases
    std::function<RVec(double)> V0 = [&, x](double phase) {
        return RVec(-0.5 * depth * (cos(2 * k * (x - phase)) + 1) * box(x - phase, -PI / k / 2, PI / k / 2));
    };

    HamiltonianFn H(hs, V0);
    Hamiltonian H0 = H(0);

    CVec psi_0 = H0[0] + H0[1];

    SplitStepper stepper = SplitStepper(dt, H, psi_0);
    Basis basis = Basis::TRIG(t, 5, 10);
    Stopper stopper = Stopper(0.9, 100);
    Cost cost = Cost(H0[2]);
    SaveFn saver = [](const Optimiser& opt) { S_INFO(opt.num_iterations, "\tfid= ", opt.bestControl.fid); };

    dCRAB optimiser = dCRAB(basis, stepper, stopper, cost, saver);
    optimiser.optimise(5);

    timer.Stop("Main");
    return 0;
}
