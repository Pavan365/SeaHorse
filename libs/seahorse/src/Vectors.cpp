/*
    NOTE: Because we need to use this as both declaration and definitions everything must
    be static/inline to prevent the symbols being duplicated when linking againts libseahorse.
*/

#include "seahorse/include/Vectors.h"
#include "seahorse/include/Globals.h"

inline const auto exp(const RVec &v) { return v.array().exp(); }
inline const auto exp(const CVec &v) { return v.array().exp(); }
inline const auto abs2(const RVec &v) { return v.array().abs2(); }
inline const auto abs2(const CVec &v) { return v.array().abs2(); }

// These only really make sense on the Reals
inline const auto cos(const RVec &v) { return v.array().cos(); }
inline const auto sin(const RVec &v) { return v.array().sin(); }
inline const auto box(const RVec &x, double min, double max)
{
    return (x.array().cwiseLess(max).cast<double>() + x.array().cwiseGreater(min).cast<double>()) - 1;
}
