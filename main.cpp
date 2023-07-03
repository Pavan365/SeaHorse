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
    T is currently in x space as a central approx derivative.
    It should also have have option to be in p basis, so diagonal. but be careful of fftfreq

    Evolution, options to be:
    - Split step -> X/2 * IFFT[ P * FFT[ X/2 * PSI(x) ]]
    - Matrix multiplication with P in x basis. X/2 * P * X/2 * PSI(x)
    - Which of X/P is half stepped
    - Higher order error reduction (https://www.asc.tuwien.ac.at/~winfried/splitting/index.php?rc=0&ab=pp-34-a-ab&name=PP%203/4%20A)

    Take V function and T matrix for evolution
    Step updating psi with dt duration
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



 