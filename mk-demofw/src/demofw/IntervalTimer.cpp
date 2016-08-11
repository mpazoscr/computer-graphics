#include "IntervalTimer.hpp"

#include <unordered_map>
#include <chrono>

namespace mk
{
  namespace demofw
  {
    double IntervalTimer::tick()
    {
      double elapsed = 0.0f;

      std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

      if (std::chrono::steady_clock::time_point() != mLastUpdate)
      {
        elapsed = static_cast<double>((now - mLastUpdate).count());
      }

      mLastUpdate = now;

      return elapsed;
    }
  }
}
