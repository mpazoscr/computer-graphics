#ifndef SRC_GL_VAO_H_
#define SRC_GL_VAO_H_

#include <GL/glew.h>
#include <vector>

namespace mk
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
       * @param vertexBuffer Vector containing the vertices that will be uploaded to the GPU.
       * @param usage Expected usage pattern of the data store.
       *              Please refer to https://www.opengl.org/sdk/docs/man/html/glBufferData.xhtml.
       */
      Vao(const std::vector<T>& vertexBuffer, GLenum usage);

      /**
       * Allocates GPU resources and binds the Vao for usage.
       *
       * @param vertexBuffer Vector containing the vertices that will be uploaded to the GPU.
       * @param indexBuffer Vector with the indices that refer to the vertices contained in vertexBuffer.
       * @param usage Expected usage pattern of the data store.
       *              Please refer to https://www.opengl.org/sdk/docs/man/html/glBufferData.xhtml.
       */
      Vao(const std::vector<T>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, GLenum usage);

      /**
       * Releases GPU resources.
       */
      ~Vao();

      /**
       * Disable copy construction.
       */
      Vao(const Vao&) = delete;

      /**
       * Disable move construction.
       */
      Vao(Vao&& vao) = delete;

      /**
       * Disable assignment.
       */
      Vao& operator=(const Vao&) = delete;

      /**
       * Disable move assignment.
       */
      Vao& operator=(Vao&& vao) = delete;

      /**
       * @return GL identifier associated to the Vbo managed by this object.
       */
      GLuint getVboId();

      /**
       * Refreshes the contents of the Vao with new data
       * @param offset The offset from which to modify the destination buffer in number of elements of type T.
       * @param buffer Pointer to the beginning of the buffer
       * @param size Number of elements in buffer
       * @note The vao must be bind()'d to be able to refresh its contents
       */
      void refreshData(int offset, const T* buffer, int size);

      /**
       * Binds the Vao so that it is ready to be rendered.
       */
      void bind();

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
      void render(GLenum mode, GLsizei count);

    private:
      GLuint mVao;
      GLuint mVbo;
      GLuint mIndices;

      void allocateVbos(const std::vector<T>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, GLenum usage);
    };
  }
}

#endif  // SRC_GL_VAO_H_
