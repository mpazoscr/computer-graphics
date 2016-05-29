#ifndef SRC_IMAGE_TEXTURE_H_
#define SRC_IMAGE_TEXTURE_H_

#include "Image.hpp"

namespace mk
{
  namespace image
  {
    /**
     * Class representing a 2D texture stored in GPU.
     */
    class Texture
    {
    public:
      /**
       * Allocates a texture in GPU with a given size in bytes
       * @param size Texture size in bytes
       */
      Texture(int size);

      /**
       * Allocates a texture in GPU with a given size in bytes
       * @param image Texture data
       */
      Texture(const Image& image);

      /**
       * Binds the GPU resource
       */
      void bind();
    };
  }
}

#endif  // SRC_IMAGE_TEXTURE_H_
