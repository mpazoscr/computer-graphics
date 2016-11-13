#include "BaseDemoApp.hpp"

#include <string>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "demofw/IntervalTimer.hpp"
#include "KeyboardProvider.hpp"
#include "MouseProvider.hpp"

namespace mk
{
  namespace demofw
  {
    namespace glfw
    {
      std::function<void(int)> BaseDemoApp::mCustomKeyPressedCallback;

      void BaseDemoApp::setCustomKeyPressedCallback(std::function<void(int)> keyPressedCallback)
      {
        mCustomKeyPressedCallback = keyPressedCallback;
      }

      void BaseDemoApp::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
      {
        if (action == GLFW_PRESS)
        {
          if (key == GLFW_KEY_ESCAPE)
          {
            glfwSetWindowShouldClose(window, GL_TRUE);
          }

          // There is indeed a race condition on mCustomKeyPressedCallback, but it is consciouslly ignored (see doc).
          if (mCustomKeyPressedCallback)
          {
            mCustomKeyPressedCallback(key);
          }
        }
      }

      BaseDemoApp::BaseDemoApp(const std::string& title, int windowWidth, int windowHeight)
      : mWindow(nullptr),
        mKeyboardProvider(),
        mMouseProvider()
      {
        if (!glfwInit())
        {
          exit(-1);
        }

        const bool isFullScreen = ((0 == windowWidth) || (0 == windowHeight));

        const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());

        if (isFullScreen)
        {
          windowWidth = videoMode->width;
          windowHeight = videoMode->height;
        }

        glfwWindowHint(GLFW_VISIBLE, 0);

        mWindow = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), isFullScreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
        if (!mWindow)
        {
          glfwTerminate();
          exit(-1);
        }

        glfwSetWindowPos(mWindow, (videoMode->width - windowWidth) / 2, (videoMode->height - windowHeight) / 2);
        glfwShowWindow(mWindow);

        glfwMakeContextCurrent(mWindow);
        glfwSwapInterval(1);

        GLenum err = glewInit();
        if (GLEW_OK != err)
        {
          std::cout << "Error: " << glewGetErrorString(err) << std::endl;
          exit(-1);
        }

        glfwSetKeyCallback(mWindow, keyCallback);

        mKeyboardProvider.reset(new KeyboardProvider(mWindow));
        mMouseProvider.reset(new MouseProvider(mWindow));
      }

      BaseDemoApp::~BaseDemoApp()
      {
        glfwDestroyWindow(mWindow);
        glfwTerminate();

        mWindow = nullptr;
      }

      IKeyboardProvider& BaseDemoApp::getKeyboardProvider() const
      {
        return *mKeyboardProvider;
      }

      IMouseProvider& BaseDemoApp::getMouseProvider() const
      {
        return *mMouseProvider;
      }

      void BaseDemoApp::showMouseCursor()
      {
        glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      }

      void BaseDemoApp::hideMouseCursor()
      {
        glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      }

      void BaseDemoApp::doRenderLoop(double framerate)
      {
        const double targetFrameTime = (framerate <= 0.0) ? 0.0 : (1.0 / framerate);

        IntervalTimer intervalTimer;

        double elapsedTotalTime = 0.0;
        double elapsedFrameTime = 0.0;

        while (!glfwWindowShouldClose(mWindow))
        {
          elapsedFrameTime += intervalTimer.tick();

          if (elapsedFrameTime >= targetFrameTime)
          {
            elapsedTotalTime += elapsedFrameTime;
            update(elapsedFrameTime * 1e-9, elapsedTotalTime * 1e-9);

            elapsedFrameTime = 0;
            intervalTimer.tick();

            render();
            glfwSwapBuffers(mWindow);
          }

          glfwPollEvents();
        }
      }
    }
  }
}
