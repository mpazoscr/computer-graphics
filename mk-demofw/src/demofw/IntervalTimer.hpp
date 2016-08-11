#ifndef SRC_DEMOFW_INTERVALTIMER_H_
#define SRC_DEMOFW_INTERVALTIMER_H_

#include <chrono>

namespace mk
{
  namespace demofw
  {
    /**
     * Keeps track of the elapsed time between calls to {@link elapsedNano}.
     */
    class IntervalTimer
    {
    public:
      /**
       * @return The time elapsed since last time this function was called in nanoseconds.
       *
       * @note First time the function is called, it returns 0;
       */
      double tick();

    private:
      std::chrono::steady_clock::time_point mLastUpdate;
    };
  }
}

#endif  // SRC_DEMOFW_INTERVALTIMER_H_
