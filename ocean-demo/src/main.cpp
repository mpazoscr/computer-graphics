#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "renderer/assets/ResourceLoader.hpp"
#include "renderer/VertexTypes.hpp"
#include "renderer/gl/ShaderProgram.hpp"
#include "renderer/gl/CubeMap.hpp"
#include "demofw/glfw/KeyboardProvider.hpp"
#include "demofw/glfw/MouseProvider.hpp"
#include "renderer/mesh/RectPatch.hpp"
#include "physics/waves/Ocean.hpp"
#include "renderer/scene/Camera.hpp"
#include "renderer/scene/Skybox.hpp"
#include "demofw/glfw/BaseDemoApp.hpp"

using namespace mk;

namespace
{
  const float kLengthScaleFactor = 0.7f;

  const glm::uvec2 kOceanSize(256, 256);
  const glm::vec2 kOceanLength(kOceanSize.x * kLengthScaleFactor, kOceanSize.y * kLengthScaleFactor);
  const glm::ivec2 kRectPatchSize(512, 512);

  void keyPressedCallback(int key)
  {
    if ('1' == key)
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
  }

  class OceanDemo : public demofw::glfw::BaseDemoApp
  {
  public:
    OceanDemo(const std::string& title, unsigned int windowWidth, unsigned int windowHeight)
    : demofw::glfw::BaseDemoApp(title, windowWidth, windowHeight),
      mSmoothMouseFilter(getMouseProvider()),
      mFpsCamera(getKeyboardProvider(), mSmoothMouseFilter, glm::vec3(0.0f, 60.0f, 0.0f), glm::vec3(0.0f, -5.0f, -5.0f)),
      mRectPatch(kRectPatchSize.x, kRectPatchSize.y),
      mOceanShader(),
      mOcean(mRectPatch, kOceanSize, kOceanLength),
      mCubeMap(renderer::assets::ResourceLoader::loadCubeMap("skybox_daylight", ".png")),
      mSkybox(mCubeMap, mFpsCamera)
    {
      mOceanShader.attachVertexShader(renderer::assets::ResourceLoader::loadShaderSource("ocean.vert"));
      mOceanShader.attachFragmentShader(renderer::assets::ResourceLoader::loadShaderSource("ocean.frag"));
      mOceanShader.link();

      glEnable(GL_DEPTH_TEST);

      setCustomKeyPressedCallback(keyPressedCallback);
    }

    virtual void update(double elapsedTime, double globalTime)
    {
      mFpsCamera.update(static_cast<float>(elapsedTime));
      mOcean.update(static_cast<float>(globalTime));
    }

    virtual void render()
    {
      const glm::vec3 lightDir(0.0f, 1.0f, 0.0f);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // Define light direction

      glm::vec3 rotatedLightDir = glm::rotateX(lightDir, glm::radians(75.0f));
      rotatedLightDir = glm::rotateY(rotatedLightDir, glm::radians(75.0f));

      // Render

      mCubeMap.bind();

      mOceanShader.use();
      mOceanShader.setUniformVector3fv("lightDir", glm::value_ptr(rotatedLightDir));
      mOceanShader.setUniformVector3fv("eyePos", glm::value_ptr(mFpsCamera.position()));
      mOceanShader.setUniformMatrix4fv("view", glm::value_ptr(mFpsCamera.lookAt()));
      mOceanShader.setUniformMatrix4fv("projection", glm::value_ptr(mFpsCamera.projection()));
      mOceanShader.setUniform1i("environmentMap", 0);

      mRectPatch.render();

      // Render skybox

      mSkybox.render();
    }

  private:
    renderer::scene::FPSCamera::SmoothMouseFilter mSmoothMouseFilter;
    renderer::scene::FPSCamera mFpsCamera;
    renderer::mesh::RectPatch<renderer::VertexPN> mRectPatch;
    renderer::gl::ShaderProgram mOceanShader;
    physics::Ocean mOcean;
    renderer::gl::CubeMap mCubeMap;
    renderer::scene::Skybox mSkybox;
  };
}

int main(void)
{
  OceanDemo oceanDemo(std::string("Ocean"), 800, 600);

  oceanDemo.hideMouseCursor();
  oceanDemo.doRenderLoop();

  return 0;
}
