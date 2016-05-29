#ifndef SRC_MATH_IFFTSOLVER_H_
#define SRC_MATH_IFFTSOLVER_H_

#include <memory>

#include "core/CoreTypes.h"

namespace mk
{
  namespace math
  {
    /**
     * Interface for FFT operations.
     * @interface IFFTSolver
     */
    class IFFTSolver
    {
    public:
      /**
       * Virtual destructor
       */
      virtual ~IFFTSolver() {}

      /**
       * @brief Performs forward FFT in 2D.
       * @param data Pointer to the FFT input data. Raw pointer is used in order to ease integration with GPU APIs.
       * The total size of this data should be sizeX * sizeY.
       * @param sizeX Size (1st dimension) of the input data.
       * @param sizeY Size (2nd dimension) of the input data.
       * @return True if the FFT was performed successfully, false otherwise.
       */
      virtual bool fft2D(core::complex* data, int sizeX, int sizeY) = 0;

      /**
       * @brief Performs inverse FFT in 2D.
       * @param data Pointer to the FFT input data. Raw pointer is used in order to ease integration with GPU APIs.
       * The total size of this data should be sizeX * sizeY.
       * @param sizeX Size (1st dimension) of the input data.
       * @param sizeY Size (2nd dimension) of the input data.
       * @return True if the inverse FFT was performed successfully, false otherwise.
       */
      virtual bool fftInv2D(core::complex* data, int sizeX, int sizeY) = 0;
    };

    typedef std::shared_ptr<IFFTSolver> FFTSolverSharedPtr;
  }
}

#endif  // SRC_MATH_IFFTSOLVER_H_
