#ifndef SRC_RENDERER_GL_FBO_H_
#define SRC_RENDERER_GL_FBO_H_

#include <GL/glew.h>

namespace mk
{
  namespace renderer
  {
    namespace gl
    {
      class Texture;

      class Fbo
      {
      public:
        /**
         * Creates an empty frame buffer object
         */
        Fbo();

        /**
         * If release has not been called before, this destructor releases the associated GPU resources.
         */
        ~Fbo();

        /**
         * Disable copy construction.
         */
        Fbo(const Fbo&) = delete;

        /**
         * Disable assignment.
         */
        Fbo& operator=(const Fbo&) = delete;
        
        /**
         * Move constructor.
         * @param fbo Instance to be moved.
         */
        Fbo(Fbo&& fbo);
        
        /**
         * Move assignment operator.
         * @param fbo Instance to be moved.
         */
        Fbo& operator=(Fbo&& fbo);
        
        /**
         * Binds the Fbo.
         */
        void bind() const;
        
        /**
         * Unbinds the Fbo.
         */
        void unbind() const;
        
        /**
         * Attaches the given texture as the ith color attachment.
         * @param target Color attachment (in the range [0, 15]).
         * @param texture Texture to attach to this Fbo.
         */
        void attachTexture(unsigned int target, const Texture& texture) const;
        
        /**
         * Dettaches ith color attachment.
         * @param target Color attachment (in the range [0, 15]).
         */
        void dettachTexture(unsigned int target) const;
        
        /**
         * @return True if the Fbo is ready to use or false otherwise.
         */
        bool isValid() const;
        
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

#endif  // SRC_RENDERER_GL_FBO_H_
