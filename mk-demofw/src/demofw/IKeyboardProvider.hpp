#ifndef SRC_DEMOFW_IKEYBOARDPROVIDER_H_
#define SRC_DEMOFW_IKEYBOARDPROVIDER_H_

#include <memory>

namespace mk
{
  namespace demofw
  {
    /**
     * Interface for a provider of information related to the keyboard.
     */
    class IKeyboardProvider
    {
    public:
      /**
       * Just the virtual destructor you would expect in an interface.
       */
      virtual ~IKeyboardProvider() {}

      /**
       * @param key Key identified by its ASCII code.
       * @return 0 if the key is not pressed, 1 if it is.
       * @warning This method should only be called from the main thread.
       */
      virtual int isKeyPressed(char key) = 0;
    };

    typedef std::shared_ptr<IKeyboardProvider> KeyboardProviderSharedPtr;
  }
}

#endif  // SRC_DEMOFW_IKEYBOARDPROVIDER_H_
