#include "DeviceCaps.h"

#include <unordered_map>
#include <cstring>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

namespace mk
{
  namespace cuda
  {
    namespace
    {
      std::unordered_map<int, DeviceCaps> capsCache;
    }

    int getDeviceCount()
    {
      int count = 0;
      cudaGetDeviceCount(&count);

      return count;
    }

    bool getDeviceCaps(int device, DeviceCaps& caps)
    {
      if (capsCache.end() != capsCache.find(device))
      {
        caps = capsCache[device];
      }
      else
      {
        if (device < getDeviceCount())
        {
          cudaDeviceProp cudaCaps;

          if (cudaSuccess == cudaGetDeviceProperties(&cudaCaps, device))
          {
            caps.name.assign(cudaCaps.name, cudaCaps.name + strlen(cudaCaps.name));
            caps.mNumMultiprocessors = cudaCaps.multiProcessorCount;
            caps.mMaxThreadsPerBlock = cudaCaps.maxThreadsPerBlock;
            caps.mMaxGridSize[0] = cudaCaps.maxGridSize[0];
            caps.mMaxGridSize[1] = cudaCaps.maxGridSize[1];
            caps.mMaxGridSize[2] = cudaCaps.maxGridSize[2];
            caps.mWarpSize = cudaCaps.warpSize;

            capsCache[device] = caps;
          }
        }
      }

      return false;
    }

    bool setDevice(int device)
    {
      if (device < getDeviceCount())
      {
        return (cudaSuccess == cudaSetDevice(device) && cudaSuccess == cudaGLSetGLDevice(device));
      }
    }
  }
}
