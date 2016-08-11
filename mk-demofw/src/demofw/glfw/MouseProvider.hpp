#ifndef SRC_DEMOFW_GLFW_MOUSEPROVIDER_H_
#define SRC_DEMOFW_GLFW_MOUSEPROVIDER_H_

#include "demofw/IMouseProvider.hpp"

struct GLFWwindow;

namespace mk
{
  namespace demofw
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
         * @return The current position of the mouse cursor in X/Y screen coordinates.
         *         The origin and domain of the coordinates depends on the underlying implementation.
         * @warning This method should only be called from the main thread.
         */
        virtual glm::ivec2 getMousePosition();

        /**
         * @return The current position of the mouse cursor in X/Y screen coordinates.
         *         The origin and domain of the coordinates depends on the underlying implementation.
         * @warning This method should only be called from the main thread.
         */
        virtual glm::vec2 MouseProvider::getMousePositionF();

        /**
         * @return True if the left button is clicked, false otherwise.
         */
        virtual bool isLeftClicked();

        /**
         * @return True if the right button is clicked, false otherwise.
         */
        virtual bool isRightClicked();

        /**
         * @return True if the middle button is clicked, false otherwise.
         */
        virtual bool isMiddleClicked();

      private:
        GLFWwindow* mWindow;
      };
    }
  }
}

#endif  // SRC_DEMOFW_GLFW_MOUSEPROVIDER_H_
