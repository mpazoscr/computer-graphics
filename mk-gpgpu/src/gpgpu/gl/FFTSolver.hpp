#ifndef SRC_GPGPU_GL_FFTSOLVER_H_
#define SRC_GPGPU_GL_FFTSOLVER_H_

#include <memory>
#include <complex>

#include "DeviceMemory.hpp"

namespace GLFFT
{
  class GLContext;
}

namespace mk
{
  namespace gpgpu
  {
    namespace gl
    {
      /**
       * FFT solver that uses GLFFT to perform inplace FFT in the GPU.
       */
      class FFTSolver
      {
      public:
        /**
         * Default constructor
         */
        FFTSolver();

        /**
         * Default destructor
         */
        ~FFTSolver();

        /**
        * @brief Performs forward FFT in 2D.
        * @param input FFT input buffer allocated in the GPU.
        * @param output FFT output buffer allocated in the GPU.
        * @param sizeX Size (1st dimension) of the input data.
        * @param sizeY Size (2nd dimension) of the input data.
        * @return True if the FFT was performed successfully, false otherwise.
        * @note The size of the GPU allocated buffers should be sizeX * sizeY.
        */
        void fft2D(DeviceMemory<std::complex<float>>& input, DeviceMemory<std::complex<float>>& output, int sizeX, int sizeY);

        /**
        * @brief Performs inverse FFT in 2D.
        * @param input FFT input buffer allocated in the GPU.
        * @param output FFT output buffer allocated in the GPU.
        * @param sizeX Size (1st dimension) of the input data.
        * @param sizeY Size (2nd dimension) of the input data.
        * @return True if the inverse FFT was performed successfully, false otherwise.
        * @note The size of the GPU allocated buffers should be sizeX * sizeY.
        */
        void fftInv2D(DeviceMemory<std::complex<float>>& input, DeviceMemory<std::complex<float>>& output, int sizeX, int sizeY);

      private:
        class FFTSolverCache;

      private:
        std::unique_ptr<GLFFT::GLContext> mGLContext;
        std::unique_ptr<FFTSolverCache> mFFTSolverCache;
      };
    }
  }
}

#endif  // SRC_GPGPU_GL_FFTSOLVER_H_
