#ifndef SRC_GL_SHADERPROGRAM_H_
#define SRC_GL_SHADERPROGRAM_H_

#include <GL/glew.h>
#include <string>

namespace mk
{
  namespace gl
  {
    class ShaderProgram
    {
    public:
      /**
       * Allocates the program in GPU.
       */
      ShaderProgram();

      /**
       * Releases GPU resources.
       */
      ~ShaderProgram();

      /**
       * Attaches and compiles a vertex shader given its source code.
       *
       * If there was a vertex shader previously attached to this program, it
       * is lost and substituted by this one.
       *
       * @param vertexShaderSrc Vertex shader source.
       * @note This function will assert if a compute shader were attached before.
       */
      void attachVertexShader(const std::string& vertexShaderSrc);

      /**
       * Attaches and compiles a fragment shader given its source code.
       *
       * If there was a fragment shader previously attached to this program, it
       * is lost and substituted by this one.
       *
       * @param fragmentShaderSrc Fragment shader source.
       * @note This function will assert if a compute shader were attached before.
       */
      void attachFragmentShader(const std::string& fragmentShaderSrc);

      /**
       * Attaches and compiles a compute shader given its source code.
       *
       * @param fragmentShaderSrc Fragment shader source.
       * @note This function will assert if a vertex or fragment shader were attached before.
       */
      void attachComputeShader(const std::string& computeShaderSrc);

      /**
       * Links the program to make it ready to use.
       *
       * If one of the shader types has not been attached, a default version will be attached instead.
       * @note The default version for the vertex shader just multiplies an input vec3 position with two
       * uniform matrices named "view" and "projection".
       * @note The default version for the fragment shader just outputs a white opaque fragment.
       */
      void link();

      /**
       * Binds the program to the GPU so that subsequent rendering will use it.
       */
      void use();

      /**
       * Sets a uniform variable for this program of type mat4 float;
       */
      void setUniformMatrix4fv(const std::string& varName, const GLfloat* matrix);

      /**
       * Sets a uniform variable for this program of type vec3 float;
       */
      void setUniformVector3fv(const std::string& varName, const GLfloat* vector);

      /**
       * Sets a uniform variable for this program of type int
       */
      void setUniform1i(const std::string& varName, const GLint value);

    private:
      enum ShaderType
      {
        kShaderTypeNone,
        kShaderTypeRender,
        kShaderTypeCompute
      };

      void attachShader(GLenum shaderType,  GLuint& shader, const std::string& shaderSrc);

      GLuint mVertexShader;
      GLuint mFragmentShader;
      GLuint mComputeShader;
      GLuint mProgram;
      ShaderType mShaderType;
    };
  }
}

#endif  // SRC_GL_SHADERPROGRAM_H_
