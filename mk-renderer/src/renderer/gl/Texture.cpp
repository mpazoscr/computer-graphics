#include "Texture.hpp"

namespace mk
{
  namespace renderer
  {
    namespace gl
    {
      Texture::Texture(unsigned int width, unsigned int height)
      : mId(0)
      {
        glGenTextures(1, &mId);
        glBindTexture(GL_TEXTURE_2D, mId);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      }

      Texture::Texture(const Image& image)
      : mId(0)
      {
        const GLsizei width = static_cast<GLsizei>(image.getWidth());
        const GLsizei height = static_cast<GLsizei>(image.getHeight());

        glGenTextures(1, &mId);
        glBindTexture(GL_TEXTURE_2D, mId);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      }

      Texture::~Texture()
      {
        release();
      }
      
      Texture::Texture(Texture&& texture)
      {
        mId = texture.mId;
        texture.mId = 0;
      }
      
      Texture& Texture::operator=(Texture&& texture)
      {
        mId = texture.mId;
        texture.mId = 0;
        
        return *this;
      }

      GLuint Texture::getId() const
      {
        return mId;
      }

      void Texture::bind(unsigned int textureUnit)
      {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, mId);
      }

      void Texture::unbind(unsigned int textureUnit)
      {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, 0);
      }

      void Texture::release()
      {
        glDeleteTextures(1, &mId);
      }
    }
  }
}
