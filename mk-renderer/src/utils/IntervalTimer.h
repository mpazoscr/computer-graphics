#ifndef SRC_UTILS_INTERVALTIMER_H_
#define SRC_UTILS_INTERVALTIMER_H_

#include <chrono>

namespace mk
{
  namespace utils
  {
    /**
     * Keeps track of the elapsed time between calls to {@link elapsedNano}.
     */
    class IntervalTimer
    {
    public:
      /**
       * @return The time elapsed since last time this function was called.
       *
       * @note First time the function is called, it returns 0;
       */
      float elapsedNano();

    private:
      std::chrono::steady_clock::time_point mLastUpdate;
    };
  }
}

#endif  // SRC_UTILS_INTERVALTIMER_H_
