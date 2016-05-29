#ifndef SRC_GPGPU_CUDA_FFTSOLVER_H_
#define SRC_GPGPU_CUDA_FFTSOLVER_H_

#include <unordered_map>
#include <cufft.h>

#include "math/IFFTSolver.h"

namespace mk
{
  namespace cuda
  {
    /**
     * Instantiates a FFT solver that uses Cuda FFT to perform inplace FFT in the currently selected CUDA device.
     * @return A pointer to the Cuda FFT solver.
     */
    mk::math::FFTSolverSharedPtr createCudaFFTSolver();

    /**
     * FFT solver that uses Cuda FFT to perform inplace FFT in the currently selected CUDA device.
     */
    class FFTSolver : public math::IFFTSolver
    {
    public:
      /**
       * Virtual destructor.
       */
      virtual ~FFTSolver() {}

    public: // from math::IFFTSolver
      /**
       * Please refer to {@link IFFTSolver#fft2D}
       */
      virtual bool fft2D(core::complex* data, int sizeX, int sizeY);

      /**
       * Please refer to {@link IFFTSolver#fftInv2D}
       */
      virtual bool fftInv2D(core::complex* data, int sizeX, int sizeY);

    private:
      /**
       * Internal class used to cache FFT plans
       */
      class FFTPlanCache
      {
      public:
        /**
         * Destroys all cached FFT plans.
         */
        ~FFTPlanCache();

        cufftHandle getFFTPlan(int sizeX, int sizeZ);

      private:
        int64_t getPlanKey(int sizeX, int sizeZ);

      private:
        std::unordered_map<int64_t, cufftHandle> mCufftPlanMap;
      };

    private:
      FFTPlanCache mFFTPlanCache;
    };
  }
}

#endif  // SRC_GPGPU_CUDA_FFTSOLVER_H_
