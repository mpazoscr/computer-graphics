#include "KeyboardProvider.h"

#include <GLFW/glfw3.h>

namespace mk
{
  namespace input
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
