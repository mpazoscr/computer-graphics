#ifndef SRC_GPGPU_GL_DEVICEMEMORY_H_
#define SRC_GPGPU_GL_DEVICEMEMORY_H_

#include <GL/glew.h>

#include <memory>
#include <cstddef>
#include <unordered_map>

namespace mk
{
  namespace cuda
  {
    /**
     * Wrapper around OpenGL shader storage buffers for GPU-side memory management.
     */
    template <typename T> class DeviceMemory
    {
    public:
      /**
       * Allocates a buffer with the requested number of elements of type T.
       *
       * @param count Number of elements of type T to allocate.
       */
      DeviceMemory(std::size_t count);

      /**
       * Deallocates the device memory.
       */
      ~DeviceMemory();

      /**
       * Disable copy construction.
       */
      DeviceMemory(const DeviceMemory&) = delete;

      /**
       * Disable move construction.
       */
      DeviceMemory(DeviceMemory&& vao) = delete;

      /**
       * Disable assignment.
       */
      DeviceMemory& operator=(const DeviceMemory&) = delete;

      /**
       * Disable move assignment.
       */
      DeviceMemory& operator=(DeviceMemory&& vao) = delete;

      /**
       * Releases any memory previously allocated by this object and binds to this object the GL buffer passed as argument.
       * @param glId GL identifier associated to the buffer that is going to be bound to this object.
       * @return True if the binding was successful, false otherwise.
       * @note The bound memory will not be considered to owned by this instance, and therefore will not be
       * released when its destructor is called.
       */
      bool bind(GLuint index);

      /**
       * Copies data from host to device.
       *
       * @param hostPtr Pointer to the host memory to be copied to device.
       * @param count Number of elements of type T to copy.
       */
      bool copyFrom(const T* hostPtr, size_t count);

      /**
       * Copies data from device to host.
       *
       * @param hostPtr Pointer to the host memory where the device data will be copied to.
       * @param count Number of elements of type T to copy.
       */
      bool copyTo(T* hostPtr, size_t count) const;

    private:
      GLuint mSsbo;
      std::unordered_map<GLuint, cudaGraphicsResource_t> mCudaResourceMap;
      bool mOwnsMemory;
    };
  }
}

#endif  // SRC_GPGPU_CUDA_DEVICEMEMORY_H_
