#include "IntervalTimer.h"

#include <unordered_map>
#include <chrono>

namespace mk
{
  namespace utils
  {
    float IntervalTimer::elapsedNano()
    {
      float elapsed = 0.0f;

      std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

      if (std::chrono::steady_clock::time_point() != mLastUpdate)
      {
        elapsed = static_cast<float>((now - mLastUpdate).count());
      }

      mLastUpdate = now;

      return elapsed;
    }
  }
}
