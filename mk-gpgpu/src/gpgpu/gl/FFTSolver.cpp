#include "FFTSolver.hpp"

#include "glfft/glfft.hpp"
#include "glfft/glfft_gl_interface.hpp"

namespace mk
{
  namespace gpgpu
  {
    namespace gl
    {
      void FFTSolver::fft2D(DeviceMemory<core::complex>& data, int sizeX, int sizeY)
      {
        GLFFT::FFTOptions options;
        GLFFT::GLContext context;
        GLFFT::FFT fft(&context, sizeX, sizeY, GLFFT::ComplexToComplex, GLFFT::Forward, GLFFT::SSBO, GLFFT::SSBO, std::make_shared<GLFFT::ProgramCache>(), options);

        GLFFT::GLBuffer input(data.getId());
        GLFFT::GLBuffer output(data.getId());

        GLFFT::CommandBuffer* command = context.request_command_buffer();
        fft.process(command, &input, &output);
        context.submit_command_buffer(command);

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
      }

      void FFTSolver::fftInv2D(DeviceMemory<core::complex>& data, int sizeX, int sizeY)
      {
        GLFFT::FFTOptions options;
        GLFFT::GLContext context;
        GLFFT::FFT fft(&context, sizeX, sizeY, GLFFT::ComplexToComplex, GLFFT::Inverse, GLFFT::SSBO, GLFFT::SSBO, std::make_shared<GLFFT::ProgramCache>(), options);

        GLFFT::GLBuffer input(data.getId());
        GLFFT::GLBuffer output(data.getId());

        GLFFT::CommandBuffer* command = context.request_command_buffer();
        fft.process(command, &input, &output);
        context.submit_command_buffer(command);

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
      }
    }
  }
}
