#ifndef SRC_INPUT_GLFWMOUSEPROVIDER_H_
#define SRC_INPUT_GLFWMOUSEPROVIDER_H_

#include "input/IMouseProvider.h"

struct GLFWwindow;

namespace mk
{
  namespace input
  {
    namespace glfw
    {
      /**
       * Interface for a provider of information related to the mouse (i.e. cursor position, scroll position, button clicks)
       */
      class MouseProvider : public IMouseProvider
      {
      public:
        /**
         * @param window GLFW window handler.
         */
        MouseProvider(GLFWwindow* window);

      public:  // From IMouseProvider
        /**
         * Returns the current position of the mouse cursor in X/Y screen coordinates.
         *
         * The origin and domain of the coordinates depends on the underlying implementation.
         */
        virtual glm::vec2 getMousePosition();

      private:
        GLFWwindow* mWindow;
      };
    }
  }
}

#endif  // SRC_INPUT_GLFWMOUSEPROVIDER_H_
