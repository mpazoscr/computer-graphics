#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "assets/ResourceLoader.hpp"
#include "core/VertexTypes.h"
#include "gl/ShaderProgram.h"
#include "cuda/DeviceCaps.h"
#include "image/EnvironmentMap.hpp"
#include "input/glfw/KeyboardProvider.h"
#include "input/glfw/MouseProvider.h"
#include "input/MouseFilters.h"
#include "mesh/RectPatch.h"
#include "mesh/RectVolume.h"
#include "physics/waves/Ocean.h"
#include "scene/Camera.h"
#include "scene/Skybox.h"
#include "utils/IntervalTimer.h"

//#define PROCEDURAL_SKYBOX

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (action == GLFW_PRESS)
  {
    if (key == GLFW_KEY_ESCAPE)
    {
      glfwSetWindowShouldClose(window, GL_TRUE);
      return;
    }

    if (key == GLFW_KEY_1)
    {
      glPolygonMode(GL_FRONT_AND_BACK, mods & GLFW_MOD_SHIFT ? GL_FILL : GL_LINE);
    }
  }
}

int main(void)
{
  if (!glfwInit())
  {
      return -1;
  }

  const int WINDOW_SIZE_X = 0;
  const int WINDOW_SIZE_Y = 0;
  const bool isFullScreen = (0 == WINDOW_SIZE_X || 0 == WINDOW_SIZE_Y);

  const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  const int windowWidth = isFullScreen ? videoMode->width : WINDOW_SIZE_X;
  const int windowHeight = isFullScreen ? videoMode->height : WINDOW_SIZE_Y;

  glfwWindowHint(GLFW_VISIBLE, 0);

  GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Ocean", isFullScreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
  if (!window)
  {
      glfwTerminate();
      return -1;
  }

  glfwSetWindowPos(window, (videoMode->width - WINDOW_SIZE_X) / 2, (videoMode->height - WINDOW_SIZE_Y) / 2);
  glfwShowWindow(window);

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  glfwSetKeyCallback(window, keyCallback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    std::cout << "Error: " << glewGetErrorString(err) << std::endl;
    return -1;
  }

  if (!mk::cuda::setDevice(0))
  {
    std::cout << "Error setting CUDA device." << std::endl;
    return -1;
  }

  const int rectPatchX = 256;
  const int rectPatchZ = 256;
  const float lengthScaleFactor = 0.7f;

  mk::mesh::RectPatch<mk::core::VertexPN> rectPatch(rectPatchX, rectPatchZ);
  mk::physics::Ocean ocean(rectPatch, rectPatchX * lengthScaleFactor, rectPatchZ * lengthScaleFactor);

  mk::input::glfw::KeyboardProvider keyboardProvider(window);
  mk::input::glfw::MouseProvider mouseProvider(window);
  mk::input::SmoothMouseFilter smoothMouseFilter(mouseProvider);
  mk::scene::FPSCamera fpsCamera(keyboardProvider, smoothMouseFilter, glm::vec3(0.0f, 60.0f, 0.0f), glm::vec3(0.0f, -5.0f, -5.0f));
  fpsCamera.setTranslationSpeed(20.0f);

  mk::assets::ResourceLoader::Initialize();

  mk::gl::ShaderProgram oceanShader;

  oceanShader.attachVertexShader(mk::assets::ResourceLoader::loadShaderSource("ocean.vert"));
  oceanShader.attachFragmentShader(mk::assets::ResourceLoader::loadShaderSource("ocean.frag"));
  oceanShader.link();

#ifdef PROCEDURAL_SKYBOX
   mk::mesh::RectVolume<mk::core::VertexPN> skybox(600.0f);
   mk::gl::ShaderProgram skyboxShader;

  skyboxShader.attachVertexShader(mk::assets::ResourceLoader::loadShaderSource("skybox.vert"));
  skyboxShader.attachFragmentShader(mk::assets::ResourceLoader::loadShaderSource("skybox.frag"));
  skyboxShader.link();
#else
  mk::image::EnvironmentMap envMap = mk::assets::ResourceLoader::loadEnvironmentMap("skybox_daylight", ".png");
  mk::scene::Skybox skybox(600.0f, envMap, fpsCamera);
#endif

  glEnable(GL_DEPTH_TEST);

  glm::vec3 lightDir(0.0f, 1.0f, 0.0f);

  mk::utils::IntervalTimer intervalTimer;

  float t = 0.0;

  while (!glfwWindowShouldClose(window))
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Camera Update

    float elapsedNano = intervalTimer.elapsedNano();

    fpsCamera.update(elapsedNano);
    ocean.update(t * 1e-9f);

    // Define light direction

    glm::vec3 rotatedLightDir = glm::rotateX(lightDir, glm::radians(75.0f));
    rotatedLightDir = glm::rotateY(rotatedLightDir, glm::radians(75.0f));

    // Render

    oceanShader.use();
    oceanShader.setUniformVector3fv("lightDir", glm::value_ptr(rotatedLightDir));
    oceanShader.setUniformVector3fv("eyePos", glm::value_ptr(fpsCamera.position()));
    oceanShader.setUniformMatrix4fv("view", glm::value_ptr(fpsCamera.lookAt()));
    oceanShader.setUniformMatrix4fv("projection", glm::value_ptr(fpsCamera.projection()));
    oceanShader.setUniform1i("environmentMap", 0);

#ifndef PROCEDURAL_SKYBOX
    envMap.bind();
#endif

    rectPatch.render();

#ifdef PROCEDURAL_SKYBOX
    skyboxShader.use();
    skyboxShader.setUniformVector3fv("sunDir", glm::value_ptr(rotatedLightDir));
    skyboxShader.setUniformVector3fv("eyePos", glm::value_ptr(fpsCamera.position()));
    skyboxShader.setUniformMatrix4fv("view", glm::value_ptr(fpsCamera.lookAt()));
    skyboxShader.setUniformMatrix4fv("projection", glm::value_ptr(fpsCamera.projection()));
#endif

    skybox.render();

    glfwSwapBuffers(window);
    glfwPollEvents();

    t += elapsedNano;
  }

#ifndef PROCEDURAL_SKYBOX
  envMap.release();
#endif

  glfwTerminate();

  return 0;
}
