#ifndef SRC_RENDERER_GL_TEXTURE_H_
#define SRC_RENDERER_GL_TEXTURE_H_

#include "renderer/Image.hpp"

namespace mk
{
  namespace renderer
  {
    namespace gl
    {
      /**
       * Class representing a 2D texture stored in GPU.
       */
      class Texture
      {
      public:
        /**
         * Allocates a texture in GPU with a given size in bytes.
         * @param width The width of the texture in bytes.
         * @param height The height of the texture in bytes.
         */
        Texture(unsigned int width, unsigned int height);

        /**
         * Allocates a texture in GPU and uploads the data contained in image.
         * @param image Texture data
         */
        Texture(const Image& image);

        /**
         * If release has not been called before, this destructor releases the associated GPU resources.
         */
        ~Texture();

        /**
         * Disable copy construction.
         */
        Texture(const Texture&) = delete;

        /**
         * Disable assignment.
         */
        Texture& operator=(const Texture&) = delete;
        
        /**
         * Move constructor.
         * @param texture Instance to be moved.
         */
        Texture(Texture&& texture);
        
        /**
         * Move assignment operator.
         * @param texture Instance to be moved.
         */
        Texture& operator=(Texture&& texture);

        /**
         * @return GL texture ID.
         */
        GLuint getId() const;

        /**
         * Binds the texture.
         * @param textureUnit Unit to which the texture will be bound.
         */
        void bind(unsigned int textureUnit);

        /**
         * Unbinds the texture.
         * @param textureUnit Unit from which the texture will be unbound.
         */
        void unbind(unsigned int textureUnit);

        /**
         * Release the GPU memory used by this texture.
         */
        void release();

      private:
        GLuint mId;
      };
    }
  }
}

#endif  // SRC_RENDERER_GL_TEXTURE_H_
