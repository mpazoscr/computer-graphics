#include "FFTSolver.hpp"

#include <cstdint>
#include <unordered_map>

#include "glfft/glfft.hpp"
#include "glfft/glfft_gl_interface.hpp"

namespace mk
{
  namespace gpgpu
  {
    namespace gl
    {
      class FFTSolver::FFTSolverCache
      {
      public:
        FFTSolverCache(GLFFT::GLContext& glContext)
        : mGLContext(glContext),
          mForwardCache(),
          mInverseCache()
        {
        }

        GLFFT::FFT& getFFT(int sizeX, int sizeY)
        {
          auto& fft = mForwardCache[getKey(sizeX, sizeY)];
          return getOrCreateFFT(fft, sizeX, sizeY, GLFFT::Forward);
        }

        GLFFT::FFT& getInvFFT(int sizeX, int sizeY)
        {
          auto& fft = mInverseCache[getKey(sizeX, sizeY)];
          return getOrCreateFFT(fft, sizeX, sizeY, GLFFT::Inverse);
        }

      private:
        std::int64_t getKey(int sizeX, int sizeY)
        {
          return (static_cast<int64_t>(sizeX) << 32) | sizeY;
        }

        GLFFT::FFT& getOrCreateFFT(std::shared_ptr<GLFFT::FFT>& fft, int sizeX, int sizeY, GLFFT::Direction direction)
        {
          if (!fft)
          {
            GLFFT::FFTOptions options;
            fft.reset(new GLFFT::FFT(&mGLContext, sizeX, sizeY, GLFFT::ComplexToComplex, direction, GLFFT::SSBO, GLFFT::SSBO, std::make_shared<GLFFT::ProgramCache>(), options));
          }

          return *fft;
        }

      private:
        GLFFT::GLContext& mGLContext;
        std::unordered_map<std::int64_t, std::shared_ptr<GLFFT::FFT>> mForwardCache;
        std::unordered_map<std::int64_t, std::shared_ptr<GLFFT::FFT>> mInverseCache;
      };

      FFTSolver::FFTSolver()
      : mGLContext(std::make_unique<GLFFT::GLContext>()),
        mFFTSolverCache(std::make_unique<FFTSolverCache>(*mGLContext))
      {
      }

      FFTSolver::~FFTSolver()
      {
      }

      void FFTSolver::fft2D(DeviceMemory<core::complex>& input, DeviceMemory<core::complex>& output, int sizeX, int sizeY)
      {
        GLFFT::FFT& fft = mFFTSolverCache->getFFT(sizeX, sizeY);

        GLFFT::GLBuffer inputBuffer(input.getId());
        GLFFT::GLBuffer outputBuffer(output.getId());

        GLFFT::CommandBuffer* command = mGLContext->request_command_buffer();
        fft.process(command, &outputBuffer, &inputBuffer);
        mGLContext->submit_command_buffer(command);

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
      }

      void FFTSolver::fftInv2D(DeviceMemory<core::complex>& input, DeviceMemory<core::complex>& output, int sizeX, int sizeY)
      {
        GLFFT::FFT& fft = mFFTSolverCache->getInvFFT(sizeX, sizeY);

        GLFFT::GLBuffer inputBuffer(input.getId());
        GLFFT::GLBuffer outputBuffer(output.getId());

        GLFFT::CommandBuffer* command = mGLContext->request_command_buffer();
        fft.process(command, &outputBuffer, &inputBuffer);
        mGLContext->submit_command_buffer(command);

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
      }
    }
  }
}
