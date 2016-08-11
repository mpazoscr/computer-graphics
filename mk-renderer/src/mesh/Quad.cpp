#include "Quad.hpp"

#include <glm/glm.hpp>

#include "core/VertexTypes.hpp"

namespace mk
{
  namespace mesh
  {
    template <typename T> Quad<T>::Quad(const std::array<glm::vec3, 4>& points)
    : mVao(),
      mVertices(4)
    {
      initMesh(points);
      initNormals(points);

      mVao.reset(new mk::gl::Vao<T>(mVertices, GL_STATIC_DRAW));

      refreshGPU();
    }

    template <typename T> void Quad<T>::invertNormal()
    {
      for (int i = 0; i < mVertices.size(); ++i)
      {
        mVertices[i].mNormal = -mVertices[i].mNormal;
      }

      refreshGPU();
    }

    template <typename T> void Quad<T>::render()
    {
      mVao->bind();
      mVao->render(GL_TRIANGLE_FAN, mVertices.size());
    }

    template <typename T> void Quad<T>::initMesh(const std::array<glm::vec3, 4>& points)
    {
      mVertices[0].mPos = points[0];
      mVertices[1].mPos = points[1];
      mVertices[2].mPos = points[2];
      mVertices[3].mPos = points[3];
    }

    template <typename T> void Quad<T>::initNormals(const std::array<glm::vec3, 4>& points)
    {
      glm::vec3 r = points[1] - points[0];
      glm::vec3 s = points[2] - points[1];
      glm::vec3 n = glm::normalize(glm::cross(r, s));

      mVertices[0].mNormal = n;
      mVertices[1].mNormal = n;
      mVertices[2].mNormal = n;
      mVertices[3].mNormal = n;
    }

    template <typename T> void Quad<T>::refreshGPU()
    {
      mVao->bind();
      mVao->refreshData(0, mVertices.data(), mVertices.size());
    }

    // Template instantiations for vertex types defined in core/VertexTypes.h

    template class Quad<mk::core::VertexPN>;
  }
}
