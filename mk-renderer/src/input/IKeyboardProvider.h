#ifndef SRC_INPUT_IKEYBOARDPROVIDER_H_
#define SRC_INPUT_IKEYBOARDPROVIDER_H_

namespace mk
{
  namespace input
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
       */
      virtual int isKeyPressed(char key) = 0;
    };
  }
}

#endif  // SRC_INPUT_IKEYBOARDPROVIDER_H_
