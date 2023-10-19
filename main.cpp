#include "seahorse.h"

int main()
{
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

    const int dim = 1<<11;

    const auto k = sqrt(2);
    const auto xlim = PI/k/2 * 3;

    auto hs = HilbertSpace(dim,xlim);
    HamiltonianFn H(hs);

    auto V0 = [&,x=hs.x()](double phase){return (RVec) (400 * (1 - 0.5 * (1 + cos(-2 * k * (x-phase))) * box(x-phase, -PI/k/2, PI/k/2)));};

    H.setV(V0);

    Hamiltonian H0 = H(0);

    auto eig = H0.spectrum(14);


//  DONT USE EIGEN::VECTOR OR MATRIX AS RVEC/RMAT/CVEC/CMAT
// USE SOMETHING LIKE QENGINE
// Lets us define scalar+vector, scalar + matrix, cos, box, etc etc




    //Time evolution
    /*
    Parallelise? calc V's before doing all the steps
    Or just build it into robustness checking evolutions

    T is currently in x space as a central approx derivative.
    It should also have have option to be in p basis, so diagonal. but be careful of fftfreq

    NB: NEED P TO BE 2*PI*fftshift(x)/(DX * DIM) before we p^2/2m ?!!!!!!

    Evolution (T(x or p)=exp(-i*dt*P^2/2m),V(x)=exp(-i*dt*potential), options to be:
    - Split step
        V(x)/2 * IFFT{ T(p) * FFT{ V(x)/2 * PSI(x)
    - Matrix multiplication with T in x basis
        V(x)/2 * T(x) * V(x)/2 * PSI(x)
        prebake the T(x) as it doesnt change
        Either matrix-wise IFFT*T(p)*FFT or central approx laplacian
        FFt automatically gives periodic boundary conditions?
        compare cost from making T once (n^3) and non-diag multiplication (n^3) vs FFT (n*log(n)) [n times] and diag multiplication (n^2?)
    - Reverse split step order
        can collapse some FFTs to optimise but slightly more to think about algorithmically
        would we ever have PSI(p) normally?
    - Higher order error reduction
        Could help if we are doing lots of stepping and prebake P^2/2m(x)
        (https://www.asc.tuwien.ac.at/~winfried/splitting/index.php?rc=0&ab=pp-34-a-ab&name=PP%203/4%20A)

    */


    std::cout<<((std::chrono::system_clock::now()-start).count())/1e6<<" seconds: Ran "<<dim<<"x"<<dim<<std::endl;

    return 0;
}



 