#ifndef SRC_MATH_UTILS_H_
#define SRC_MATH_UTILS_H_

#include <cmath>

namespace mk
{
  namespace math
  {
    const float kFloatEpsilon = 1e-12f;
    const float kPi = 3.14159265359f;
    const float kSqrtOfTwo = static_cast<float>(sqrt(2.0));
    const float kSqrtOfHalf = 1.0f / kSqrtOfTwo;

    /**
     * @return 0.5^k
     */
    double expDecayPow2(double k);

    /**
     * @return True if value is a power of 2, false otherwise.
     */
    bool isPowerOf2(int value);
  }
}

#endif  // SRC_MATH_UTILS_H_
