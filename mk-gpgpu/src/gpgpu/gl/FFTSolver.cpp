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
      namespace
      {
        const bool kDoFFTWisdom = false;

        struct FFTPlan
        {
          FFTPlan(const std::shared_ptr<GLFFT::FFTWisdom>& fftWisdom, const std::shared_ptr<GLFFT::FFT>& fft)
          : mFFTWisdom(fftWisdom),
            mFFT(fft)
          {
          }

          std::shared_ptr<GLFFT::FFTWisdom> mFFTWisdom;
          std::shared_ptr<GLFFT::FFT> mFFT;
        };
      }

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
          auto& fftPlan = mForwardCache[getKey(sizeX, sizeY)];
          return getOrCreateFFT(fftPlan, sizeX, sizeY, GLFFT::Forward);
        }

        GLFFT::FFT& getInvFFT(int sizeX, int sizeY)
        {
          auto& fftPlan = mInverseCache[getKey(sizeX, sizeY)];
          return getOrCreateFFT(fftPlan, sizeX, sizeY, GLFFT::Inverse);
        }

      private:
        std::int64_t getKey(int sizeX, int sizeY)
        {
          return (static_cast<int64_t>(sizeX) << 32) | sizeY;
        }

        GLFFT::FFT& getOrCreateFFT(std::shared_ptr<FFTPlan>& fftPlan, int sizeX, int sizeY, GLFFT::Direction direction)
        {
          if (!fftPlan)
          {
            GLFFT::FFTOptions options;

            auto fftWisdom = std::make_shared<GLFFT::FFTWisdom>();
            if (kDoFFTWisdom)
            {
              fftWisdom->set_static_wisdom(GLFFT::FFTWisdom::get_static_wisdom_from_renderer(&mGLContext));
              fftWisdom->learn_optimal_options_exhaustive(&mGLContext, sizeX, sizeY, GLFFT::ComplexToComplex, GLFFT::SSBO, GLFFT::SSBO, options.type);
            }

            auto fft = std::make_shared<GLFFT::FFT>(&mGLContext, sizeX, sizeY, GLFFT::ComplexToComplex, direction, GLFFT::SSBO, GLFFT::SSBO, std::make_shared<GLFFT::ProgramCache>(), options);

            fftPlan = std::make_shared<FFTPlan>(fftWisdom, fft);
          }

          return *fftPlan->mFFT;
        }

      private:
        GLFFT::GLContext& mGLContext;
        std::unordered_map<std::int64_t, std::shared_ptr<FFTPlan>> mForwardCache;
        std::unordered_map<std::int64_t, std::shared_ptr<FFTPlan>> mInverseCache;
      };

      FFTSolver::FFTSolver()
      : mGLContext(std::make_unique<GLFFT::GLContext>()),
        mFFTSolverCache(std::make_unique<FFTSolverCache>(*mGLContext))
      {
      }

      FFTSolver::~FFTSolver()
      {
      }

      void FFTSolver::fft2D(DeviceMemory<std::complex<float>>& input, DeviceMemory<std::complex<float>>& output, int sizeX, int sizeY)
      {
        GLFFT::FFT& fft = mFFTSolverCache->getFFT(sizeX, sizeY);

        GLFFT::GLBuffer inputBuffer(input.getId());
        GLFFT::GLBuffer outputBuffer(output.getId());

        GLFFT::CommandBuffer* command = mGLContext->request_command_buffer();
        fft.process(command, &outputBuffer, &inputBuffer);
        mGLContext->submit_command_buffer(command);

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
      }

      void FFTSolver::fftInv2D(DeviceMemory<std::complex<float>>& input, DeviceMemory<std::complex<float>>& output, int sizeX, int sizeY)
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
