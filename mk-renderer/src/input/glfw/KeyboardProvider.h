#ifndef SRC_INPUT_GLFWKEYBOARDPROVIDER_H_
#define SRC_INPUT_GLFWKEYBOARDPROVIDER_H_

#include "input/IKeyboardProvider.h"

struct GLFWwindow;

namespace mk
{
  namespace input
  {
    namespace glfw
    {
      /**
       * Interface for a provider of information related to the keyboard.
       */
      class KeyboardProvider : public IKeyboardProvider
      {
      public:
        /**
         * @param window GLFW window handler.
         */
        KeyboardProvider(GLFWwindow* window);

      public:  // From IKeyboardProvider
        /**
         * @param key Key identified by its ASCII code.
         * @return 0 if the key is not pressed, 1 if it is.
         */
        virtual int isKeyPressed(char key);

      private:
        GLFWwindow* mWindow;
      };
    }
  }
}

#endif  // SRC_INPUT_GLFWKEYBOARDPROVIDER_H_
