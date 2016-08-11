#include "KeyboardProvider.hpp"

#include <GLFW/glfw3.h>

namespace mk
{
  namespace demofw
  {
    namespace glfw
    {
      KeyboardProvider::KeyboardProvider(GLFWwindow* window)
      : mWindow(window)
      {
      }

      int KeyboardProvider::isKeyPressed(char key)
      {
        return glfwGetKey(mWindow, static_cast<int>(key)) & GLFW_PRESS;
      }
    }
  }
}
