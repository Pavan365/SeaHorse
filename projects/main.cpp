#include "seahorse.hpp"
INCLUDE_sourceFile;

int main()
{
    srand(get_rand_seed());
    Timer timer;
    timer.Start();

    const int dim = 1 << 11;
    const auto k = sqrt(2);
    const auto xlim = PI / k / 2 * 3;
    const double dt = 0.001;
    const int numSteps = 3e3;

    auto hs = HilbertSpace(dim, xlim);
    const RVec x = hs.x();
    const auto depth = 400;

    Potential V = ShakenPotential(hs, RVec(-0.5 * depth * (cos(2 * k * x) + 1) * box(x, -PI / k / 2, PI / k / 2)));
    HamiltonianFn H(hs, V);
    Hamiltonian H0 = H(0);

    SplitStepper stepper = SplitStepper(dt, H);
    RVec randControl = RVec::Random(1e5);
    stepper.evolve(H0[0], randControl);
    Cost cost = StateTransfer(stepper, H0[0], H0[1]) + 1e3 * makeBoundaries(PI / k / 2) + 1e-6 * makeRegularisation();

    const RVec t = RVec::LinSpaced(numSteps, 0, dt * numSteps);
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
