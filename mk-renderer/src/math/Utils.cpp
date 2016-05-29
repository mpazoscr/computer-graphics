#include "Utils.h"

#include <cmath>

namespace mk
{
  namespace math
  {
    double expDecayPow2(double k)
    {
      return exp(log(0.5) * k);
    }

    bool isPowerOf2(int value)
    {
      return (0 != value) && (0 == (value & (value - 1)));
    }
  }
}
