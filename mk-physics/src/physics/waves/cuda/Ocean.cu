#include "Ocean_cu.h"

#include <math_constants.h>
#include <glm/glm.hpp>

#include "math/Utils.h"

using mk::core::complex;

__device__ complex makeComplex(float x, float y)
{
  complex c;

  c.x = x;
  c.y = y;

  return c;
}

__device__ complex add(complex a, complex b)
{
  return makeComplex(a.x + b.x, a.y + b.y);
}

__device__ complex mult(complex ab, complex cd)
{
    return makeComplex(ab.x * cd.x - ab.y * cd.y, ab.x * cd.y + ab.y * cd.x);
}

__device__ complex conj(complex arg)
{
    return makeComplex(arg.x, -arg.y);
}

__device__ complex eulerExp(float arg)
{
    return makeComplex(cosf(arg), sinf(arg));
}

__global__ void calculateSpectrumKernel(int sizeX, int sizeZ, float lengthX, float lengthZ, float gravity, float t, const complex* h0, complex* spectrum, complex* dispX, complex* dispZ, complex* gradX, complex* gradZ)
{
  int indexX = blockIdx.x * blockDim.x + threadIdx.x;
  int indexZ = blockIdx.y * blockDim.y + threadIdx.y;

  int indexXMirrored = sizeX - indexX - 1;
  int indexZMirrored = sizeZ - indexZ - 1;

  int index = indexZ * sizeX + indexX;
  int indexMirrored = indexZMirrored * sizeX + indexXMirrored;

  float kx = (indexX - sizeX / 2.0f) * (2.0f * mk::math::kPi / lengthX);
  float kz = (indexZ - sizeZ / 2.0f) * (2.0f * mk::math::kPi / lengthZ);
  float kLength = sqrt(kx * kx + kz * kz);
  float w = sqrt(gravity * kLength);

  spectrum[index] = add(mult(h0[index], eulerExp(w * t)),
                        mult(conj(h0[indexMirrored]), eulerExp(-w * t)));

  gradX[index] = makeComplex(-kx * spectrum[index].y, kx * spectrum[index].x);
  gradZ[index] = makeComplex(-kz * spectrum[index].y, kz * spectrum[index].x);

  if (kLength > 1e-12f)
  {
    dispX[index] = makeComplex(kx / kLength * spectrum[index].y, -kx / kLength * spectrum[index].x);
    dispZ[index] = makeComplex(kz / kLength * spectrum[index].y, -kz / kLength * spectrum[index].x);

    //jxx[index] = makeComplex(kx * kx / kLength * spectrum[index].x, kx * kx / kLength * spectrum[index].y);
    //jzz[index] = makeComplex(kz * kz / kLength * spectrum[index].x, kz * kz / kLength * spectrum[index].y);
    //jxz[index] = makeComplex(kx * kz / kLength * spectrum[index].x, kx * kz / kLength * spectrum[index].y);
  }
  else
  {
    dispX[index] = makeComplex(0.0f, 0.0f);
    dispZ[index] = makeComplex(0.0f, 0.0f);

    //jxx[index] = makeComplex(0.0f, 0.0f);
    //jzz[index] = makeComplex(0.0f, 0.0f);
    //jxz[index] = makeComplex(0.0f, 0.0f);
  }
}

__global__ void updateMeshKernel(int sizeX, int sizeZ, float dispFactor, const complex* h, complex* dispX, complex* dispZ, mk::core::VertexPN* mesh)
{
  int indexX = blockIdx.x * blockDim.x + threadIdx.x;
  int indexZ = blockIdx.y * blockDim.y + threadIdx.y;

  int index = indexZ * sizeX + indexX;

  float sign = ((indexX + indexZ) & 0x01) ? -1.0f : 1.0f;

  mesh[index].mPos.x = (indexX - sizeX / 2.0f) + dispFactor * sign * dispX[index].x;
  mesh[index].mPos.y = sign * h[index].x;
  mesh[index].mPos.z = (indexZ - sizeZ / 2.0f) + dispFactor * sign * dispZ[index].x;
}

__global__ void updateNormalsKernel(int sizeX, int sizeZ, complex* gradX, complex* gradZ, mk::core::VertexPN* mesh)
{
  int indexX = blockIdx.x * blockDim.x + threadIdx.x;
  int indexZ = blockIdx.y * blockDim.y + threadIdx.y;

  int index = indexZ * sizeX + indexX;

  float sign = ((indexX + indexZ) & 0x01) ? 1.0f : -1.0f; // Reverse sign here, since we need a - anyway

  mesh[index].mNormal = glm::normalize(glm::vec3(sign * gradX[index].x, 1.0f, sign * gradZ[index].x));
}

namespace mk
{
  namespace physics
  {
    namespace cuda
    {
      namespace
      {
        const int kBlocksPerSide = 16;
      }

      void calculateSpectrum(int sizeX, int sizeZ, float lengthX, float lengthZ, float gravity, float t, const complex* h0, complex* spectrum, complex* dispX, complex* dispZ, complex* gradX, complex* gradZ)
      {
        assert((sizeX % kBlocksPerSide == 0));
        assert((sizeZ % kBlocksPerSide == 0));

        dim3 grid(kBlocksPerSide, kBlocksPerSide, 1);
        dim3 block(sizeX / kBlocksPerSide, sizeZ / kBlocksPerSide, 1);

        calculateSpectrumKernel<<<grid, block>>>(sizeX, sizeZ, lengthX, lengthZ, gravity, t, h0, spectrum, dispX, dispZ, gradX, gradZ);
      }

      void updateMesh(int sizeX, int sizeZ, float dispFactor, const complex* h, complex* dispX, complex* dispZ, core::VertexPN* mesh)
      {
        assert((sizeX % kBlocksPerSide == 0));
        assert((sizeZ % kBlocksPerSide == 0));

        dim3 grid(kBlocksPerSide, kBlocksPerSide, 1);
        dim3 block(sizeX / kBlocksPerSide, sizeX / kBlocksPerSide, 1);

        updateMeshKernel<<<grid, block>>>(sizeX, sizeZ, dispFactor, h, dispX, dispZ, mesh);
      }

      void updateNormals(int sizeX, int sizeZ, complex* gradX, complex* gradZ, core::VertexPN* mesh)
      {
        assert((sizeX % kBlocksPerSide == 0));
        assert((sizeZ % kBlocksPerSide == 0));

        dim3 grid(kBlocksPerSide, kBlocksPerSide, 1);
        dim3 block(sizeX / kBlocksPerSide, sizeZ / kBlocksPerSide, 1);

        updateNormalsKernel<<<grid, block>>>(sizeX, sizeZ, gradX, gradZ, mesh);
      }
    }
  }
}
