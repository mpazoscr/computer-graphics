#include "ShaderProgram.hpp"

#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include <streambuf>

namespace mk
{
  namespace gl
  {
    const char* kDefaultVertexShader =
        "#version 400\n"
        "layout(location = 0) in vec3 vp;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main () {"
        "  gl_Position = projection * view * vec4 (vp, 1.0);"
        "}";

    const char* kDefaultFragmentShader =
        "#version 400\n"
        "out vec4 frag_colour;\n"
        "void main () {"
        "  frag_colour = vec4 (1.0, 1.0, 1.0, 1.0);"
        "}";

    ShaderProgram::ShaderProgram()
      : mVertexShader(0),
        mFragmentShader(0),
        mProgram(0)
    {
      mProgram = glCreateProgram();
    }

    ShaderProgram::~ShaderProgram()
    {
      glUseProgram(0);
      glDetachShader(mProgram, mVertexShader);
      glDetachShader(mProgram, mFragmentShader);
      glDeleteShader(mVertexShader);
      glDeleteShader(mFragmentShader);
      glDeleteProgram(mProgram);
    }

    void ShaderProgram::attachVertexShader(const std::string& vertexShaderSrc)
    {
      attachShader(GL_VERTEX_SHADER, mVertexShader, vertexShaderSrc);
    }

    void ShaderProgram::attachFragmentShader(const std::string& fragmentShaderSrc)
    {
      attachShader(GL_FRAGMENT_SHADER, mFragmentShader, fragmentShaderSrc);
    }

    void ShaderProgram::link()
    {
      if (0 == mVertexShader)
      {
        attachShader(GL_VERTEX_SHADER,
                     mVertexShader,
                     std::string(kDefaultVertexShader, std::strlen(kDefaultVertexShader)));
      }

      if (0 == mFragmentShader)
      {
        attachShader(GL_FRAGMENT_SHADER,
                     mFragmentShader,
                     std::string(kDefaultFragmentShader, std::strlen(kDefaultFragmentShader)));
      }

      glLinkProgram(mProgram);

      GLint linkStatus;
      glGetProgramiv(mProgram, GL_LINK_STATUS, &linkStatus);

      if (GL_FALSE == linkStatus)
      {
        GLint errorLength;
        glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &errorLength);

        GLchar* errorLog = new GLchar[errorLength];
        glGetProgramInfoLog(mProgram, errorLength, nullptr, errorLog);

        std::cout << "Error linking program.\nLog:\n" << errorLog << std::endl;
      }
    }

    void ShaderProgram::use()
    {
      glUseProgram(mProgram);
    }

    void ShaderProgram::attachShader(GLenum shaderType, GLuint& shader, const std::string& shaderSrc)
    {
      if (shader)
      {
        glDetachShader(mProgram, shader);
        glDeleteShader(shader);
      }

      shader = glCreateShader(shaderType);

      GLint shaderLength = static_cast<GLint>(shaderSrc.length());
      const char* shaderSrcPtr = shaderSrc.data();

      glShaderSource(shader, 1, &shaderSrcPtr, &shaderLength);
      glCompileShader(shader);

      GLint shaderStatus;
      glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderStatus);

      if (GL_FALSE == shaderStatus)
      {
        GLint errorLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errorLength);

        GLchar* errorLog = new GLchar[errorLength];
        glGetShaderInfoLog(shader, errorLength, nullptr, errorLog);

        std::cout << "Error compiling shader:\n" << shaderSrc << "\n\nLog:\n" << errorLog << std::endl;

        glDeleteShader(shader);
        shader = 0;
      }
      else
      {
        glAttachShader(mProgram, shader);
      }
    }

    void ShaderProgram::setUniformMatrix4fv(const std::string& varName, const GLfloat* matrix)
    {
      GLint location = glGetUniformLocation(mProgram, varName.c_str());

      glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
    }

    void ShaderProgram::setUniformVector3fv(const std::string &varName, const GLfloat *vector)
    {
      GLint location = glGetUniformLocation(mProgram, varName.c_str());

      glUniform3fv(location, 1, vector);
    }

    void ShaderProgram::setUniform1i(const std::string& varName, const GLint value)
    {
      GLint location = glGetUniformLocation(mProgram, varName.c_str());

      glUniform1i(location, value);
    }
  }
}
