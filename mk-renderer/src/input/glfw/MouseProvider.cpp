#include "MouseProvider.h"

#include <GLFW/glfw3.h>

namespace mk
{
  namespace input
  {
    namespace glfw
    {
      MouseProvider::MouseProvider(GLFWwindow* window)
        : mWindow(window)
      {
      }

      glm::vec2 MouseProvider::getMousePosition()
      {
        double newMouseX;
        double newMouseY;

        glfwGetCursorPos(mWindow, &newMouseX, &newMouseY);

        return glm::vec2(static_cast<float>(newMouseX), static_cast<float>(newMouseY));
      }
    }
  }
}
