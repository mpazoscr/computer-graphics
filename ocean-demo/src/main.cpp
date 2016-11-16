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

//#define PROCEDURAL_SKYBOX

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
      //mOcean(mRectPatch, kRectPatchX * kLengthScaleFactor, kRectPatchZ * kLengthScaleFactor),
#ifdef PROCEDURAL_SKYBOX
      mSkybox(600.0f),
      mSkyboxShader()
#else
      mEnvMap(mk::assets::ResourceLoader::loadEnvironmentMap("skybox_daylight", ".png")),
      mSkybox(600.0f, mEnvMap, mFpsCamera)
#endif
    {
      mOceanShader.attachVertexShader(mk::assets::ResourceLoader::loadShaderSource("ocean.vert"));
      mOceanShader.attachFragmentShader(mk::assets::ResourceLoader::loadShaderSource("ocean.frag"));
      mOceanShader.link();

    #ifdef PROCEDURAL_SKYBOX
      mSkyboxShader.attachVertexShader(mk::assets::ResourceLoader::loadShaderSource("skybox.vert"));
      mSkyboxShader.attachFragmentShader(mk::assets::ResourceLoader::loadShaderSource("skybox.frag"));
      mSkyboxShader.link();
    #endif

      glEnable(GL_DEPTH_TEST);

      setCustomKeyPressedCallback(keyPressedCallback);
    }

    virtual ~OceanDemo()
    {
#ifndef PROCEDURAL_SKYBOX
      mEnvMap.release();
#endif
    }

    virtual void update(double elapsedTime, double globalTime)
    {
      mFpsCamera.update(static_cast<float>(elapsedTime));
      //mOcean.update(static_cast<float>(globalTime));
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

  #ifdef PROCEDURAL_SKYBOX
      mSkyboxShader.use();
      mSkyboxShader.setUniformVector3fv("sunDir", glm::value_ptr(rotatedLightDir));
      mSkyboxShader.setUniformVector3fv("eyePos", glm::value_ptr(mFpsCamera.position()));
      mSkyboxShader.setUniformMatrix4fv("view", glm::value_ptr(mFpsCamera.lookAt()));
      mSkyboxShader.setUniformMatrix4fv("projection", glm::value_ptr(mFpsCamera.projection()));
  #endif

      mSkybox.render();
    }

  private:
    mk::scene::FPSCamera::SmoothMouseFilter mSmoothMouseFilter;
    mk::scene::FPSCamera mFpsCamera;
    mk::mesh::RectPatch<mk::core::VertexPN> mRectPatch;
    mk::gl::ShaderProgram mOceanShader;
    //mk::physics::Ocean mOcean;

#ifdef PROCEDURAL_SKYBOX
    mk::mesh::RectVolume<mk::core::VertexPN> mSkybox;
    mk::gl::ShaderProgram mSkyboxShader;
#else
    mk::image::EnvironmentMap mEnvMap;
    mk::scene::Skybox mSkybox;
#endif
  };
}

int main(void)
{
  OceanDemo oceanDemo(std::string("Ocean"), 800, 600);

  oceanDemo.hideMouseCursor();
  oceanDemo.doRenderLoop();

  return 0;
}
