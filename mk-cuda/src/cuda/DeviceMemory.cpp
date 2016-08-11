#include "DeviceMemory.hpp"

#include <cuda_runtime.h>
#include <cufft.h>

#include "core/CoreTypes.hpp"
#include "core/VertexTypes.hpp"

namespace mk
{
  namespace cuda
  {
    template <typename T> DeviceMemory<T>::DeviceMemory()
      : mDevMemPtr(nullptr),
        mOwnsMemory(false)
    {
    }

    template <typename T> DeviceMemory<T>::DeviceMemory(size_t count)
      : mDevMemPtr(nullptr),
        mOwnsMemory(true)
    {
      cudaMalloc(reinterpret_cast<void**>(&mDevMemPtr), count * sizeof(T));
    }

    template <typename T> DeviceMemory<T>::~DeviceMemory()
    {
      if (mOwnsMemory && (nullptr != mDevMemPtr))
      {
        cudaFree(mDevMemPtr);
      }
    }

    template <typename T> void DeviceMemory<T>::allocate(size_t count)
    {
      if (nullptr != mDevMemPtr)
      {
        cudaFree(mDevMemPtr);
      }

      mDevMemPtr = nullptr;
      mOwnsMemory = true;

      cudaMalloc(reinterpret_cast<void**>(&mDevMemPtr), count * sizeof(T));
    }

    template <typename T> bool DeviceMemory<T>::bindGlBuffer(GLuint glId)
    {
      assert(nullptr == mCudaResourceMap[glId]);

      if (nullptr != mDevMemPtr)
      {
        cudaFree(mDevMemPtr);
      }

      mDevMemPtr = nullptr;
      mOwnsMemory = false;

      cudaGraphicsResource_t bufferCudaResource;

      if (cudaSuccess == cudaGraphicsGLRegisterBuffer(&bufferCudaResource, glId, cudaGraphicsMapFlagsNone) &&
          cudaSuccess == cudaGraphicsMapResources(1, &bufferCudaResource))
      {
        size_t size;
        cudaError_t errorCode = cudaGraphicsResourceGetMappedPointer(reinterpret_cast<void**>(&mDevMemPtr), &size, bufferCudaResource);

        if (cudaSuccess == errorCode)
        {
          mCudaResourceMap[glId] = bufferCudaResource;
          return true;
        }

        return false;
      }

      return false;
    }

    template <typename T> bool DeviceMemory<T>::unbindGlBuffer(GLuint glId)
    {
      cudaGraphicsResource_t bufferCudaResource = mCudaResourceMap[glId];

      assert(nullptr != bufferCudaResource);

      bool success = (cudaSuccess == cudaGraphicsUnmapResources(1, &bufferCudaResource) &&
                      cudaSuccess == cudaGraphicsUnregisterResource(bufferCudaResource));

      if (success)
      {
        mCudaResourceMap[glId] = nullptr;
        return true;
      }

      return false;
    }

    template <typename T> bool DeviceMemory<T>::copyFrom(const T* ptrToHostMem, size_t count)
    {
      return cudaSuccess == cudaMemcpy(mDevMemPtr, ptrToHostMem, count * sizeof(T), cudaMemcpyHostToDevice);
    }

    template <typename T> bool DeviceMemory<T>::copyTo(T* ptrToHostMem, size_t count) const
    {
      return cudaSuccess == cudaMemcpy(ptrToHostMem, mDevMemPtr, count * sizeof(T), cudaMemcpyDeviceToHost);
    }

    template <typename T> T* DeviceMemory<T>::ptr() const
    {
      return mDevMemPtr;
    }

    template <typename T> bool DeviceMemory<T>::isValid() const
    {
      return nullptr != mDevMemPtr;
    }

    template class DeviceMemory<float>;
    template class DeviceMemory<core::complex>;
    template class DeviceMemory<core::VertexPN>;
    template class DeviceMemory<core::VertexPC>;
    template class DeviceMemory<core::VertexPNT>;
  }
}
