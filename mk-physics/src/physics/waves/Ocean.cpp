#include "Ocean.hpp"

#include <cmath>
#include <cassert>
#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "math/Utils.hpp"
#include "renderer/assets/ResourceLoader.hpp"

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

    Ocean::Ocean(renderer::mesh::RectPatch<renderer::VertexPN>& rectPatch, glm::uvec2 size, glm::uvec2 tiles, glm::vec2 length)
    : mRectPatch(rectPatch),
      mDevH0(size.x * size.y * sizeof(std::complex<float>), GL_STATIC_DRAW),
      mDevGpuSpectrumIn(size.x * size.y * sizeof(std::complex<float>)),
      mDevDispXIn(size.x * size.y * sizeof(std::complex<float>)),
      mDevDispZIn(size.x * size.y * sizeof(std::complex<float>)),
      mDevGradXIn(size.x * size.y * sizeof(std::complex<float>)),
      mDevGradZIn(size.x * size.y * sizeof(std::complex<float>)),
      mDevGpuSpectrumOut(size.x * size.y * sizeof(std::complex<float>)),
      mDevDispXOut(size.x * size.y * sizeof(std::complex<float>)),
      mDevDispZOut(size.x * size.y * sizeof(std::complex<float>)),
      mDevGradXOut(size.x * size.y * sizeof(std::complex<float>)),
      mDevGradZOut(size.x * size.y * sizeof(std::complex<float>)),
      mFFTSolver(),
      mCalculateSpectrumProgram(),
      mUpdateMeshProgram(),
      mUpdateNormalsProgram(),
      mSize(size),
      mLength(length),
      mWindDir(kWindDir),
      mWindSpeed(kWindSpeed),
      mGravity(kGravity),
      mAmplitude(kAmplitude),
      mCrossWindDampingCoefficient(kCrossWindDampingCoefficient),
      mSmallWavesDampingCoefficient(kSmallWavesDampingCoefficient),
      mDisplacementFactor(kDisplacementFactor)
    {
      assert(math::isPowerOf2(mSize.x) && "Ocean grid size X is not a power of 2");
      assert(math::isPowerOf2(mSize.y) && "Ocean grid size Z is not a power of 2");
      assert((tiles.x > 0) && "Number of ocean tiles must be greater than 0");
      assert((tiles.y > 0) && "Number of ocean tiles must be greater than 0");

      mRectPatch.resize(mSize.x * tiles.x, mSize.y * tiles.y);

      mCalculateSpectrumProgram.attachComputeShader(renderer::assets::ResourceLoader::loadShaderSource("ocean_calculate_spectrum.comp"));
      mCalculateSpectrumProgram.link();

      mUpdateMeshProgram.attachComputeShader(renderer::assets::ResourceLoader::loadShaderSource("ocean_update_mesh.comp"));
      mUpdateMeshProgram.link();

      mUpdateNormalsProgram.attachComputeShader(renderer::assets::ResourceLoader::loadShaderSource("ocean_update_normals.comp"));
      mUpdateNormalsProgram.link();

      precomputeH0();
    }

    void Ocean::update(float t)
    {
      const glm::uvec2 meshSize(mRectPatch.getWidth(), mRectPatch.getHeight());
      const glm::vec2 oceanLength(mLength.x, mLength.y);

      const unsigned int blockSizeX = mSize.x / kBlocksPerSide;
      const unsigned int blockSizeY = mSize.y / kBlocksPerSide;

      // Generate spectrum in GPU

      mDevH0.bind(0);
      mDevGpuSpectrumIn.bind(1);
      mDevDispXIn.bind(2);
      mDevDispZIn.bind(3);
      mDevGradXIn.bind(4);
      mDevGradZIn.bind(5);

      mCalculateSpectrumProgram.use();
      mCalculateSpectrumProgram.setUniformVector2uv("oceanSize", glm::value_ptr(mSize));
      mCalculateSpectrumProgram.setUniformVector2fv("oceanLength", glm::value_ptr(oceanLength));
      mCalculateSpectrumProgram.setUniform1f("g", kGravity);
      mCalculateSpectrumProgram.setUniform1f("t", t);
      mCalculateSpectrumProgram.dispatchCompute(blockSizeX, blockSizeY, 1);

      // Perform FFT

      mFFTSolver.fftInv2D(mDevGpuSpectrumIn, mDevGpuSpectrumOut, mSize.x, mSize.y);
      mFFTSolver.fftInv2D(mDevDispXIn, mDevDispXOut, mSize.x, mSize.y);
      mFFTSolver.fftInv2D(mDevDispZIn, mDevDispZOut, mSize.x, mSize.y);
      mFFTSolver.fftInv2D(mDevGradXIn, mDevGradXOut, mSize.x, mSize.y);
      mFFTSolver.fftInv2D(mDevGradZIn, mDevGradZOut, mSize.x, mSize.y);

      // Update mesh position

      mDevGpuSpectrumOut.bind(0);
      mDevDispXOut.bind(1);
      mDevDispZOut.bind(2);
      mRectPatch.getVao().bind(3);

      mUpdateMeshProgram.use();
      mUpdateMeshProgram.setUniformVector2uv("oceanSize", glm::value_ptr(mSize));
      mUpdateMeshProgram.setUniformVector2uv("meshSize", glm::value_ptr(meshSize));
      mUpdateMeshProgram.setUniform1f("dispFactor", mDisplacementFactor);
      mUpdateMeshProgram.dispatchCompute(blockSizeX, blockSizeY, 1);

      // Update normals

      mDevGpuSpectrumOut.bind(0);
      mDevGradXOut.bind(1);
      mDevGradZOut.bind(2);
      mRectPatch.getVao().bind(3);

      mUpdateNormalsProgram.use();
      mUpdateNormalsProgram.setUniformVector2uv("oceanSize", glm::value_ptr(mSize));
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
      return glm::vec2((x - mSize.x / 2.0f) * (2.0f * math::kPi / mLength.x),
                       (z - mSize.y / 2.0f) * (2.0f * math::kPi / mLength.y));
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
      std::vector<std::complex<float>> h0(mSize.x * mSize.y);

      std::random_device randomDev;
      std::mt19937 randomGen(randomDev());
      std::normal_distribution<float> normalDist(0.0f, 1.0f);

      for (unsigned int z = 0; z < mSize.y; ++z)
      for (unsigned int x = 0; x < mSize.x; ++x)
      {
        int index = z * mSize.x + x;

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
