#ifndef SRC_SCENE_CAMERA_H_
#define SRC_SCENE_CAMERA_H_

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace mk
{
  namespace input
  {
    class MouseFilter;
    class IKeyboardProvider;
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
       * @param keyboardProvider Provider of keyboard input information.
       * @param mouseFilter Provider of mouse input information.
       */
      FPSCamera(input::IKeyboardProvider& keyboardProvider, input::MouseFilter& mouseFilter);

      /**
       * @param keyboardProvider Provider of keyboard input information.
       * @param mouseFilter Provider of mouse input information.
       * @param position Initial position of the camera
       * @param target Initial target which the camera is looking at.
       */
      FPSCamera(input::IKeyboardProvider& keyboardProvider, input::MouseFilter& mouseFilter, const glm::vec3& position, glm::vec3 target);

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
       * @param dtNanoSecs Last frame duration in nano seconds.
       */
      void update(float dtNanoSecs);

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
      input::IKeyboardProvider& mKeyboardProvider;
      input::MouseFilter& mMouseFilter;

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
