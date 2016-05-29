#ifndef SRC_IMAGE_ENVIRONMENT_H_
#define SRC_IMAGE_ENVIRONMENT_H_

#include <GL/glew.h>

namespace mk
{
  namespace image
  {
    /**
     * Environment map composed of 6 textures that are stored in GPU.
     */
    class EnvironmentMap
    {
    public:
      /**
       * Allocates a texture object in GPU
       */
      EnvironmentMap();

      /**
       * Uploads the data for the size of the environment map that corresponds to -Z
       * @param width Image width
       * @param height Image height
       * @param format Image GL format (GL_RGB or GL_RGBA)
       * @param data Image data
       * @note The environment map has to be bound to be able to use this function. See {@link EnvironmentMap#bind}
       */
      void setMinusZ(int width, int height, GLenum format, const void* data);

      /**
       * Uploads the data for the size of the environment map that corresponds to +Z
       * @param width Image width
       * @param height Image height
       * @param format Image GL format (GL_RGB or GL_RGBA)
       * @param data Image data
       * @note The environment map has to be bound to be able to use this function. See {@link EnvironmentMap#bind}
       */
      void setPlusZ(int width, int height, GLenum format, const void* data);

      /**
       * Uploads the data for the size of the environment map that corresponds to -Y
       * @param width Image width
       * @param height Image height
       * @param format Image GL format (GL_RGB or GL_RGBA)
       * @param data Image data
       * @note The environment map has to be bound to be able to use this function. See {@link EnvironmentMap#bind}
       */
      void setMinusY(int width, int height, GLenum format, const void* data);

      /**
       * Uploads the data for the size of the environment map that corresponds to +Y
       * @param width Image width
       * @param height Image height
       * @param format Image GL format (GL_RGB or GL_RGBA)
       * @param data Image data
       * @note The environment map has to be bound to be able to use this function. See {@link EnvironmentMap#bind}
       */
      void setPlusY(int width, int height, GLenum format, const void* data);

      /**
       * Uploads the data for the size of the environment map that corresponds to -X
       * @param width Image width
       * @param height Image height
       * @param format Image GL format (GL_RGB or GL_RGBA)
       * @param data Image data
       * @note The environment map has to be bound to be able to use this function. See {@link EnvironmentMap#bind}
       */
      void setMinusX(int width, int height, GLenum format, const void* data);

      /**
       * Uploads the data for the size of the environment map that corresponds to +X
       * @param width Image width
       * @param height Image height
       * @param format Image GL format (GL_RGB or GL_RGBA)
       * @param data Image data
       * @note The environment map has to be bound to be able to use this function. See {@link EnvironmentMap#bind}
       */
      void setPlusX(int width, int height, GLenum format, const void* data);

      /**
       * Binds the GPU resource
       */
      void bind();

      /**
       * Releases the GPU resources. After this, the object will be useless and binding it will have no effect.
       */
      void release();

    private:
      GLuint mTexture;
    };
  }
}

#endif  // SRC_IMAGE_ENVIRONMENT_H_
