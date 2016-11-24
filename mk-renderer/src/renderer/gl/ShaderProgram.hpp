#ifndef SRC_RENDERER_GL_SHADERPROGRAM_H_
#define SRC_RENDERER_GL_SHADERPROGRAM_H_

#include <string>

#include <GL/glew.h>

namespace mk
{
  namespace renderer
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
         * Move constructor.
         */
        ShaderProgram(ShaderProgram&&) = default;

        /**
         * Move assignment operator.
         */
        ShaderProgram& operator=(ShaderProgram&&) = default;

        /**
         * Disable copy construction.
         */
        ShaderProgram(const ShaderProgram&) = default;

        /**
         * Disable assignment.
         */
        ShaderProgram& operator=(const ShaderProgram&) = delete;

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
         * Runs compute shader and waits until execution is complete using the appropriate GPU memory barrier.
         * @param numGroupsX Number of groups for X dimension.
         * @param numGroupsX Number of groups for Y dimension.
         * @param numGroupsX Number of groups for Z dimension.
         * @note This function can only be used on programs built from a compute shader.
         */
        void dispatchCompute(unsigned int numGroupsX, unsigned int numGroupsY, unsigned int numGroupsZ);

        /**
         * Sets a uniform variable for this program of type mat4
         */
        void setUniformMatrix4fv(const std::string& varName, const GLfloat* matrix);

        /**
        * Sets a uniform variable for this program of type uvec2
        */
        void setUniformVector2uv(const std::string& varName, const GLuint* vector);

        /**
        * Sets a uniform variable for this program of type vec2
        */
        void setUniformVector2fv(const std::string& varName, const GLfloat* vector);

        /**
         * Sets a uniform variable for this program of type vec3
         */
        void setUniformVector3fv(const std::string& varName, const GLfloat* vector);

        /**
         * Sets a uniform variable for this program of type int
         */
        void setUniform1i(const std::string& varName, const GLint value);

        /**
        * Sets a uniform variable for this program of type float
        */
        void setUniform1f(const std::string& varName, const GLfloat value);

      private:
        enum ShaderType
        {
          kShaderTypeNone,
          kShaderTypeRender,
          kShaderTypeCompute
        };

        void attachShader(GLenum shaderType, GLuint& shader, const std::string& shaderSrc);

        GLuint mVertexShader;
        GLuint mFragmentShader;
        GLuint mComputeShader;
        GLuint mProgram;
        ShaderType mShaderType;
      };
    }
  }
}

#endif  // SRC_RENDERER_GL_SHADERPROGRAM_H_
