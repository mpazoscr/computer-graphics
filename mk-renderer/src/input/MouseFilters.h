#ifndef SRC_INPUT_MOUSEFILTERS_H_
#define SRC_INPUT_MOUSEFILTERS_H_

#include <glm/vec2.hpp>

namespace mk
{
  namespace input
  {
    class IMouseProvider;

    /**
     * Abstract class for mouse filters that apply a transformation to the mouse position signal.
     */
    class MouseFilter
    {
    public:
      /**
       * Initializes the internal state and filters the initial noise in the mouse position signal.
       *
       * @param mouseProvider Provider of mouse input information.
       */
      MouseFilter(IMouseProvider& mouseProvider);

      /**
       * Sets the factor that will be multiplied to the mouse displacement.
       *
       * The function {@link mouseDelta} will take this value into account when calculating the mouse
       * delta.
       *
       * @param mouseSpeedFactor Factor to be multiplied to the mouse displacement
       */
      void setMouseSpeedFactor(float mouseSpeedFactor);

      /**
       * @return The factor that will be multiplied to the mouse displacement when calculating the mouse
       * delta returned by {@link mouseDelta}.
       */
      float mouseSpeedFactor() const;

      /**
       * Sets the deceleration used to calculate the mouse position rate of change.
       *
       * @param mouseDeceleration Maximum mouse speed.
       *
       * @note Only used by some filters.
       */
      void setMouseDeceleration(float mouseDeceleration);

      /**
       * @return The deceleration used to calculate the mouse position rate of change.
       */
      float mouseDeceleration() const;

      /**
       * Calculates the mouse delta after applying the corresponding filter.
       *
       * @param dt Time step in seconds.
       * @return The mouse delta in pixels after applying the corresponding filter. This value will be such
       * that given a delta time it will never exceed the velocity specified by {@link setMouseSpeedFactor}.
       */
      virtual glm::vec2 mouseDelta(float dt) = 0;

    protected:
      IMouseProvider& mMouseProvider;
      glm::vec2 mLastMousePos;
      float mMouseSpeedFactor;
      float mMouseDeceleration;
    };

    /**
     * Mouse filter that applies no transformation to the values returned by the OS.
     */
    class NullMouseFilter : public MouseFilter
    {
    public:
      /**
       * @param mouseProvider Provider of mouse input information.
       */
      NullMouseFilter(IMouseProvider& mouseProvider);

    public:  // From MouseFilter
      /**
       * @param dt Time step in seconds.
       * @return The resulting mouse given the mouse coordinates given by the OS.
       */
      virtual glm::vec2 mouseDelta(float dt);
    };

    /**
     * Mouse filter that smoothens the mouse signal.
     *
     * In some systems the mouse signal can vary violently, causing a jittery movement.
     * This class applies a low pass filter to smoothen the signal.
     */
    class SmoothMouseFilter : public MouseFilter
    {
    public:
      /**
       * @param mouseProvider Provider of mouse input information.
       */
      SmoothMouseFilter(IMouseProvider& mouseProvider);

    public:  // From MouseFilter
      /**
       * @param dt Time step in seconds.
       * @return The resulting mouse delta in pixels after applying the low pass filter. This value will be such
       * that given a delta time it will never exceed the velocity specified by {@link setMouseSpeedFactor}.
       */
      virtual glm::vec2 mouseDelta(float dt);
    };

    /**
     * Mouse filter that simulates inertia given an acceleration.
     *
     * @note Makes use of {@link MouseFilter#mouseDeceleration}.
     */
    class InertiaMouseFilter : public MouseFilter
    {
    public:
      /**
       * @param mouseProvider Provider of mouse input information.
       */
      InertiaMouseFilter(IMouseProvider& mouseProvider);

    public:  // From MouseFilter
      /**
       * @param dt Time step in seconds.
       * @return The resulting mouse delta in pixels after applying the inertia. This value will be such
       * that given a delta time it will never exceed the velocity specified by {@link setMouseSpeedFactor}.
       */
      virtual glm::vec2 mouseDelta(float dt);

    private:
      glm::vec2 mInertiaDir;
      float mSpeed;
    };
  }
}

#endif  // SRC_INPUT_MOUSEFILTERS_H_
