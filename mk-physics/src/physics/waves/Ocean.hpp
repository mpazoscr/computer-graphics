#ifndef SRC_PHYSICS_WAVES_OCEAN_H_
#define SRC_PHYSICS_WAVES_OCEAN_H_

#include <vector>

#include "core/CoreTypes.hpp"
#include "core/VertexTypes.hpp"
#include "cuda/DeviceMemory.hpp"
#include "math/IFFTSolver.hpp"
#include "mesh/RectPatch.hpp"

namespace mk
{
  namespace physics
  {
    /**
     * Statistical ocean simulator.
     *
     * The ocean is simulated following the statistical model described in Tessendorf's paper.
     */
    class Ocean
    {
    public:
      /**
       * Allocates and precomputes all the information necessary for the ocean simulation.
       *
       * @param rectPatch Reference to the mesh that will be updated according to the ocean simulation.
       * @param lengthX Physical size of the side parallel to the X-axis.
       * @param lengthZ Physical size of the side parallel to the Z-axis.
       * @note Keep in mind that the Ocean#update method will read and write from this rectPatch.
       * @warning Both sizeX and sizeZ are required to be power of 2.
       */
      Ocean(mesh::RectPatch<core::VertexPN>& rectPatch, float lengthX, float lengthZ);

      /**
       * Performs one step of the ocean simulation.
       *
       * @param t Global simulation time in seconds.
       * @note Keep in mind that the RectPatch passed as argument in Ocean#Ocean will be read/written in this method.
       */
      void update(float t);

      /**
       * @param windDir Direction of the wind that will determine the direction of the ocean waves.
       * This vector will be normalized, so its lenth will not affect the intensity of the wind. Use {@link setWindSpeed} for that.
       */
      void setWindDir(glm::vec2 windDir);

      /**
       * @param windSpeed Speed of the wind.
       */
      void setWindSpeed(float windSpeed);

      /**
       * @param gravity Acceleration of gravity in m/s^-2
       */
      void setGravity(float gravity);

      /**
       * @param amplitude Constant that determines the maximum amplitude of the ocean waves.
       */
      void setAmplitude(float amplitude);

      /**
       * @param crossWindDampingCoefficient Coefficient that controls the presence of waves perpendicular to the wind direction.
       */
      void setCrossWindDampingCoefficient(float crossWindDampingCoefficient);

      /**
       * @param smallWavesDampingCoefficient Coefficient that controls the presence of waves of small wave longitude.
       */
      void setSmallWavesDampingCoefficient(float smallWavesDampingCoefficient);

      /**
       * @param displacementFactor Controls the 'chopiness' of the ocean waves.
       */
      void setDisplacementFactor(float displacementFactor);

    private:
      mesh::RectPatch<core::VertexPN>& mRectPatch;
      cuda::DeviceMemory<core::complex> mDevH0;
      cuda::DeviceMemory<core::complex> mDevGpuSpectrum;
      cuda::DeviceMemory<core::complex> mDevDispX;
      cuda::DeviceMemory<core::complex> mDevDispZ;
      cuda::DeviceMemory<core::complex> mDevGradX;
      cuda::DeviceMemory<core::complex> mDevGradZ;
      cuda::DeviceMemory<core::VertexPN> mDevVbo;
      math::FFTSolverSharedPtr mFFTSolver;
      float mLengthX;
      float mLengthZ;
      glm::vec2 mWindDir;
      float mWindSpeed;
      float mGravity;
      float mAmplitude;
      float mCrossWindDampingCoefficient;
      float mSmallWavesDampingCoefficient;
      float mDisplacementFactor;

      glm::vec2 kVector(int x, int z);
      float phillipsSpectrum(const glm::vec2& k);
      void precomputeH0();
    };
  }
}

#endif  // SRC_PHYSICS_WAVES_OCEAN_H_
