#ifndef SRC_PHYSICS_CUDA_OCEAN_CU_H_
#define SRC_PHYSICS_CUDA_OCEAN_CU_H_

#include "core/CoreTypes.hpp"
#include "core/VertexTypes.hpp"

namespace mk
{
  namespace physics
  {
    namespace cuda
    {
      void calculateSpectrum(int sizeX, int sizeZ, float lengthX, float lengthZ, float gravity, float t, const core::complex* h0, core::complex* spectrum, core::complex* dispX, core::complex* dispZ, core::complex* gradX, core::complex* gradZ);
      void updateMesh(int sizeX, int sizeZ, float dispFactor, const core::complex* h, core::complex* dispX, core::complex* dispZ, core::VertexPN* mesh);
      void updateNormals(int sizeX, int sizeZ, core::complex* gradX, core::complex* gradZ, core::VertexPN* mesh);
    }
  }
}

#endif  // SRC_PHYSICS_CUDA_OCEAN_CU_H_
