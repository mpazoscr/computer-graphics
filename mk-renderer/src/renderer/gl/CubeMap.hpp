#ifndef SRC_RENDERER_GL_CUBEMAP_H_
#define SRC_RENDERER_GL_CUBEMAP_H_

#include <GL/glew.h>

#include <cstddef>

namespace mk
{
  namespace renderer
  {
    namespace gl
    {
      /**
       * Environment map composed of 6 textures that are stored in GPU.
       */
      class CubeMap
      {
      public:
        /**
         * Allocates a texture object in GPU
         */
        CubeMap();

        /**
         * If release has not been called before, this destructor releases the associated GPU resources.
         */
        ~CubeMap();

        /**
         * Disable copy construction.
         */
        CubeMap(const CubeMap&) = delete;

        /**
         * Disable assignment.
         */
        CubeMap& operator=(const CubeMap&) = delete;
        
        /**
         * Move constructor.
         * @param cubeMap Instance to be moved.
         */
        CubeMap(CubeMap&& cubeMap);
        
        /**
         * Move assignment operator.
         * @param cubeMap Instance to be moved.
         */
        CubeMap& operator=(CubeMap&& cubeMap);

        /**
         * Uploads the data for the side of the environment map that corresponds to -Z
         * @param width Image width
         * @param height Image height
         * @param format Image GL format (GL_RGB or GL_RGBA)
         * @param data Image data
         * @note The environment map has to be bound to be able to use this function. See {@link EnvironmentMap#bind}
         */
        void setMinusZ(std::size_t width, std::size_t height, GLenum format, const void* data);

        /**
         * Uploads the data for the side of the environment map that corresponds to +Z
         * @param width Image width
         * @param height Image height
         * @param format Image GL format (GL_RGB or GL_RGBA)
         * @param data Image data
         * @note The environment map has to be bound to be able to use this function. See {@link EnvironmentMap#bind}
         */
        void setPlusZ(std::size_t width, std::size_t height, GLenum format, const void* data);

        /**
         * Uploads the data for the side of the environment map that corresponds to -Y
         * @param width Image width
         * @param height Image height
         * @param format Image GL format (GL_RGB or GL_RGBA)
         * @param data Image data
         * @note The environment map has to be bound to be able to use this function. See {@link EnvironmentMap#bind}
         */
        void setMinusY(std::size_t width, std::size_t height, GLenum format, const void* data);

        /**
         * Uploads the data for the side of the environment map that corresponds to +Y
         * @param width Image width
         * @param height Image height
         * @param format Image GL format (GL_RGB or GL_RGBA)
         * @param data Image data
         * @note The environment map has to be bound to be able to use this function. See {@link EnvironmentMap#bind}
         */
        void setPlusY(std::size_t width, std::size_t height, GLenum format, const void* data);

        /**
         * Uploads the data for the side of the environment map that corresponds to -X
         * @param width Image width
         * @param height Image height
         * @param format Image GL format (GL_RGB or GL_RGBA)
         * @param data Image data
         * @note The environment map has to be bound to be able to use this function. See {@link EnvironmentMap#bind}
         */
        void setMinusX(std::size_t width, std::size_t height, GLenum format, const void* data);

        /**
         * Uploads the data for the side of the environment map that corresponds to +X
         * @param width Image width
         * @param height Image height
         * @param format Image GL format (GL_RGB or GL_RGBA)
         * @param data Image data
         * @note The environment map has to be bound to be able to use this function. See {@link EnvironmentMap#bind}
         */
        void setPlusX(std::size_t width, std::size_t height, GLenum format, const void* data);

        /**
         * Uploads the data for the side of the environment map give by side
         * @param target Side of the environment map to which this image will be set
         * @param width Image width
         * @param height Image height
         * @param format Image GL format (GL_RGB or GL_RGBA)
         * @param data Image data
         * @note The environment map has to be bound to be able to use this function. See {@link EnvironmentMap#bind}
         */
        void setImage(GLenum target, std::size_t width, std::size_t height, GLenum format, const void* data);

        /**
         * Binds the GPU resource
         */
        void bind() const;

        /**
         * Releases the GPU resources. After this, the object will be useless and binding it will have no effect.
         */
        void release();

      private:
        GLuint mTexture;
      };
    }
  }
}

#endif  // SRC_RENDERER_GL_CUBEMAP_H_
