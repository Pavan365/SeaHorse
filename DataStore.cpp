/*
    We want to hold on to various types of data:
        - String

        - Double
        - Int
        - Complex<Double>

        - RVec
        - CVec

        - RMat
        - CMat

    Maybe https://github.com/stephenberry/glaze
    Apparently can integrate to eigen too

        // struct containing the data we want to write to datastore on each step
        struct stateData 
        {
            stateData(CVec psi, ...)
            {
                // do the maths to get the data
            }
            Cvec psi;
            double norm;
            RVec psi_abs2;
            ...

            AUTO_GLAZE...
        }
        
        // then pass it out to an overall store



    We want to store/load data fast:
        - open/close not streams


*/ 

struct DataStore
{
    
};