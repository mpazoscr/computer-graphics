#ifndef SRC_PHYSICS_WAVES_OCEAN_H_
#define SRC_PHYSICS_WAVES_OCEAN_H_

#include "gpgpu/gl/DeviceMemory.hpp"
#include "gpgpu/gl/FFTSolver.hpp"
#include "renderer/VertexTypes.hpp"
#include "renderer/mesh/RectPatch.hpp"
#include "renderer/gl/ShaderProgram.hpp"

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
       *                  This mesh will be resized to accomodate the necessary number of vertices for the ocean.
       * @param size Size of the ocean patch (in vertices).
       * @param tiles Number of times the ocean patch will be repeated in the X and Z axis respectively. Neither can be zero.
       * @param length Physical size of the ocean patch.
       * @note Keep in mind that the Ocean#update method will read and write from this rectPatch.
       * @warning Both components of size are required to be power of 2.
       */
      Ocean(renderer::mesh::RectPatch<renderer::VertexPN>& rectPatch, glm::uvec2 size, glm::uvec2 tiles, glm::vec2 length);

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
      glm::vec2 kVector(int x, int z);
      float phillipsSpectrum(const glm::vec2& k);
      void precomputeH0();

    private:
      renderer::mesh::RectPatch<renderer::VertexPN>& mRectPatch;
      gpgpu::gl::DeviceMemory<std::complex<float>> mDevH0;
      gpgpu::gl::DeviceMemory<std::complex<float>> mDevGpuSpectrumIn;
      gpgpu::gl::DeviceMemory<std::complex<float>> mDevDispXIn;
      gpgpu::gl::DeviceMemory<std::complex<float>> mDevDispZIn;
      gpgpu::gl::DeviceMemory<std::complex<float>> mDevGradXIn;
      gpgpu::gl::DeviceMemory<std::complex<float>> mDevGradZIn;
      gpgpu::gl::DeviceMemory<std::complex<float>> mDevGpuSpectrumOut;
      gpgpu::gl::DeviceMemory<std::complex<float>> mDevDispXOut;
      gpgpu::gl::DeviceMemory<std::complex<float>> mDevDispZOut;
      gpgpu::gl::DeviceMemory<std::complex<float>> mDevGradXOut;
      gpgpu::gl::DeviceMemory<std::complex<float>> mDevGradZOut;
      gpgpu::gl::FFTSolver mFFTSolver;
      renderer::gl::ShaderProgram mCalculateSpectrumProgram;
      renderer::gl::ShaderProgram mUpdateMeshProgram;
      renderer::gl::ShaderProgram mUpdateNormalsProgram;
      glm::uvec2 mSize;
      glm::vec2 mLength;
      glm::vec2 mWindDir;
      float mWindSpeed;
      float mGravity;
      float mAmplitude;
      float mCrossWindDampingCoefficient;
      float mSmallWavesDampingCoefficient;
      float mDisplacementFactor;
    };
  }
}

#endif  // SRC_PHYSICS_WAVES_OCEAN_H_
