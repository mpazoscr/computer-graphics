#include "Vao.h"

#include "core/VertexTypes.h"

namespace mk
{
  namespace gl
  {
    // Default implementation of constructors for types not defined in core/VertexTypes.h

    template <typename T> Vao<T>::Vao(const std::vector<T>& buffer, GLenum usage)
    : mVbo(0),
      mVao(0),
      mIndices(0)
    {
    }

    template <typename T> Vao<T>::Vao(const std::vector<T>& buffer, const std::vector<unsigned int>& indexBuffer, GLenum usage)
    : mVbo(0),
      mVao(0),
      mIndices(0)
    {
    }

    // Define one template specialization per Vertex type defined in core/VertexTypes.h

    template <> Vao<mk::core::VertexP>::Vao(const std::vector<mk::core::VertexP>& vertexBuffer,
                                            GLenum usage)
    : mVbo(0),
      mVao(0),
      mIndices(0)
    {
      glGenVertexArrays(1, &mVao);
      glBindVertexArray(mVao);

      std::vector<unsigned int> indexBuffer;
      allocateVbos(vertexBuffer, indexBuffer, usage);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

      glEnableVertexAttribArray(0);
    }

    template <> Vao<mk::core::VertexPN>::Vao(const std::vector<mk::core::VertexPN>& vertexBuffer,
                                             GLenum usage)
    : mVbo(0),
      mVao(0),
      mIndices(0)
    {
      glGenVertexArrays(1, &mVao);
      glBindVertexArray(mVao);

      std::vector<unsigned int> indexBuffer;
      allocateVbos(vertexBuffer, indexBuffer, usage);

      const size_t sizeOfVertex = sizeof(mk::core::VertexPN);
      const size_t normalOffset = sizeof(mk::core::VertexPN::mPos);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, nullptr);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, reinterpret_cast<const GLvoid*>(0 + normalOffset));

      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
    }

    template <> Vao<mk::core::VertexPNC>::Vao(const std::vector<mk::core::VertexPNC>& vertexBuffer,
                                              GLenum usage)
    : mVbo(0),
      mVao(0),
      mIndices(0)
    {
      glGenVertexArrays(1, &mVao);
      glBindVertexArray(mVao);

      std::vector<unsigned int> indexBuffer;
      allocateVbos(vertexBuffer, indexBuffer, usage);

      const size_t sizeOfVertex = sizeof(mk::core::VertexPNC);
      const size_t normalOffset = sizeof(mk::core::VertexPNC::mPos);
      const size_t colourOffset = sizeof(mk::core::VertexPNC::mPos) + sizeof(mk::core::VertexPNC::mNormal);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, nullptr);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, reinterpret_cast<const GLvoid*>(0 + normalOffset));
      glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeOfVertex, reinterpret_cast<const GLvoid*>(0 + colourOffset));
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glEnableVertexAttribArray(2);
    }

    template <> Vao<mk::core::VertexPNT>::Vao(const std::vector<mk::core::VertexPNT>& vertexBuffer,
                                              GLenum usage)
    : mVbo(0),
      mVao(0),
      mIndices(0)
    {
      glGenVertexArrays(1, &mVao);
      glBindVertexArray(mVao);

      std::vector<unsigned int> indexBuffer;
      allocateVbos(vertexBuffer, indexBuffer, usage);

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

    template <> Vao<mk::core::VertexP>::Vao(const std::vector<mk::core::VertexP>& vertexBuffer,
                                            const std::vector<unsigned int>& indexBuffer,
                                            GLenum usage)
    : mVbo(0),
      mVao(0),
      mIndices(0)
    {
      glGenVertexArrays(1, &mVao);
      glBindVertexArray(mVao);

      allocateVbos(vertexBuffer, indexBuffer, usage);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

      glEnableVertexAttribArray(0);
    }

    template <> Vao<mk::core::VertexPN>::Vao(const std::vector<mk::core::VertexPN>& vertexBuffer,
                                             const std::vector<unsigned int>& indexBuffer,
                                             GLenum usage)
    : mVbo(0),
      mVao(0),
      mIndices(0)
    {
      glGenVertexArrays(1, &mVao);
      glBindVertexArray(mVao);

      allocateVbos(vertexBuffer, indexBuffer, usage);

      const size_t sizeOfVertex = sizeof(mk::core::VertexPN);
      const size_t normalOffset = sizeof(mk::core::VertexPN::mPos);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, nullptr);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, reinterpret_cast<const GLvoid*>(0 + normalOffset));

      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
    }

    template <> Vao<mk::core::VertexPNC>::Vao(const std::vector<mk::core::VertexPNC>& vertexBuffer,
                                              const std::vector<unsigned int>& indexBuffer,
                                              GLenum usage)
    : mVbo(0),
      mVao(0),
      mIndices(0)
    {
      glGenVertexArrays(1, &mVao);
      glBindVertexArray(mVao);

      allocateVbos(vertexBuffer, indexBuffer, usage);

      const size_t sizeOfVertex = sizeof(mk::core::VertexPNC);
      const size_t normalOffset = sizeof(mk::core::VertexPNC::mPos);
      const size_t colourOffset = sizeof(mk::core::VertexPNC::mPos) + sizeof(mk::core::VertexPNC::mNormal);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, nullptr);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, reinterpret_cast<const GLvoid*>(0 + normalOffset));
      glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeOfVertex, reinterpret_cast<const GLvoid*>(0 + colourOffset));
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glEnableVertexAttribArray(2);
    }

    template <> Vao<mk::core::VertexPNT>::Vao(const std::vector<mk::core::VertexPNT>& vertexBuffer,
                                              const std::vector<unsigned int>& indexBuffer,
                                              GLenum usage)
    : mVbo(0),
      mVao(0),
      mIndices(0)
    {
      glGenVertexArrays(1, &mVao);
      glBindVertexArray(mVao);

      allocateVbos(vertexBuffer, indexBuffer, usage);

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
      if (mVbo)
      {
        glDeleteBuffers(1, &mVbo);
      }
      if (mIndices)
      {
        glDeleteBuffers(1, &mIndices);
      }
    }

    template <typename T> GLuint Vao<T>::getVboId()
    {
      return mVbo;
    }

    template <typename T> void Vao<T>::refreshData(int offset, const T* buffer, int size)
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

    template <> void Vao<mk::core::VertexPNC>::bind()
    {
      glBindVertexArray(mVao);
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glEnableVertexAttribArray(2);
    }

    template <> void Vao<mk::core::VertexPNT>::bind()
    {
      glBindVertexArray(mVao);
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glEnableVertexAttribArray(2);
    }

    template <typename T> void Vao<T>::unbind()
    {
      glBindVertexArray(0);
    }

    template <typename T> void Vao<T>::render(GLenum mode, GLsizei count)
    {
      if (mIndices == 0)
      {
        glDrawArrays(mode, 0, count);
      }
      else
      {
        glDrawElements(mode, count, GL_UNSIGNED_INT, nullptr);
      }
    }

    template <typename T> void Vao<T>::allocateVbos(const std::vector<T>& vertexBuffer,
                                                    const std::vector<unsigned int>& indexBuffer,
                                                    GLenum usage)
    {
      glGenBuffers(1, &mVbo);
      glBindBuffer(GL_ARRAY_BUFFER, mVbo);
      glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(T), vertexBuffer.data(), usage);

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
    template class Vao<mk::core::VertexPNC>;
    template class Vao<mk::core::VertexPNT>;
  }
}
