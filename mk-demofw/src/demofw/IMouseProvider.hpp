#ifndef SRC_DEMOFW_IMOUSEPROVIDER_H_
#define SRC_DEMOFW_IMOUSEPROVIDER_H_

#include <memory>
#include <glm/vec2.hpp>

namespace mk
{
  namespace demofw
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
       * @return The current position of the mouse cursor in X/Y screen coordinates.
       *         The origin and domain of the coordinates depends on the underlying implementation.
       * @warning This method should only be called from the main thread.
       */
      virtual glm::ivec2 getMousePosition() = 0;

      /**
       * @return The current position of the mouse cursor in X/Y screen coordinates.
       *         The origin and domain of the coordinates depends on the underlying implementation.
       * @warning This method should only be called from the main thread.
       */
      virtual glm::vec2 getMousePositionF() = 0;

      /**
       * @return True if the left button is clicked, false otherwise.
       */
      virtual bool isLeftClicked() = 0;

      /**
       * @return True if the right button is clicked, false otherwise.
       */
      virtual bool isRightClicked() = 0;

      /**
       * @return True if the middle button is clicked, false otherwise.
       */
      virtual bool isMiddleClicked() = 0;
    };

    typedef std::shared_ptr<IMouseProvider> MouseProviderSharedPtr;
  }
}

#endif  // SRC_DEMOFW_IMOUSEPROVIDER_H_
