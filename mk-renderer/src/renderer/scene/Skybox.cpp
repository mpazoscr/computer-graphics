#include "Skybox.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_access.hpp>

#include "renderer/assets/ResourceLoader.hpp"

namespace mk
{
  namespace renderer
  {
    namespace scene
    {
      namespace
      {
        const std::array<glm::vec3, 8> unitCubeVectors
        {
          glm::vec3(-0.5f, -0.5f, -0.5f),
          glm::vec3(-0.5f, -0.5f,  0.5f),
          glm::vec3(-0.5f,  0.5f,  0.5f),
          glm::vec3(-0.5f,  0.5f, -0.5f),
          glm::vec3( 0.5f, -0.5f, -0.5f),
          glm::vec3( 0.5f, -0.5f,  0.5f),
          glm::vec3( 0.5f,  0.5f,  0.5f),
          glm::vec3( 0.5f,  0.5f, -0.5f)
        };
      }

      Skybox::Skybox(const gl::CubeMap& cubeMap, const scene::ICamera& camera)
      : mCubeMap(cubeMap),
        mCamera(camera),
        mShader(),
        mQuads
        {
          mesh::Quad<VertexPN>(    // -Z
            std::array<glm::vec3, 4>
            {
              unitCubeVectors[3],
              unitCubeVectors[0],
              unitCubeVectors[4],
              unitCubeVectors[7]
            }),
          mesh::Quad<VertexPN>(    // +Z
            std::array<glm::vec3, 4>
            {
              unitCubeVectors[6],
              unitCubeVectors[5],
              unitCubeVectors[1],
              unitCubeVectors[2]
            }),
          mesh::Quad<VertexPN>(    // -Y
            std::array<glm::vec3, 4>
            {
              unitCubeVectors[0],
              unitCubeVectors[1],
              unitCubeVectors[5],
              unitCubeVectors[4]
            }),
          mesh::Quad<VertexPN>(    // +Y
            std::array<glm::vec3, 4>
            {
              unitCubeVectors[2],
              unitCubeVectors[3],
              unitCubeVectors[7],
              unitCubeVectors[6]
            }),
          mesh::Quad<VertexPN>(    // -X
            std::array<glm::vec3, 4>
            {
              unitCubeVectors[2],
              unitCubeVectors[1],
              unitCubeVectors[0],
              unitCubeVectors[3]
            }),
          mesh::Quad<VertexPN>(    // +X
            std::array<glm::vec3, 4>
            {
              unitCubeVectors[7],
              unitCubeVectors[4],
              unitCubeVectors[5],
              unitCubeVectors[6]
            }),
        }
      {
        mShader.attachVertexShader(assets::ResourceLoader::loadShaderSource("skybox_env_map_default.vert"));
        mShader.attachFragmentShader(assets::ResourceLoader::loadShaderSource("skybox_env_map_default.frag"));
        mShader.link();
      }

      void Skybox::render()
      {
        glm::mat4 lookAtMatrix = mCamera.lookAt();

        // Remove translation components from the view matrix so that the skybox appears not to move
        lookAtMatrix[3][0] = 0.0f;
        lookAtMatrix[3][1] = 0.0f;
        lookAtMatrix[3][2] = 0.0f;

        mShader.use();
        mShader.setUniformMatrix4fv("view", glm::value_ptr(lookAtMatrix));
        mShader.setUniformMatrix4fv("projection", glm::value_ptr(mCamera.projection()));
        mShader.setUniform1i("environmentMap", 0);

        mCubeMap.bind();
      
        glDepthFunc(GL_LEQUAL);

        for (auto& quad : mQuads)
        {
          quad.render();
        }

        glDepthFunc(GL_LESS);
      }
    }
  }
}
