#ifndef SRC_SCENE_CAMERA_H_
#define SRC_SCENE_CAMERA_H_

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace mk
{
  namespace demofw
  {
    class IKeyboardProvider;
    class IMouseProvider;
  }

  namespace scene
  {
    /**
     * @interface ICamera
     */
    class ICamera
    {
    public:
      /**
       * @return Look at matrix. This matrix will be updated by {@link FPSCamera#update}.
       */
      virtual const glm::mat4& lookAt() const = 0;

      /**
       * @return Projection matrix. This matrix will be updated by {@link FPSCamera#update}.
       */
      virtual const glm::mat4& projection() const = 0;

      /**
       * @return The camera position in world coordinates.
       */
      virtual const glm::vec3& position() const = 0;

      /**
       * @return The direction to which the camera is looking. {@link FPSCamera#update} must be called before retrieving this value.
       */
      virtual const glm::vec3& target() const = 0;
    };

    /**
     * FPS style camera that is controlled with AWSD and mouse.
     */
    class FPSCamera : public ICamera
    {
    public:
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
        MouseFilter(demofw::IMouseProvider& mouseProvider);

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
        demofw::IMouseProvider& mMouseProvider;
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
        NullMouseFilter(demofw::IMouseProvider& mouseProvider);

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
        SmoothMouseFilter(demofw::IMouseProvider& mouseProvider);

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
        InertiaMouseFilter(demofw::IMouseProvider& mouseProvider);

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

    public:
      /**
       * @param keyboardProvider Provider of keyboard input information.
       * @param mouseFilter Provider of mouse input information.
       */
      FPSCamera(demofw::IKeyboardProvider& keyboardProvider, MouseFilter& mouseFilter);

      /**
       * @param keyboardProvider Provider of keyboard input information.
       * @param mouseFilter Provider of mouse input information.
       * @param position Initial position of the camera
       * @param target Initial target which the camera is looking at.
       */
      FPSCamera(demofw::IKeyboardProvider& keyboardProvider, MouseFilter& mouseFilter, const glm::vec3& position, glm::vec3 target);

      /**
       * @param translationSpeed New translation speed applied to move forwards, backwards and strafe.
       */
      void setTranslationSpeed(float translationSpeed);

      /**
       * @return Translation speed applied to move forwards, backwards and strafe.
       */
      float translationSpeed();

      /**
       * @param rotSpeed New rotation speed applied to mouse movement.
       */
      void setRotationSpeed(float rotSpeed);

      /**
       * @return Rotation speed applied to mouse movement.
       */
      float rotationSpeed();

      /**
       * @param rotationDeceleration New rotation deceleration applied to mouse movement.
       * @note Only used by some mouse filters. So depending on the {@link MouseFilter} used, it might not have any effect.
       */
      void setRotationDeceleration(float rotDeceleration);

      /**
       * @return Rotation deceleration applied to mouse movement.
       * @note Only used by some mouse filters. So depending on the {@link MouseFilter} used, it might not have any effect.
       */
      float rotationDeceleration();

      /**
       * @param fov New FOV expressed in degrees.
       */
      void setFov(float fov);

      /**
       * @return FOV expressed in degrees.
       */
      float fov();

      /**
       * @param fov New aspect ratio (i.e. screen width / screen height).
       */
      void setAspectRatio(float aspectRatio);

      /**
       * @return Aspect ratio (i.e. screen width / screen height).
       */
      float aspectRatio();

      /**
       * Using the input information from they mouse and keyboard, it updates the camera matrices.
       *
       * @param dtSecs Last frame duration in seconds.
       * @warning This method should only be called from the main thread.
       */
      void update(float dtSecs);

    public: // from ICamera
      /**
       * @return Look at matrix. This matrix will be updated by {@link FPSCamera#update}.
       */
      virtual const glm::mat4& lookAt() const;

      /**
       * @return Projection matrix. This matrix will be updated by {@link FPSCamera#update}.
       */
      virtual const glm::mat4& projection() const;

      /**
       * @return The camera position in world coordinates.
       */
      virtual const glm::vec3& position() const;

      /**
       * @return The direction to which the camera is looking. {@link FPSCamera#update} must be called before retrieving this value.
       */
      virtual const glm::vec3& target() const;

    private:
      demofw::IKeyboardProvider& mKeyboardProvider;
      MouseFilter& mMouseFilter;

      glm::mat4 mLookAt;
      glm::mat4 mProjection;

      glm::vec3 mPosition;
      glm::vec3 mTarget;

      float mPitch;
      float mYaw;

      float mTranslationSpeed;
      float mRotationSpeed;
      float mRotationDeceleration;

      float mFov;
      float mAspectRatio;
      float mZNear;
      float mZFar;

      void setTarget(glm::vec3 target);
    };
  }
}

#endif  // SRC_SCENE_CAMERA_H_
