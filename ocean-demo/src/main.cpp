#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "assets/ResourceLoader.hpp"
#include "core/VertexTypes.hpp"
#include "gl/ShaderProgram.hpp"
#include "image/EnvironmentMap.hpp"
#include "demofw/glfw/KeyboardProvider.hpp"
#include "demofw/glfw/MouseProvider.hpp"
#include "mesh/RectPatch.hpp"
#include "mesh/RectVolume.hpp"
#include "physics/waves/Ocean.hpp"
#include "scene/Camera.hpp"
#include "scene/Skybox.hpp"
#include "demofw/glfw/BaseDemoApp.hpp"

namespace
{
  const int kRectPatchX = 256;
  const int kRectPatchZ = 256;
  const float kLengthScaleFactor = 0.7f;

  void keyPressedCallback(int key)
  {
    if ('1' == key)
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
  }

  class OceanDemo : public mk::demofw::glfw::BaseDemoApp
  {
  public:
    OceanDemo(const std::string& title, unsigned int windowWidth, unsigned int windowHeight)
    : mk::demofw::glfw::BaseDemoApp(title, windowWidth, windowHeight),
      mSmoothMouseFilter(getMouseProvider()),
      mFpsCamera(getKeyboardProvider(), mSmoothMouseFilter, glm::vec3(0.0f, 60.0f, 0.0f), glm::vec3(0.0f, -5.0f, -5.0f)),
      mRectPatch(kRectPatchX, kRectPatchZ),
      mOceanShader(),
      mOcean(mRectPatch, kRectPatchX * kLengthScaleFactor, kRectPatchZ * kLengthScaleFactor),
      mEnvMap(mk::assets::ResourceLoader::loadEnvironmentMap("skybox_daylight", ".png")),
      mSkybox(mEnvMap, mFpsCamera)
    {
      mOceanShader.attachVertexShader(mk::assets::ResourceLoader::loadShaderSource("ocean.vert"));
      mOceanShader.attachFragmentShader(mk::assets::ResourceLoader::loadShaderSource("ocean.frag"));
      mOceanShader.link();

      glEnable(GL_DEPTH_TEST);

      setCustomKeyPressedCallback(keyPressedCallback);
    }

    virtual ~OceanDemo()
    {
      mEnvMap.release();
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
    mk::scene::FPSCamera::SmoothMouseFilter mSmoothMouseFilter;
    mk::scene::FPSCamera mFpsCamera;
    mk::mesh::RectPatch<mk::core::VertexPN> mRectPatch;
    mk::gl::ShaderProgram mOceanShader;
    mk::physics::Ocean mOcean;
    mk::image::EnvironmentMap mEnvMap;
    mk::scene::Skybox mSkybox;
  };
}

int main(void)
{
  OceanDemo oceanDemo(std::string("Ocean"), 800, 600);

  oceanDemo.hideMouseCursor();
  oceanDemo.doRenderLoop();

  return 0;
}
