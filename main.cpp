#include "seahorse/seahorse.h"
using namespace Eigen;
using namespace Spectra;

int main()
{
    std::__1::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();;



    const int dim = 1<<11;

    const auto k = sqrt(2);
    const auto xlim = PI/k/2 * 3;

    auto hs = HilbertSpace(dim,xlim);
    Hamiltonian H(hs);

    auto V0 = [&,x=hs.x()](double phase){return (VectorXd) (400 * (1-0.5 * box(1 + cos(-2 * k * (x-phase)), x-phase, -PI/k/2, PI/k/2)));};
    H.setV(V0);

    auto H0 = H(0);

    // auto eig = H.allEigs();
    auto eig = H0.spectrum(14);


    //Time evolution
    /*
    Parallelise? calc V's before doing all the steps
    Or just build it into robustness checking evolutions

    T is currently in x space as a central approx derivative.
    It should also have have option to be in p basis, so diagonal. but be careful of fftfreq

    Evolution (T(x/p)=P^2/2m,V(x)=potential), options to be:
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
    - Eigenstate evolution?
        exp(-i E_n dt) . PSI(eigenbasis)
        exp(-i H dt) . PSI(x)
        I guess with fixed potential it might be useful
    */

    // Printing the H/T/V matrices
    
    plots(hs.x(),H0.V);
    for(int i=0;i<eig.cols();i++){
        Eigen::VectorXd vect = eig.col(i)(seq(0,Eigen::placeholders::last-1));
        auto energy = eig(Eigen::placeholders::last,i);

        plt::plot(hs.x(),40*vect.array()+energy);
        plt::text(xlim,energy,std::to_string(i));
    }
    plt::savefig("output/test.pdf"); // show the figure instead of saving it
    plt::show();

    std::cout<<(start-std::chrono::high_resolution_clock::now()).count()<<": Ran "<<dim<<"x"<<dim<<std::endl;

    return 0;
}



 