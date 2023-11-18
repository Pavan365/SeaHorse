// #include "libs/seahorse/src/seahorse.cpp" // use for full integration [slow build, but might fix weird errors]
#include "libs/seahorse/include/seahorse.h"

// We include the file as `unsigned char sourceFile[]`
#include "main.cpp.rawtext"

int main()
{
    Timer timer;
    timer.Start();

    const int dim = 1 << 10;
    const auto k = sqrt(2);
    const auto xlim = PI / k / 2 * 4;
    const double dt = 0.001;
    const int numSteps = 3e3;
    const RVec t = RVec::LinSpaced(numSteps, 0, dt * numSteps);

    auto hs = HilbertSpace(dim, xlim);
    const RVec x = hs.x();
    const auto depth = 400;

    // We MUST return an RVec not an Eigen::Expression or the memory is freed and
    // we get a segfault in some cases
    std::function<RVec(double)> V0 = [=](double phase) {
        return RVec(-0.5 * depth * (cos(2 * k * (x - phase)) + 1) * box(x - phase, -PI / k / 2, PI / k / 2));
    };

    HamiltonianFn H(hs, V0);
    Hamiltonian H0 = H(0);

    CVec psi_0 = H0[0] + H0[1];

    SplitStepper stepper = SplitStepper(dt, H, psi_0);

    Cost cost = StateTransfer(stepper, psi_0, H0[0]) + 1e-10 * makeBoundaries(-1, 1) + 1e-10 * makeRegularisation();

    Basis basis = Basis::TRIG(t, 8.5, 10);

    Stopper stopper = FidStopper(0.99) + IterStopper(100) + StallStopper(20);

    SaveFn saver = [](const Optimiser& opt) {
        S_INFO(opt.num_iterations, "\tfid= ", opt.bestControl.fid);
    };

    dCRAB optimiser = dCRAB(basis, stopper, cost, saver);
    optimiser.optimise(5);

    timer.Stop("(Main)");
    return 0;
}
