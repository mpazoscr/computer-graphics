#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/epsilon.hpp>

#include "input/IKeyboardProvider.h"
#include "input/MouseFilters.h"
#include "math/Utils.h"

namespace mk
{
  namespace scene
  {
    namespace
    {
      const float kDefaultTranslationSpeed = 2.0f;
      const float kDefaultRotationSpeed = 90.0f;
      const float kDefaultRotationDeceleration = 500.0f;
      const float kDefaultFov = 45.0f;
      const float kDefaultAspectRatio = 1.0f;
      const float kDefaultZNear = 1.0f;
      const float kDefaultZFar = 1000.0f;
    }

    FPSCamera::FPSCamera(input::IKeyboardProvider& keyboardProvider, input::MouseFilter& mouseFilter)
      : FPSCamera(keyboardProvider, mouseFilter, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f))
    {
    }

    FPSCamera::FPSCamera(input::IKeyboardProvider& keyboardProvider, input::MouseFilter& mouseFilter, const glm::vec3& position, glm::vec3 target)
      : mKeyboardProvider(keyboardProvider),
        mMouseFilter(mouseFilter),
        mPosition(position),
        mTarget(),
        mPitch(0.0f),
        mYaw(0.0f),
        mTranslationSpeed(kDefaultTranslationSpeed),
        mRotationSpeed(kDefaultRotationSpeed),
        mRotationDeceleration(kDefaultRotationDeceleration),
        mFov(kDefaultFov),
        mAspectRatio(kDefaultAspectRatio),
        mZNear(kDefaultZNear),
        mZFar(kDefaultZFar)
    {
      setTarget(target);

      setRotationSpeed(kDefaultRotationSpeed);
      setRotationDeceleration(kDefaultRotationDeceleration);
    }

    void FPSCamera::setTranslationSpeed(float translationSpeed)
    {
      mTranslationSpeed = translationSpeed;
    }

    float FPSCamera::translationSpeed()
    {
      return mTranslationSpeed;
    }

    void FPSCamera::setRotationSpeed(float rotSpeed)
    {
      mRotationSpeed = rotSpeed;
      mMouseFilter.setMouseSpeedFactor(rotationSpeed());
    }

    float FPSCamera::rotationSpeed()
    {
      return mRotationSpeed;
    }

    void FPSCamera::setRotationDeceleration(float rotDeceleration)
    {
      mRotationDeceleration = rotDeceleration;
      mMouseFilter.setMouseDeceleration(rotationDeceleration());
    }

    float FPSCamera::rotationDeceleration()
    {
      return mRotationDeceleration;
    }

    void FPSCamera::setFov(float fov)
    {
      mFov = fov;
    }

    float FPSCamera::fov()
    {
      return mFov;
    }

    void FPSCamera::setAspectRatio(float aspectRatio)
    {
      mAspectRatio = aspectRatio;
    }

    float FPSCamera::aspectRatio()
    {
      return mAspectRatio;
    }

    const glm::mat4& FPSCamera::lookAt() const
    {
      return mLookAt;
    }

    const glm::mat4& FPSCamera::projection() const
    {
      return mProjection;
    }

    const glm::vec3& FPSCamera::position() const
    {
      return mPosition;
    }

    const glm::vec3& FPSCamera::target() const
    {
      return mTarget;
    }

    void FPSCamera::update(float dtNanoSecs)
    {
      const float dtSecs = dtNanoSecs * 1e-9f;

      float move_dir = static_cast<float>(mKeyboardProvider.isKeyPressed('W') - mKeyboardProvider.isKeyPressed('S'));
      float strafe_dir = static_cast<float>(mKeyboardProvider.isKeyPressed('D') - mKeyboardProvider.isKeyPressed('A'));

      glm::vec2 mousePosDelta = mMouseFilter.mouseDelta(dtSecs);

      mPitch = glm::clamp(mPitch + mousePosDelta.y, -90.0f, 90.0f);
      mYaw = glm::mod(mYaw + mousePosDelta.x, 360.0f);

      glm::mat4 orientation;

      orientation = glm::rotate(orientation, glm::radians(mYaw), glm::vec3(0.0f, 1.0f, 0.0f));
      orientation = glm::rotate(orientation, glm::radians(mPitch), glm::vec3(1.0f, 0.0f, 0.0f));

      glm::vec4 target4(0.0f, 0.0f, -1.0f, 1.0f);
      glm::vec4 up4(0.0f, 1.0f, 0.0f, 1.0f);

      target4 = orientation * target4;
      up4 = orientation * up4;

      mTarget = glm::vec3(target4);
      glm::vec3 up = glm::vec3(up4);
      glm::vec3 strafe = glm::cross(mTarget, up);

      mPosition += mTarget * move_dir * mTranslationSpeed * dtSecs;
      mPosition += strafe * strafe_dir * mTranslationSpeed * dtSecs;

      mLookAt = glm::lookAt(mPosition, mPosition + mTarget, up);
      mProjection = glm::perspective(mFov, mAspectRatio, mZNear, mZFar);
    }

    void FPSCamera::setTarget(glm::vec3 target)
    {
      float targetLength = glm::length(target);

      if (!glm::epsilonEqual(targetLength, 0.0f, mk::math::kFloatEpsilon))
      {
        glm::vec3 projXZ = glm::vec3(target.x, 0.0f, target.z);
        glm::vec3 projYZ = glm::vec3(0.0f, target.y, target.z);

        float projXZLength = glm::length(projXZ);
        float projYZLength = glm::length(projYZ);

        target *= (1.0f / targetLength);

        if (!glm::epsilonEqual(projXZLength, 0.0f, mk::math::kFloatEpsilon))
        {
          projXZ *= (1.0f / projXZLength);
        }
        if (!glm::epsilonEqual(projYZLength, 0.0f, mk::math::kFloatEpsilon))
        {
          projYZ *= (1.0f / projYZLength);
        }

        mPitch = -glm::degrees(glm::acos(glm::clamp(glm::dot(target, projXZ), 0.0f, 1.0f)));
        mYaw = glm::degrees(glm::acos(glm::clamp(glm::dot(target, projYZ), 0.0f, 1.0f)));
      }
    }
  }
}
