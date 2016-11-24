#ifndef _FBO_HPP_
#define _FBO_HPP_

#include <GL/glew.h>

namespace mk
{
  namespace renderer
  {
    namespace gl
    {
      class Fbo
      {
      public:
        /**
         * Creates an empty frame buffer object
         */
        Fbo();
        
        /**
         * If release() has not been called manually, the destructor releases the associated GPU resource.
         */
        ~Fbo();
        
        /**
         * Binds teh Fbo.
         */
        void bind();
        
        /**
         * Uninds teh Fbo.
         */
        void unbind();
        
        /**
         * Attaches the given texture as the ith color attachment.
         * @param target Color attachment (in the range [0, 15]).
         * @param texture Texture to attach to this Fbo.
         */
        void attachTexture(unsigned int target, const Texture& texture);
        
        /**
         * Dettaches ith color attachment.
         * @param target Color attachment (in the range [0, 15]).
         */
        void dettachTexture(unsigned int target);
        
        /**
         * @return True if the Fbo is ready to use or false otherwise.
         */
        bool isValid();
        
        /**
         * Releases the associated GPU resource.
         */
        void release();
        
      private:
        GLuint mFbo;
      };
    }
  }
}

#endif
