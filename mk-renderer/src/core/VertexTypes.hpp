#ifndef SRC_CORE_VERTEXTYPES_H_
#define SRC_CORE_VERTEXTYPES_H_

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace mk
{
  namespace core
  {
    struct VertexP
    {
      glm::vec3 mPos;
    };

    struct VertexPN
    {
      glm::vec3 mPos;
      glm::vec3 mNormal;
    };

    struct VertexPC
    {
      glm::vec3 mPos;
      glm::vec4 mColour;
    };

    struct VertexPNT
    {
      glm::vec3 mPos;
      glm::vec3 mNormal;
      glm::vec2 mTexCoords;
    };
  }
}

#endif  // SRC_CORE_VERTEXTYPES_H_
