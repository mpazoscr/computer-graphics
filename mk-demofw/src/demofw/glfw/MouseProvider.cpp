#include "MouseProvider.hpp"

#include <GLFW/glfw3.h>

namespace mk
{
  namespace demofw
  {
    namespace glfw
    {
      MouseProvider::MouseProvider(GLFWwindow* window)
      : mWindow(window)
      {
      }

      glm::ivec2 MouseProvider::getMousePosition()
      {
        double newMouseX;
        double newMouseY;

        glfwGetCursorPos(mWindow, &newMouseX, &newMouseY);

        return glm::vec2(static_cast<int>(newMouseX), static_cast<int>(newMouseY));
      }

      glm::vec2 MouseProvider::getMousePositionF()
      {
        double newMouseX;
        double newMouseY;

        glfwGetCursorPos(mWindow, &newMouseX, &newMouseY);

        return glm::vec2(static_cast<float>(newMouseX), static_cast<float>(newMouseY));
      }

      bool MouseProvider::isLeftClicked()
      {
        const int buttonState = glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_LEFT);
        return ((GLFW_PRESS == buttonState) || (GLFW_REPEAT == buttonState));
      }

      bool MouseProvider::isRightClicked()
      {
        const int buttonState = glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_RIGHT);
        return ((GLFW_PRESS == buttonState) || (GLFW_REPEAT == buttonState));
      }

      bool MouseProvider::isMiddleClicked()
      {
        const int buttonState = glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_MIDDLE);
        return ((GLFW_PRESS == buttonState) || (GLFW_REPEAT == buttonState));
      }
    }
  }
}
