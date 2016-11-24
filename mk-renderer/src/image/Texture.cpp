#include "Texture.hpp"

namespace mk
{
  namespace image
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
      glGenTextures(1, &mId);
      glBindTexture(GL_TEXTURE_2D, mId);
      
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.getWidth(), image.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.data());
      
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    
    Texture::~Texture()
    {
      release();
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
