#ifndef SRC_GPGPU_GL_FFTSOLVER_H_
#define SRC_GPGPU_GL_FFTSOLVER_H_

#include "core/CoreTypes.hpp"

#include "DeviceMemory.hpp"

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
        * @brief Performs forward FFT in 2D.
        * @param data FFT input data allocated in the GPU.
        * The total size of this data should be sizeX * sizeY.
        * @param sizeX Size (1st dimension) of the input data.
        * @param sizeY Size (2nd dimension) of the input data.
        * @return True if the FFT was performed successfully, false otherwise.
        */
        void fft2D(DeviceMemory<core::complex>& data, int sizeX, int sizeY);

        /**
        * @brief Performs inverse FFT in 2D.
        * @param data FFT input data allocated in the GPU.
        * The total size of this data should be sizeX * sizeY.
        * @param sizeX Size (1st dimension) of the input data.
        * @param sizeY Size (2nd dimension) of the input data.
        * @return True if the inverse FFT was performed successfully, false otherwise.
        */
        void fftInv2D(DeviceMemory<core::complex>& data, int sizeX, int sizeY);
      };
    }
  }
}

#endif  // SRC_GPGPU_GL_FFTSOLVER_H_
