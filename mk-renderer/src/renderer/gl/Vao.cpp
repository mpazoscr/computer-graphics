#include "Vao.hpp"

#include "renderer/VertexTypes.hpp"

namespace mk
{
  namespace renderer
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

      template <> Vao<VertexP>::Vao(const std::vector<VertexP>& dataBuffer, GLenum usage)
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

      template <> Vao<VertexPN>::Vao(const std::vector<VertexPN>& dataBuffer, GLenum usage)
      : mBuffer(0),
        mVao(0),
        mIndices(0)
      {
        glGenVertexArrays(1, &mVao);
        glBindVertexArray(mVao);

        std::vector<unsigned int> indexBuffer;
        allocateBuffers(dataBuffer, indexBuffer, usage);

        const size_t sizeOfVertex = sizeof(VertexPN);
        const size_t normalOffset = sizeof(VertexPN::mPos);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, nullptr);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, reinterpret_cast<const GLvoid*>(0 + normalOffset));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
      }

      template <> Vao<VertexPC>::Vao(const std::vector<VertexPC>& dataBuffer, GLenum usage)
      : mBuffer(0),
        mVao(0),
        mIndices(0)
      {
        glGenVertexArrays(1, &mVao);
        glBindVertexArray(mVao);

        std::vector<unsigned int> indexBuffer;
        allocateBuffers(dataBuffer, indexBuffer, usage);

        const size_t sizeOfVertex = sizeof(VertexPC);
        const size_t colourOffset = sizeof(VertexPC::mPos);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, nullptr);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeOfVertex, reinterpret_cast<const GLvoid*>(0 + colourOffset));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
      }

      template <> Vao<VertexPNT>::Vao(const std::vector<VertexPNT>& dataBuffer, GLenum usage)
      : mBuffer(0),
        mVao(0),
        mIndices(0)
      {
        glGenVertexArrays(1, &mVao);
        glBindVertexArray(mVao);

        std::vector<unsigned int> indexBuffer;
        allocateBuffers(dataBuffer, indexBuffer, usage);

        const size_t sizeOfVertex = sizeof(VertexPNT);
        const size_t normalOffset = sizeof(VertexPNT::mPos);
        const size_t texCoordsOffset = sizeof(VertexPNT::mPos) + sizeof(VertexPNT::mNormal);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, nullptr);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, reinterpret_cast<const GLvoid*>(0 + normalOffset));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeOfVertex, reinterpret_cast<const GLvoid*>(0 + texCoordsOffset));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
      }

      template <> Vao<VertexP>::Vao(const std::vector<VertexP>& dataBuffer,
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

      template <> Vao<VertexPN>::Vao(const std::vector<VertexPN>& dataBuffer,
                                     const std::vector<unsigned int>& indexBuffer,
                                     GLenum usage)
      : mBuffer(0),
        mVao(0),
        mIndices(0)
      {
        glGenVertexArrays(1, &mVao);
        glBindVertexArray(mVao);

        allocateBuffers(dataBuffer, indexBuffer, usage);

        const size_t sizeOfVertex = sizeof(VertexPN);
        const size_t normalOffset = sizeof(VertexPN::mPos);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, nullptr);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, reinterpret_cast<const GLvoid*>(0 + normalOffset));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
      }

      template <> Vao<VertexPC>::Vao(const std::vector<VertexPC>& dataBuffer,
                                     const std::vector<unsigned int>& indexBuffer,
                                     GLenum usage)
      : mBuffer(0),
        mVao(0),
        mIndices(0)
      {
        glGenVertexArrays(1, &mVao);
        glBindVertexArray(mVao);

        allocateBuffers(dataBuffer, indexBuffer, usage);

        const size_t sizeOfVertex = sizeof(VertexPC);
        const size_t colourOffset = sizeof(VertexPC::mPos);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, nullptr);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeOfVertex, reinterpret_cast<const GLvoid*>(0 + colourOffset));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
      }

      template <> Vao<VertexPNT>::Vao(const std::vector<VertexPNT>& dataBuffer,
                                      const std::vector<unsigned int>& indexBuffer,
                                      GLenum usage)
      : mBuffer(0),
        mVao(0),
        mIndices(0)
      {
        glGenVertexArrays(1, &mVao);
        glBindVertexArray(mVao);

        allocateBuffers(dataBuffer, indexBuffer, usage);

        const size_t sizeOfVertex = sizeof(VertexPNT);
        const size_t normalOffset = sizeof(VertexPNT::mPos);
        const size_t texCoordsOffset = sizeof(VertexPNT::mPos) + sizeof(VertexPNT::mNormal);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, nullptr);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, reinterpret_cast<const GLvoid*>(0 + normalOffset));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeOfVertex, reinterpret_cast<const GLvoid*>(0 + texCoordsOffset));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
      }

      template <typename T> Vao<T>::~Vao()
      {
        glDeleteBuffers(1, &mBuffer);
        glDeleteBuffers(1, &mIndices);
        glDeleteVertexArrays(1, &mVao);
      }
      
      template <typename T> Vao<T>::Vao(Vao<T>&& vao)
      {
        mBuffer = vao.mBuffer;
        mVao = vao.mVao;
        mIndices = vao.mIndices;
        
        vao.mBuffer = 0;
        vao.mVao = 0;
        vao.mIndices = 0;
      }
      
      template <typename T> Vao<T>& Vao<T>::operator=(Vao<T>&& vao)
      {
        mBuffer = vao.mBuffer;
        mVao = vao.mVao;
        mIndices = vao.mIndices;
        
        vao.mBuffer = 0;
        vao.mVao = 0;
        vao.mIndices = 0;
        
        return *this;
      }

      template <typename T> GLuint Vao<T>::getBufferId()
      {
        return mBuffer;
      }

      template <typename T> void Vao<T>::refreshData(int offset, const T* buffer, std::size_t size)
      {
        void* devPtr = glMapBufferRange(GL_ARRAY_BUFFER, offset * sizeof(T), size * sizeof(T), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
        std::memcpy(devPtr, buffer, size * sizeof(T));
        glUnmapBuffer(GL_ARRAY_BUFFER);
      }

      template <typename T> void Vao<T>::bind()
      {
      }

      template <> void Vao<VertexP>::bind()
      {
        glBindVertexArray(mVao);
        glEnableVertexAttribArray(0);
      }

      template <> void Vao<VertexPN>::bind()
      {
        glBindVertexArray(mVao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
      }

      template <> void Vao<VertexPC>::bind()
      {
        glBindVertexArray(mVao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
      }

      template <> void Vao<VertexPNT>::bind()
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

      template class Vao<VertexP>;
      template class Vao<VertexPN>;
      template class Vao<VertexPC>;
      template class Vao<VertexPNT>;
    }
  }
}
