#include "DeviceMemory.hpp"

#include <cstring>

namespace mk
{
  namespace gpgpu
  {
    namespace gl
    {
      template <typename T> DeviceMemory<T>::DeviceMemory(std::size_t count)
      : mSsbo(0)
      {
        glGenBuffers(1, &mSsbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, count * sizeof(T), NULL, GL_DYNAMIC_COPY);
      }

      template <typename T> DeviceMemory<T>::~DeviceMemory()
      {
        glDeleteBuffers(1, &mSsbo);
      }

      template <typename T> GLuint DeviceMemory<T>::getId()
      {
        return mSsbo;
      }

      template <typename T> bool DeviceMemory<T>::bind(GLuint index)
      {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, mSsbo);
      }

      template <typename T> bool DeviceMemory<T>::copyFrom(const T* hostPtr, size_t count)
      {
        const std::size_t size = count * sizeof(T);

        T* devPtr = static_cast<T*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
        std::memcpy(devPtr, hostPtr, size);
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
      }

      template <typename T> bool DeviceMemory<T>::copyTo(T* hostPtr, size_t count) const
      {
        const std::size_t size = count * sizeof(T);

        const T* devPtr = static_cast<T*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size, GL_MAP_READ_BIT));
        std::memcpy(hostPtr, devPtr, size);
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
      }
    }
  }
}
