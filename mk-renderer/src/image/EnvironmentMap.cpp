#include "EnvironmentMap.hpp"

#include <cstdint>

namespace mk
{
  namespace image
  {
    EnvironmentMap::EnvironmentMap()
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

    void EnvironmentMap::setMinusZ(int width, int height, GLenum format, const void* data)
    {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    }

    void EnvironmentMap::setPlusZ(int width, int height, GLenum format, const void* data)
    {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    }

    void EnvironmentMap::setMinusY(int width, int height, GLenum format, const void* data)
    {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    }

    void EnvironmentMap::setPlusY(int width, int height, GLenum format, const void* data)
    {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    }

    void EnvironmentMap::setMinusX(int width, int height, GLenum format, const void* data)
    {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    }

    void EnvironmentMap::setPlusX(int width, int height, GLenum format, const void* data)
    {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    }

    void EnvironmentMap::bind()
    {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_CUBE_MAP, mTexture);
    }

    void EnvironmentMap::release()
    {
      glDeleteTextures(1, &mTexture);
    }
  }
}
