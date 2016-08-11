#include "Skybox.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_access.hpp>

#include "assets/ResourceLoader.hpp"

namespace mk
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

    Skybox::Skybox(float size, image::EnvironmentMap envMap, const scene::ICamera& camera)
    : mEnvMap(envMap),
      mCamera(camera),
      mShader(),
      mQuads
      {
        mesh::Quad<core::VertexPN>(    // -Z
          std::array<glm::vec3, 4>
          {
            unitCubeVectors[3] * size,
            unitCubeVectors[0] * size,
            unitCubeVectors[4] * size,
            unitCubeVectors[7] * size
          }),
        mesh::Quad<core::VertexPN>(    // +Z
          std::array<glm::vec3, 4>
          {
            unitCubeVectors[6] * size,
            unitCubeVectors[5] * size,
            unitCubeVectors[1] * size,
            unitCubeVectors[2] * size
          }),
        mesh::Quad<core::VertexPN>(    // -Y
          std::array<glm::vec3, 4>
          {
            unitCubeVectors[0] * size,
            unitCubeVectors[1] * size,
            unitCubeVectors[5] * size,
            unitCubeVectors[4] * size
          }),
        mesh::Quad<core::VertexPN>(    // +Y
          std::array<glm::vec3, 4>
          {
            unitCubeVectors[2] * size,
            unitCubeVectors[3] * size,
            unitCubeVectors[7] * size,
            unitCubeVectors[6] * size
          }),
        mesh::Quad<core::VertexPN>(    // -X
          std::array<glm::vec3, 4>
          {
            unitCubeVectors[2] * size,
            unitCubeVectors[1] * size,
            unitCubeVectors[0] * size,
            unitCubeVectors[3] * size
          }),
        mesh::Quad<core::VertexPN>(    // +X
          std::array<glm::vec3, 4>
          {
            unitCubeVectors[7] * size,
            unitCubeVectors[4] * size,
            unitCubeVectors[5] * size,
            unitCubeVectors[6] * size
          }),
      }
    {
      mShader.attachVertexShader(mk::assets::ResourceLoader::loadShaderSource("skybox_env.vert"));
      mShader.attachFragmentShader(mk::assets::ResourceLoader::loadShaderSource("skybox_env.frag"));
      mShader.link();
    }

    void Skybox::render()
    {
      glm::mat4 lookAtMatrix = mCamera.lookAt();

      lookAtMatrix[3][0] = 0.0f;
      lookAtMatrix[3][1] = 0.0f;
      lookAtMatrix[3][2] = 0.0f;

      mShader.use();
      mShader.setUniformMatrix4fv("view", glm::value_ptr(lookAtMatrix));
      mShader.setUniformMatrix4fv("projection", glm::value_ptr(mCamera.projection()));
      mShader.setUniform1i("environmentMap", 0);

      mEnvMap.bind();

      for (auto& quad : mQuads)
      {
        quad.render();
      }
    }
  }
}
