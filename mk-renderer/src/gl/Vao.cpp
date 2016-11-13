#include "Vao.hpp"

#include "core/VertexTypes.hpp"

namespace mk
{
  namespace gl
  {
    // Default implementation of constructors for types not defined in core/VertexTypes.h

    template <typename T> Vao<T>::Vao(const std::vector<T>& dataBuffer, GLenum usage)
    : mBuffer(0),
      mVao(0),
      mIndices(0)
    {
    }

    template <typename T> Vao<T>::Vao(const std::vector<T>& dataBuffer, const std::vector<unsigned int>& indexBuffer, GLenum usage)
    : mBuffer(0),
      mVao(0),
      mIndices(0)
    {
    }

    // Define one template specialization per Vertex type defined in core/VertexTypes.h

    template <> Vao<mk::core::VertexP>::Vao(const std::vector<mk::core::VertexP>& dataBuffer,
                                            GLenum usage)
    : mBuffer(0),
      mVao(0),
      mIndices(0)
    {
      glGenVertexArrays(1, &mVao);
      glBindVertexArray(mVao);

      std::vector<unsigned int> indexBuffer;
      allocateBuffers(dataBuffer, indexBuffer, usage);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

      glEnableVertexAttribArray(0);
    }

    template <> Vao<mk::core::VertexPN>::Vao(const std::vector<mk::core::VertexPN>& dataBuffer,
                                             GLenum usage)
    : mBuffer(0),
      mVao(0),
      mIndices(0)
    {
      glGenVertexArrays(1, &mVao);
      glBindVertexArray(mVao);

      std::vector<unsigned int> indexBuffer;
      allocateBuffers(dataBuffer, indexBuffer, usage);

      const size_t sizeOfVertex = sizeof(mk::core::VertexPN);
      const size_t normalOffset = sizeof(mk::core::VertexPN::mPos);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, nullptr);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, reinterpret_cast<const GLvoid*>(0 + normalOffset));

      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
    }

    template <> Vao<mk::core::VertexPC>::Vao(const std::vector<mk::core::VertexPC>& dataBuffer,
                                             GLenum usage)
    : mBuffer(0),
      mVao(0),
      mIndices(0)
    {
      glGenVertexArrays(1, &mVao);
      glBindVertexArray(mVao);

      std::vector<unsigned int> indexBuffer;
      allocateBuffers(dataBuffer, indexBuffer, usage);

      const size_t sizeOfVertex = sizeof(mk::core::VertexPC);
      const size_t colourOffset = sizeof(mk::core::VertexPC::mPos);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, nullptr);
      glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeOfVertex, reinterpret_cast<const GLvoid*>(0 + colourOffset));
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
    }

    template <> Vao<mk::core::VertexPNT>::Vao(const std::vector<mk::core::VertexPNT>& dataBuffer,
                                              GLenum usage)
    : mBuffer(0),
      mVao(0),
      mIndices(0)
    {
      glGenVertexArrays(1, &mVao);
      glBindVertexArray(mVao);

      std::vector<unsigned int> indexBuffer;
      allocateBuffers(dataBuffer, indexBuffer, usage);

      const size_t sizeOfVertex = sizeof(mk::core::VertexPNT);
      const size_t normalOffset = sizeof(mk::core::VertexPNT::mPos);
      const size_t texCoordsOffset = sizeof(mk::core::VertexPNT::mPos) + sizeof(mk::core::VertexPNT::mNormal);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, nullptr);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, reinterpret_cast<const GLvoid*>(0 + normalOffset));
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeOfVertex, reinterpret_cast<const GLvoid*>(0 + texCoordsOffset));
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glEnableVertexAttribArray(2);
    }

    template <> Vao<mk::core::VertexP>::Vao(const std::vector<mk::core::VertexP>& dataBuffer,
                                            const std::vector<unsigned int>& indexBuffer,
                                            GLenum usage)
    : mBuffer(0),
      mVao(0),
      mIndices(0)
    {
      glGenVertexArrays(1, &mVao);
      glBindVertexArray(mVao);

      allocateBuffers(dataBuffer, indexBuffer, usage);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

      glEnableVertexAttribArray(0);
    }

    template <> Vao<mk::core::VertexPN>::Vao(const std::vector<mk::core::VertexPN>& dataBuffer,
                                             const std::vector<unsigned int>& indexBuffer,
                                             GLenum usage)
    : mBuffer(0),
      mVao(0),
      mIndices(0)
    {
      glGenVertexArrays(1, &mVao);
      glBindVertexArray(mVao);

      allocateBuffers(dataBuffer, indexBuffer, usage);

      const size_t sizeOfVertex = sizeof(mk::core::VertexPN);
      const size_t normalOffset = sizeof(mk::core::VertexPN::mPos);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, nullptr);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, reinterpret_cast<const GLvoid*>(0 + normalOffset));

      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
    }

    template <> Vao<mk::core::VertexPC>::Vao(const std::vector<mk::core::VertexPC>& dataBuffer,
                                             const std::vector<unsigned int>& indexBuffer,
                                             GLenum usage)
    : mBuffer(0),
      mVao(0),
      mIndices(0)
    {
      glGenVertexArrays(1, &mVao);
      glBindVertexArray(mVao);

      allocateBuffers(dataBuffer, indexBuffer, usage);

      const size_t sizeOfVertex = sizeof(mk::core::VertexPC);
      const size_t colourOffset = sizeof(mk::core::VertexPC::mPos);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, nullptr);
      glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeOfVertex, reinterpret_cast<const GLvoid*>(0 + colourOffset));
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
    }

    template <> Vao<mk::core::VertexPNT>::Vao(const std::vector<mk::core::VertexPNT>& dataBuffer,
                                              const std::vector<unsigned int>& indexBuffer,
                                              GLenum usage)
    : mBuffer(0),
      mVao(0),
      mIndices(0)
    {
      glGenVertexArrays(1, &mVao);
      glBindVertexArray(mVao);

      allocateBuffers(dataBuffer, indexBuffer, usage);

      const size_t sizeOfVertex = sizeof(mk::core::VertexPNT);
      const size_t normalOffset = sizeof(mk::core::VertexPNT::mPos);
      const size_t texCoordsOffset = sizeof(mk::core::VertexPNT::mPos) + sizeof(mk::core::VertexPNT::mNormal);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, nullptr);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, reinterpret_cast<const GLvoid*>(0 + normalOffset));
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeOfVertex, reinterpret_cast<const GLvoid*>(0 + texCoordsOffset));
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glEnableVertexAttribArray(2);
    }

    template <typename T> Vao<T>::~Vao()
    {
      if (mVao)
      {
        glDeleteVertexArrays(1, &mVao);
      }
      if (mBuffer)
      {
        glDeleteBuffers(1, &mBuffer);
      }
      if (mIndices)
      {
        glDeleteBuffers(1, &mIndices);
      }
    }

    template <typename T> GLuint Vao<T>::getBufferId()
    {
      return mBuffer;
    }

    template <typename T> void Vao<T>::refreshData(int offset, const T* buffer, std::size_t size)
    {
      glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(T), size * sizeof(T), reinterpret_cast<const GLvoid*>(buffer));
    }

    template <typename T> void Vao<T>::bind()
    {
    }

    template <> void Vao<mk::core::VertexP>::bind()
    {
      glBindVertexArray(mVao);
      glEnableVertexAttribArray(0);
    }

    template <> void Vao<mk::core::VertexPN>::bind()
    {
      glBindVertexArray(mVao);
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
    }

    template <> void Vao<mk::core::VertexPC>::bind()
    {
      glBindVertexArray(mVao);
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
    }

    template <> void Vao<mk::core::VertexPNT>::bind()
    {
      glBindVertexArray(mVao);
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glEnableVertexAttribArray(2);
    }

    template <typename T> void Vao<T>::bind(unsigned int index)
    {
      glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, mBuffer);
    }

    template <typename T> void Vao<T>::unbind()
    {
      glBindVertexArray(0);
    }

    template <typename T> void Vao<T>::render(GLenum mode, std::size_t count)
    {
      if (mIndices == 0)
      {
        glDrawArrays(mode, 0, static_cast<GLsizei>(count));
      }
      else
      {
        glDrawElements(mode, static_cast<GLsizei>(count), GL_UNSIGNED_INT, nullptr);
      }
    }

    template <typename T> void Vao<T>::allocateBuffers(const std::vector<T>& dataBuffer,
                                                       const std::vector<unsigned int>& indexBuffer,
                                                       GLenum usage)
    {
      glGenBuffers(1, &mBuffer);
      glBindBuffer(GL_ARRAY_BUFFER, mBuffer);
      glBufferData(GL_ARRAY_BUFFER, dataBuffer.size() * sizeof(T), dataBuffer.data(), usage);

      if (!indexBuffer.empty())
      {
        glGenBuffers(1, &mIndices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.size() * sizeof(unsigned int), indexBuffer.data(), GL_STATIC_DRAW);
      }
    }

    // Template instantiations for vertex types defined in core/VertexTypes.h

    template class Vao<mk::core::VertexP>;
    template class Vao<mk::core::VertexPN>;
    template class Vao<mk::core::VertexPC>;
    template class Vao<mk::core::VertexPNT>;
  }
}
