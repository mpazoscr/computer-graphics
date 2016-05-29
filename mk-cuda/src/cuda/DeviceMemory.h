#ifndef SRC_GPGPU_CUDA_DEVICEMEMORY_H_
#define SRC_GPGPU_CUDA_DEVICEMEMORY_H_

#include <GL/glew.h>
#include <cuda_gl_interop.h>
#include <memory>
#include <unordered_map>

namespace mk
{
  namespace cuda
  {
    /**
     * Wrapper around CUDA device-allocated memory that providers copying methods.
     */
    template <typename T> class DeviceMemory
    {
    public:
      /**
       * Initializes an empty DeviceMemory object. No allocations are made.
       *
       * @note {@link DeviceMemory#isValid} will return false after instantiating an object with this constructor.
       */
      DeviceMemory();

      /**
       * Allocates the requested number of elements of type T in the primary device.
       *
       * @param count Number of elements of type T to allocate in the primary device.
       * @note {@link DeviceMemory#isValid} can be used to check if the allocation was successful.
       */
      DeviceMemory(size_t count);

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
       * Allocates the requested number of elements of type T in the primary device.
       *
       * @param count Number of elements of type T to allocate in the primary device.
       * @note {@link DeviceMemory#isValid} can be used to check if the allocation was successful.
       * @note Any previous allocated memory will be released.
       */
      void allocate(size_t count);

      /**
       * Releases any memory previously allocated by this object and binds to this object the GL buffer passed as argument.
       * @param glId GL identifier associated to the buffer that is going to be bound to this object.
       * @return True if the binding was successful, false otherwise.
       * @note The bound memory will not be considered to owned by this instance, and therefore will not be
       * released when its destructor is called.
       */
      bool bindGlBuffer(GLuint glId);

      /**
       * Unbinds the GL buffer passed as argument.
       * @param glId GL identifier associated to the buffer that is going to be unbound from this object.
       * @return True if the unbinding was successful, false otherwise.
       */
      bool unbindGlBuffer(GLuint glId);

      /**
       * Copies data from host to device.
       *
       * @param ptrToHostMem Pointer to the host memory to be copied to device.
       * @param count Number of elements of type T to copy.
       */
      bool copyFrom(const T* ptrToHostMem, size_t count);

      /**
       * Copies data from device to host.
       *
       * @param ptrToHostMem Pointer to the host memory where the device data will be copied to.
       * @param count Number of elements of type T to copy.
       */
      bool copyTo(T* ptrToHostMem, size_t count) const;

      /**
       * @return A reference to the pointer that holds the allocated device memory.
       */
      T* ptr() const;

      /**
       * @return True if the memory is successfuly allocated and ready to use, false otherwise.
       */
      bool isValid() const;

    private:
      T* mDevMemPtr;
      std::unordered_map<GLuint, cudaGraphicsResource_t> mCudaResourceMap;
      bool mOwnsMemory;
    };
  }
}

#endif  // SRC_GPGPU_CUDA_DEVICEMEMORY_H_
