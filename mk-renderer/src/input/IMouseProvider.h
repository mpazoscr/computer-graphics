#ifndef SRC_INPUT_IMOUSEPROVIDER_H_
#define SRC_INPUT_IMOUSEPROVIDER_H_

#include <glm/vec2.hpp>

namespace mk
{
  namespace input
  {
    /**
     * Interface for a provider of information related to the mouse (i.e. cursor position, scroll position, button clicks)
     */
    class IMouseProvider
    {
    public:
      /**
       * Just the virtual destructor you would expect in an interface.
       */
      virtual ~IMouseProvider() {}

      /**
       * Returns the current position of the mouse cursor in X/Y screen coordinates.
       *
       * The origin and domain of the coordinates depends on the underlying implementation.
       */
      virtual glm::vec2 getMousePosition() = 0;
    };
  }
}

#endif  // SRC_INPUT_IMOUSEPROVIDER_H_
