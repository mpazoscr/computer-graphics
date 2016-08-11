#ifndef SRC_DEMOFW_GLFW_KEYBOARDPROVIDER_H_
#define SRC_DEMOFW_GLFW_KEYBOARDPROVIDER_H_

#include "demofw/IKeyboardProvider.hpp"

struct GLFWwindow;

namespace mk
{
  namespace demofw
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
         * @warning This method should only be called from the main thread.
         */
        virtual int isKeyPressed(char key);

      private:
        GLFWwindow* mWindow;
      };
    }
  }
}

#endif  // SRC_DEMOFW_GLFW_KEYBOARDPROVIDER_H_
