#include "MouseFilters.h"

#include <glm/glm.hpp>

#include "math/Utils.h"
#include "input/IMouseProvider.h"

namespace mk
{
  namespace input
  {
    MouseFilter::MouseFilter(IMouseProvider& mouseProvider)
      : mMouseProvider(mouseProvider)
    {
      mLastMousePos = mMouseProvider.getMousePosition();
    }

    void MouseFilter::setMouseSpeedFactor(float mouseSpeedFactor)
    {
      mMouseSpeedFactor = mouseSpeedFactor;
    }

    float MouseFilter::mouseSpeedFactor() const
    {
      return mMouseSpeedFactor;
    }

    void MouseFilter::setMouseDeceleration(float mouseDeceleration)
    {
      mMouseDeceleration = mouseDeceleration;
    }

    float MouseFilter::mouseDeceleration() const
    {
      return mMouseDeceleration;
    }

    NullMouseFilter::NullMouseFilter(IMouseProvider& mouseProvider)
      : MouseFilter(mouseProvider)
    {
    }

    glm::vec2 NullMouseFilter::mouseDelta(float dt)
    {
      glm::vec2 newMouse = mMouseProvider.getMousePosition();

      glm::vec2 mousePosDelta = mLastMousePos - newMouse;

      mLastMousePos = newMouse;

      return mousePosDelta * mouseSpeedFactor() * dt;
    }

    const double kExpDecayCoefficient = 200.0;

    SmoothMouseFilter::SmoothMouseFilter(IMouseProvider& mouseProvider)
      : MouseFilter(mouseProvider)
    {
    }

    glm::vec2 SmoothMouseFilter::mouseDelta(float dt)
    {
      glm::vec2 newMouse = mMouseProvider.getMousePosition();

      float alpha = static_cast<float>(mk::math::expDecayPow2(kExpDecayCoefficient * dt));

      newMouse = mLastMousePos * alpha + newMouse * (1.0f - alpha);

      glm::vec2 mousePosDelta = mLastMousePos - newMouse;

      mLastMousePos = newMouse;

      return mousePosDelta * mouseSpeedFactor() * dt;
    }

    InertiaMouseFilter::InertiaMouseFilter(IMouseProvider& mouseProvider)
      : MouseFilter(mouseProvider)
    {
    }

    glm::vec2 InertiaMouseFilter::mouseDelta(float dt)
    {
      glm::vec2 newMouse = mMouseProvider.getMousePosition();

      glm::vec2 mousePosDelta = mLastMousePos - newMouse;

      mLastMousePos = newMouse;

      if (glm::any(glm::greaterThan(glm::abs(mousePosDelta), glm::vec2(mk::math::kFloatEpsilon))))
      {
        mInertiaDir = glm::normalize(mousePosDelta);
        mSpeed = glm::max(mSpeed, mouseSpeedFactor() * glm::length(mousePosDelta));
      }
      else
      {
        mSpeed = glm::clamp(mSpeed - mouseDeceleration() * dt, 0.0f, mouseSpeedFactor());
        mousePosDelta = mInertiaDir;
      }

      return mousePosDelta * mSpeed * dt;
    }
  }
}
