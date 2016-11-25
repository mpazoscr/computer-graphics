#ifndef SRC_RENDERER_GL_VAO_H_
#define SRC_RENDERER_GL_VAO_H_

#include <vector>
#include <cstddef>

#include <GL/glew.h>

namespace mk
{
  namespace renderer
  {
    namespace gl
    {
      /**
       * Class encapsulating a Vertex Array Object.
       *
       * T should be one of the Vertex types defined in core/VertexTypes.h, otherwise the
       * use of the Vao will have no effect.
       *
       * @note This implementation always a unique array with interleaved attributes.
       */
      template <typename T> class Vao
      {
      public:
        /**
         * Allocates GPU resources and binds the Vao for usage.
         *
         * @param dataBuffer Vector containing the vertices that will be uploaded to the GPU.
         * @param usage Expected usage pattern of the data store.
         *              Please refer to https://www.opengl.org/sdk/docs/man/html/glBufferData.xhtml.
         */
        Vao(const std::vector<T>& dataBuffer, GLenum usage);

        /**
         * Allocates GPU resources and binds the Vao for usage.
         *
         * @param dataBuffer Vector containing the vertices that will be uploaded to the GPU.
         * @param indexBuffer Vector with the indices that refer to the vertices contained in dataBuffer.
         * @param usage Expected usage pattern of the data store.
         *              Please refer to https://www.opengl.org/sdk/docs/man/html/glBufferData.xhtml.
         */
        Vao(const std::vector<T>& dataBuffer, const std::vector<unsigned int>& indexBuffer, GLenum usage);

        /**
         * Releases GPU resources.
         */
        ~Vao();

        /**
         * Disable copy construction.
         */
        Vao(const Vao&) = delete;

        /**
         * Disable assignment.
         */
        Vao& operator=(const Vao&) = delete;
        
        /**
         * Move constructor.
         * @param vao Instance to be moved.
         */
        Vao(Vao&& vao);
        
        /**
         * Move assignment operator.
         * @param vao Instance to be moved.
         */
        Vao& operator=(Vao&& vao);

        /**
         * @return GL identifier associated to the buffer managed by this object.
         */
        GLuint getBufferId();

        /**
         * Refreshes the contents of the Vao with new data
         * @param offset The offset from which to modify the destination buffer in number of elements of type T.
         * @param buffer Pointer to the beginning of the buffer
         * @param size Number of elements in buffer
         * @note The vao must be bind()'d to be able to refresh its contents
         */
        void refreshData(int offset, const T* buffer, std::size_t size);

        /**
         * Binds the Vao so that it is ready to be rendered.
         */
        void bind();

        /**
         * Binds the underlying buffer as an SSBO so that it is ready to be accessed from a compute shader.
         * @param index Index to bind the buffer to, which must match the one declared in the compute shader.
         */
        void bind(unsigned int index);

        /**
         * Unbinds the Vao.
         */
        void unbind();

        /**
         * Renders the Vao
         *
         * @param mode Specifies what kind of primitives to render.
         * Please refer to https://www.opengl.org/sdk/docs/man/html/glDrawElements.xhtml or https://www.opengl.org/sdk/docs/man/html/glDrawArrays.xhtml.
         * @param count Specifies the number of elements to be rendered.
         */
        void render(GLenum mode, std::size_t count);

      private:
        GLuint mBuffer;
        GLuint mVao;
        GLuint mIndices;

        void allocateBuffers(const std::vector<T>& dataBuffer, const std::vector<unsigned int>& indexBuffer, GLenum usage);
      };
    }
  }
}

#endif  // SRC_RENDERER_GL_VAO_H_
