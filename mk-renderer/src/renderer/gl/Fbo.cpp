#include "Fbo.hpp"

#include "Texture.hpp"

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
      
      void Fbo::bind() const
      {
        glBindFramebuffer(GL_FRAMEBUFFER, mFbo);
      }
      
      void Fbo::unbind() const
      {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
      }
      
      void Fbo::attachTexture(unsigned int target, const Texture& texture) const
      {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + target, GL_TEXTURE_2D, texture.getId(), 0);
      }
      
      void Fbo::dettachTexture(unsigned int target) const
      {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + target, GL_TEXTURE_2D, 0, 0);
      }
      
      bool Fbo::isValid() const
      {
        return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
      }
      
      void Fbo::release()
      {
        glDeleteFramebuffers(1, &mFbo);
        mFbo = 0;
      }
    }
  }
}
