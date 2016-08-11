#include "FFTSolver.hpp"

namespace mk
{
  namespace cuda
  {
    mk::math::FFTSolverSharedPtr createCudaFFTSolver()
    {
      return std::shared_ptr<cuda::FFTSolver>(new cuda::FFTSolver());
    }

    bool FFTSolver::fft2D(core::complex* data, int sizeX, int sizeY)
    {
      cufftComplex* cufftData = reinterpret_cast<cufftComplex*>(data);

      return CUFFT_SUCCESS == cufftExecC2C(mFFTPlanCache.getFFTPlan(sizeX, sizeY), cufftData, cufftData, CUFFT_FORWARD);
    }

    bool FFTSolver::fftInv2D(core::complex* data, int sizeX, int sizeY)
    {
      cufftComplex* cufftData = reinterpret_cast<cufftComplex*>(data);

      return CUFFT_SUCCESS == cufftExecC2C(mFFTPlanCache.getFFTPlan(sizeX, sizeY), cufftData, cufftData, CUFFT_INVERSE);
    }

    FFTSolver::FFTPlanCache::~FFTPlanCache()
    {
      for (auto& planPair : mCufftPlanMap)
      {
        cufftDestroy(planPair.second);
      }
    }

    cufftHandle FFTSolver::FFTPlanCache::getFFTPlan(int sizeX, int sizeY)
    {
      cufftHandle fftPlan;
      int64_t key = getPlanKey(sizeX, sizeY);

      if (mCufftPlanMap.find(key) != mCufftPlanMap.end())
      {
        fftPlan = mCufftPlanMap[key];
      }
      else
      {
        cufftPlan2d(&fftPlan, sizeX, sizeY, CUFFT_C2C);
        mCufftPlanMap[key] = fftPlan;
      }

      return fftPlan;
    }

    int64_t FFTSolver::FFTPlanCache::getPlanKey(int sizeX, int sizeY)
    {
      int64_t sX = static_cast<int64_t>(sizeX);
      int64_t sY = static_cast<int64_t>(sizeY);

      return sX << 32 | sY;
    }
  }
}
