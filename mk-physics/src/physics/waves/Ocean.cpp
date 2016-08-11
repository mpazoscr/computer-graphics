#include "Ocean.hpp"

#include <cmath>
#include <cassert>
#include <random>
#include <glm/glm.hpp>
#include <cuda.h>
#include <cufft.h>

#include "cuda/Ocean_cu.hpp"
#include "cuda/FFTSolver.hpp"
#include "math/Utils.hpp"

namespace mk
{
  namespace physics
  {
    namespace
    {
      const glm::vec2 kWindDir(1.0f, 0.0f);
      const float kWindSpeed(100.0f);
      const float kGravity(9.8f);
      const float kAmplitude(2.0e-5f);
      const float kCrossWindDampingCoefficient(1.0f);
      const float kSmallWavesDampingCoefficient(0.0000001f);
      const float kDisplacementFactor(-1.3f);
    }

    Ocean::Ocean(mesh::RectPatch<core::VertexPN>& rectPatch, float lengthX, float lengthZ)
    : mRectPatch(rectPatch),
      mDevGpuSpectrum(mRectPatch.n() * mRectPatch.m() * sizeof(core::complex)),
      mDevH0(mRectPatch.n() *  mRectPatch.m() * sizeof(core::complex)),
      mDevDispX(mRectPatch.n() *  mRectPatch.m() * sizeof(core::complex)),
      mDevDispZ(mRectPatch.n() *  mRectPatch.m() * sizeof(core::complex)),
      mDevGradX(mRectPatch.n() *  mRectPatch.m() * sizeof(core::complex)),
      mDevGradZ(mRectPatch.n() *  mRectPatch.m() * sizeof(core::complex)),
      mDevVbo(),
      mFFTSolver(mk::cuda::createCudaFFTSolver()),
      mLengthX(lengthX),
      mLengthZ(lengthZ),
      mWindDir(kWindDir),
      mWindSpeed(kWindSpeed),
      mGravity(kGravity),
      mAmplitude(kAmplitude),
      mCrossWindDampingCoefficient(kCrossWindDampingCoefficient),
      mSmallWavesDampingCoefficient(kSmallWavesDampingCoefficient),
      mDisplacementFactor(kDisplacementFactor)
    {
      bool sizeXIsPowerO2 = math::isPowerOf2(mRectPatch.n());
      bool sizeZIsPowerO2 = math::isPowerOf2(mRectPatch.m());

      assert(sizeXIsPowerO2 && "Ocean grid size X is not a power of 2");
      assert(sizeZIsPowerO2 && "Ocean grid size Z is not a power of 2");

      precomputeH0();
    }

    void Ocean::update(float t)
    {
      // Generate spectrum in GPU

      cuda::calculateSpectrum(mRectPatch.n(), mRectPatch.m(), mLengthX, mLengthZ, mGravity, t, mDevH0.ptr(), mDevGpuSpectrum.ptr(), mDevDispX.ptr(), mDevDispZ.ptr(), mDevGradX.ptr(), mDevGradZ.ptr());

      // Perform FFT

      mFFTSolver->fftInv2D(mDevGpuSpectrum.ptr(), mRectPatch.n(), mRectPatch.m());
      mFFTSolver->fftInv2D(mDevDispX.ptr(), mRectPatch.n(), mRectPatch.m());
      mFFTSolver->fftInv2D(mDevDispZ.ptr(), mRectPatch.n(), mRectPatch.m());
      mFFTSolver->fftInv2D(mDevGradX.ptr(), mRectPatch.n(), mRectPatch.m());
      mFFTSolver->fftInv2D(mDevGradZ.ptr(), mRectPatch.n(), mRectPatch.m());

      // Update mesh in GPU

      mDevVbo.bindGlBuffer(mRectPatch.getVboId());

      cuda::updateMesh(mRectPatch.n(), mRectPatch.m(), mDisplacementFactor, mDevGpuSpectrum.ptr(), mDevDispX.ptr(), mDevDispZ.ptr(), mDevVbo.ptr());
      cuda::updateNormals(mRectPatch.n(), mRectPatch.m(), mDevGradX.ptr(), mDevGradZ.ptr(), mDevVbo.ptr());

      mDevVbo.unbindGlBuffer(mRectPatch.getVboId());
    }

    void Ocean::setWindDir(glm::vec2 windDir)
    {
      assert(glm::length(windDir) > 0.0 && "Ocean wind direction vector has zero length");

      mWindDir = glm::normalize(windDir);
    }

    void Ocean::setWindSpeed(float windSpeed)
    {
      mWindSpeed = windSpeed;
    }

    void Ocean::setGravity(float gravity)
    {
      mGravity = gravity;
    }

    void Ocean::setAmplitude(float amplitude)
    {
      mAmplitude = amplitude;
    }

    void Ocean::setCrossWindDampingCoefficient(float crossWindDampingCoefficient)
    {
      mCrossWindDampingCoefficient = crossWindDampingCoefficient;
    }

    void Ocean::setSmallWavesDampingCoefficient(float smallWavesDampingCoefficient)
    {
      mSmallWavesDampingCoefficient = smallWavesDampingCoefficient;
    }

    void Ocean::setDisplacementFactor(float displacementFactor)
    {
      mDisplacementFactor = displacementFactor;
    }

    glm::vec2 Ocean::kVector(int x, int z)
    {
      return glm::vec2((x - mRectPatch.n() / 2.0f) * (2.0f * math::kPi / mLengthX),
                       (z - mRectPatch.m() / 2.0f) * (2.0f * math::kPi / mLengthZ));
    }

    float Ocean::phillipsSpectrum(const glm::vec2& k)
    {
      const float lengthK = glm::length(k);
      const float lengthKSquared = lengthK * lengthK;
      const float dotKWind = glm::dot(k / lengthK , mWindDir);
      const float L = mWindSpeed * mWindSpeed / mGravity;

      float phillips =  mAmplitude * expf(-1.0f / (lengthKSquared * L * L)) * dotKWind * dotKWind / (lengthKSquared * lengthKSquared);

      if (dotKWind < 0.0f)
      {
        phillips *= mCrossWindDampingCoefficient;
      }

      return phillips * expf(-lengthKSquared * L * L * mSmallWavesDampingCoefficient);
    }

    void Ocean::precomputeH0()
    {
      std::vector<core::complex> h0(mRectPatch.n() * mRectPatch.m());

      std::random_device randomDev;
      std::mt19937 randomGen(randomDev());
      std::normal_distribution<float> normalDist(0.0f, 1.0f);

      for (int z = 0; z < mRectPatch.m(); ++z)
      for (int x = 0; x < mRectPatch.n(); ++x)
      {
        int index = z * mRectPatch.n() + x;

        glm::vec2 k = kVector(x, z);

        if (k.x == 0 && k.y == 0) // Prevent div by 0 in phillipsSpectrum!
        {
          h0[index].x = 0.0f;
          h0[index].y = 0.0f;
        }
        else
        {
          float spectrumHalfSquared = sqrt(phillipsSpectrum(k)) * math::kSqrtOfHalf;

          h0[index].x = normalDist(randomGen) * spectrumHalfSquared;
          h0[index].y = normalDist(randomGen) * spectrumHalfSquared;
        }
      }

      mDevH0.copyFrom(h0.data(), h0.size());
    }
  }
}
