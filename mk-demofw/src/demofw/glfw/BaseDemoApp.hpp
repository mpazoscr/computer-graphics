#ifndef SRC_DEMOFW_GLFW_BASEDEMOAPP_H_
#define SRC_DEMOFW_GLFW_BASEDEMOAPP_H_

#include <functional>

#include "demofw/IKeyboardProvider.hpp"
#include "demofw/IMouseProvider.hpp"

struct GLFWwindow;

namespace mk
{
  namespace demofw
  {
    namespace glfw
    {
      class BaseDemoApp
      {
      public:
        /**
         * Sets a custom callback to be called when a key is pressed.
         *
         * @param keyPressedCallback Callback to be called. It accepts one integer parameter which is the ASCII code of the pressed key.
         * @warning This function needs to be called before the class is instantiated, to avoid a race condition.
         */
        static void setCustomKeyPressedCallback(std::function<void(int)> keyPressedCallback);

      private:
        /**
         * Callback function for key pressed event as required by GLFW.
         * @param window GLFW window.
         * @param key The keyboard key that was pressed or released.
         * @param scancode The system-specific scancode of the key.
         * @param action GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT.
         * @param mods 	Bit field describing which modifier keys were held down.
         */
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

      public:
        /**
         * Creates a GLFW window with the given height and width.
         *
         * @param title Window's title
         * @param windowHeight Window height
         * @param windowWidth Window width
         * @note If either windowHeight or windowWidth are 0, the window will be set as fullscreen.
         * @warning This class should only be instantiated from the main thread.
         */
        BaseDemoApp(const std::string& title, int windowWidth, int windowHeight);

        /**
         * Shuts down the GLFW window.
         */
        virtual ~BaseDemoApp();

        /**
         * @return Keyboard provider associated with this application.
         * @warning The returned reference should only be used for as long as this instance of BaseDemoApp is alive.
         *          Using it afterwards will casue undefined behaviour.
         */
        IKeyboardProvider& getKeyboardProvider() const;

        /**
         * @return Mouse provider associated with this application.
         * @warning The returned reference should only be used for as long as this instance of BaseDemoApp is alive.
         *          Using it afterwards will casue undefined behaviour.
         */
        IMouseProvider& getMouseProvider() const;

        /**
         * Makes the mouse cursor visible (it is visible by default).
         */
        void showMouseCursor();

        /**
         * Makes the mouse cursor invisible.
         */
        void hideMouseCursor();

        /**
         * Executes the render loop until the application is stopped by pressing Escape.
         *
         * @param framerate Desired framerate limit. If <= 0, there is no framerate limit.
         * @note if framerate is > 0.0, both update and render will be called at a frequency
         * as close to framerate as possible.
         */
        void doRenderLoop(double framerate = 0.0);

        /**
         * Function that moves forward the app by an elapsed time.
         *
         * @param elapsedTime Time elapsed in seconds since last update.
         * @param globalTime Time elapsed in seconds since first update.
         */
        virtual void update(double elapsedTime, double globalTime) = 0;

        /**
         * Render function.
         */
        virtual void render() = 0;

      private:
        static std::function<void(int)> mCustomKeyPressedCallback;

      private:
        GLFWwindow* mWindow;
        KeyboardProviderSharedPtr mKeyboardProvider;
        MouseProviderSharedPtr mMouseProvider;
      };
    }
  }
}

#endif  // SRC_DEMOFW_GLFW_BASEDEMOAPP_H_
