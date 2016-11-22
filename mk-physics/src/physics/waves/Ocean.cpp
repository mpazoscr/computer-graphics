#include "Ocean.hpp"

#include <cmath>
#include <cassert>
#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "math/Utils.hpp"
#include "assets/ResourceLoader.hpp"

namespace mk
{
  namespace physics
  {
    namespace
    {
      const int kBlocksPerSide = 16;

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
      mDevH0(mRectPatch.n() *  mRectPatch.m() * sizeof(std::complex<float>), GL_STATIC_DRAW),
      mDevGpuSpectrumIn(mRectPatch.n() * mRectPatch.m() * sizeof(std::complex<float>)),
      mDevDispXIn(mRectPatch.n() *  mRectPatch.m() * sizeof(std::complex<float>)),
      mDevDispZIn(mRectPatch.n() *  mRectPatch.m() * sizeof(std::complex<float>)),
      mDevGradXIn(mRectPatch.n() *  mRectPatch.m() * sizeof(std::complex<float>)),
      mDevGradZIn(mRectPatch.n() *  mRectPatch.m() * sizeof(std::complex<float>)),
      mDevGpuSpectrumOut(mRectPatch.n() * mRectPatch.m() * sizeof(std::complex<float>)),
      mDevDispXOut(mRectPatch.n() *  mRectPatch.m() * sizeof(std::complex<float>)),
      mDevDispZOut(mRectPatch.n() *  mRectPatch.m() * sizeof(std::complex<float>)),
      mDevGradXOut(mRectPatch.n() *  mRectPatch.m() * sizeof(std::complex<float>)),
      mDevGradZOut(mRectPatch.n() *  mRectPatch.m() * sizeof(std::complex<float>)),
      mFFTSolver(),
      mCalculateSpectrumProgram(),
      mUpdateMeshProgram(),
      mUpdateNormalsProgram(),
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
      assert(math::isPowerOf2(mRectPatch.n()) && "Ocean grid size X is not a power of 2");
      assert(math::isPowerOf2(mRectPatch.m()) && "Ocean grid size Z is not a power of 2");

      mCalculateSpectrumProgram.attachComputeShader(assets::ResourceLoader::loadShaderSource("ocean_calculate_spectrum.comp"));
      mCalculateSpectrumProgram.link();

      mUpdateMeshProgram.attachComputeShader(assets::ResourceLoader::loadShaderSource("ocean_update_mesh.comp"));
      mUpdateMeshProgram.link();

      mUpdateNormalsProgram.attachComputeShader(assets::ResourceLoader::loadShaderSource("ocean_update_normals.comp"));
      mUpdateNormalsProgram.link();

      precomputeH0();
    }

    void Ocean::update(float t)
    {
      const glm::uvec2 meshSize(mRectPatch.n(), mRectPatch.m());
      const glm::vec2 physicalSize(mLengthX, mLengthZ);

      const unsigned int blockSizeX = mRectPatch.n() / kBlocksPerSide;
      const unsigned int blockSizeY = mRectPatch.m() / kBlocksPerSide;

      // Generate spectrum in GPU

      mDevH0.bind(0);
      mDevGpuSpectrumIn.bind(1);
      mDevDispXIn.bind(2);
      mDevDispZIn.bind(3);
      mDevGradXIn.bind(4);
      mDevGradZIn.bind(5);

      mCalculateSpectrumProgram.use();
      mCalculateSpectrumProgram.setUniformVector2uv("meshSize", glm::value_ptr(meshSize));
      mCalculateSpectrumProgram.setUniformVector2fv("physicalSize", glm::value_ptr(physicalSize));
      mCalculateSpectrumProgram.setUniform1f("g", kGravity);
      mCalculateSpectrumProgram.setUniform1f("t", t);
      mCalculateSpectrumProgram.dispatchCompute(blockSizeX, blockSizeY, 1);

      // Perform FFT

      mFFTSolver.fftInv2D(mDevGpuSpectrumIn, mDevGpuSpectrumOut, mRectPatch.n(), mRectPatch.m());
      mFFTSolver.fftInv2D(mDevDispXIn, mDevDispXOut, mRectPatch.n(), mRectPatch.m());
      mFFTSolver.fftInv2D(mDevDispZIn, mDevDispZOut, mRectPatch.n(), mRectPatch.m());
      mFFTSolver.fftInv2D(mDevGradXIn, mDevGradXOut, mRectPatch.n(), mRectPatch.m());
      mFFTSolver.fftInv2D(mDevGradZIn, mDevGradZOut, mRectPatch.n(), mRectPatch.m());

      // Update mesh position

      mDevGpuSpectrumOut.bind(0);
      mDevDispXOut.bind(1);
      mDevDispZOut.bind(2);
      mRectPatch.getVao().bind(3);

      mUpdateMeshProgram.use();
      mUpdateMeshProgram.setUniformVector2uv("meshSize", glm::value_ptr(meshSize));
      mUpdateMeshProgram.setUniform1f("dispFactor", mDisplacementFactor);
      mUpdateMeshProgram.dispatchCompute(blockSizeX, blockSizeY, 1);

      // Update normals

      mDevGpuSpectrumOut.bind(0);
      mDevGradXOut.bind(1);
      mDevGradZOut.bind(2);
      mRectPatch.getVao().bind(3);

      mUpdateNormalsProgram.use();
      mUpdateNormalsProgram.setUniformVector2uv("meshSize", glm::value_ptr(meshSize));
      mUpdateNormalsProgram.dispatchCompute(blockSizeX, blockSizeY, 1);
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
      std::vector<std::complex<float>> h0(mRectPatch.n() * mRectPatch.m());

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
          h0[index].real(0.0f);
          h0[index].imag(0.0f);
        }
        else
        {
          float spectrumHalfSquared = sqrt(phillipsSpectrum(k)) * math::kSqrtOfHalf;

          h0[index].real(normalDist(randomGen) * spectrumHalfSquared);
          h0[index].imag(normalDist(randomGen) * spectrumHalfSquared);
        }
      }

      mDevH0.copyFrom(h0.data(), h0.size());
    }
  }
}
