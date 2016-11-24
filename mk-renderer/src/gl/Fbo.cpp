#include "Fbo.hpp"

namespace mk
{
  namespace renderer
  {
    namespace gl
    {
      Fbo::Fbo()
      : mFbo(0)
      {
        glGenFramebuffers(1, &mFbo);
      }
      
      Fbo::~Fbo()
      {
        release();
      }
      
      void Fbo::bind()
      {
        glBindFramebuffer(GL_FRAMEBUFFER, mFbo);
      }
      
      void Fbo::unbind()
      {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
      }
      
      void Fbo::attachTexture(unsigned int target, const Texture& texture)
      {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + target, GL_TEXTURE_2D, texture.getId(), 0);
      }
      
      void Fbo::dettachTexture(unsigned int target)
      {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + target, GL_TEXTURE_2D, 0, 0);
      }
      
      bool Fbo::isValid()
      {
        return glCheckFrameBufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
      }
      
      void Fbo::release()
      {
        glDeleteFramebuffers(1, &mFbo);
      }
    }
  }
}
