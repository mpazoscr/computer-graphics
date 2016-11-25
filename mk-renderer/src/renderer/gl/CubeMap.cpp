#include "CubeMap.hpp"

#include <cstdint>

namespace mk
{
  namespace renderer
  {
    namespace gl
    {
      CubeMap::CubeMap()
      {
        glGenTextures(1, &mTexture);
        glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_CUBE_MAP, mTexture);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      }

      CubeMap::~CubeMap()
      {
        release();
      }
      
      CubeMap::CubeMap(CubeMap&& cubeMap)
      {
        mTexture = cubeMap.mTexture;
        cubeMap.mTexture = 0;
      }
      
      CubeMap& CubeMap::operator=(CubeMap&& cubeMap)
      {
        mTexture = cubeMap.mTexture;
        cubeMap.mTexture = 0;
        
        return *this;
      }

      void CubeMap::setMinusZ(std::size_t width, std::size_t height, GLenum format, const void* data)
      {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, format, GL_UNSIGNED_BYTE, data);
      }

      void CubeMap::setPlusZ(std::size_t width, std::size_t height, GLenum format, const void* data)
      {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, format, GL_UNSIGNED_BYTE, data);
      }

      void CubeMap::setMinusY(std::size_t width, std::size_t height, GLenum format, const void* data)
      {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, format, GL_UNSIGNED_BYTE, data);
      }

      void CubeMap::setPlusY(std::size_t width, std::size_t height, GLenum format, const void* data)
      {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, format, GL_UNSIGNED_BYTE, data);
      }

      void CubeMap::setMinusX(std::size_t width, std::size_t height, GLenum format, const void* data)
      {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, format, GL_UNSIGNED_BYTE, data);
      }

      void CubeMap::setPlusX(std::size_t width, std::size_t height, GLenum format, const void* data)
      {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, format, GL_UNSIGNED_BYTE, data);
      }

      void CubeMap::setImage(GLenum target, std::size_t width, std::size_t height, GLenum format, const void* data)
      {
        glTexImage2D(target, 0, format, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, format, GL_UNSIGNED_BYTE, data);
      }

      void CubeMap::bind() const
      {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, mTexture);
      }

      void CubeMap::release()
      {
        glDeleteTextures(1, &mTexture);
        mTexture = 0;
      }
    }
  }
}
