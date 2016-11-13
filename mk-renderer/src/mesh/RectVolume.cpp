#include "RectVolume.hpp"

#include <cstddef>

#include "core/VertexTypes.hpp"

namespace mk
{
  namespace mesh
  {
    template <typename T> RectVolume<T>::RectVolume(float size) : RectVolume(size, size, size)
    {
    }

    template <typename T> RectVolume<T>::RectVolume(float sizeX, float sizeY, float sizeZ)
    : mVao(),
      mVertices(36),
      mSizeX(sizeX),
      mSizeY(sizeY),
      mSizeZ(sizeZ)
    {
      float halfSizeX = mSizeX / 2.0f;
      float halfSizeY = mSizeY / 2.0f;
      float halfSizeZ = mSizeZ / 2.0f;

      glm::vec3 v1(-halfSizeX, -halfSizeY, -halfSizeZ);
      glm::vec3 v2(-halfSizeX, -halfSizeY,  halfSizeZ);
      glm::vec3 v3(-halfSizeX,  halfSizeY,  halfSizeZ);
      glm::vec3 v4(-halfSizeX,  halfSizeY, -halfSizeZ);
      glm::vec3 v5( halfSizeX, -halfSizeY, -halfSizeZ);
      glm::vec3 v6( halfSizeX, -halfSizeY,  halfSizeZ);
      glm::vec3 v7( halfSizeX,  halfSizeY,  halfSizeZ);
      glm::vec3 v8( halfSizeX,  halfSizeY, -halfSizeZ);

      glm::vec3 normalXMinus = glm::vec3(-1.0f, 0.0f, 0.0f);
      glm::vec3 normalXPlus = glm::vec3(1.0f, 0.0f, 0.0f);
      glm::vec3 normalYMinus = glm::vec3(0.0f, -1.0f, 0.0f);
      glm::vec3 normalYPlus= glm::vec3(0.0f, 1.0f, 0.0f);
      glm::vec3 normalZMinus = glm::vec3(0.0f, 0.0f, -1.0f);
      glm::vec3 normalZPlus = glm::vec3(0.0f, 0.0f, 1.0f);

      int index = 0;

      mVertices[index].mPos = v1; mVertices[index++].mNormal = normalXMinus;
      mVertices[index].mPos = v2; mVertices[index++].mNormal = normalXMinus;
      mVertices[index].mPos = v3; mVertices[index++].mNormal = normalXMinus;
      mVertices[index].mPos = v3; mVertices[index++].mNormal = normalXMinus;
      mVertices[index].mPos = v4; mVertices[index++].mNormal = normalXMinus;
      mVertices[index].mPos = v1; mVertices[index++].mNormal = normalXMinus;

      mVertices[index].mPos = v4; mVertices[index++].mNormal = normalZMinus;
      mVertices[index].mPos = v8; mVertices[index++].mNormal = normalZMinus;
      mVertices[index].mPos = v5; mVertices[index++].mNormal = normalZMinus;
      mVertices[index].mPos = v5; mVertices[index++].mNormal = normalZMinus;
      mVertices[index].mPos = v4; mVertices[index++].mNormal = normalZMinus;
      mVertices[index].mPos = v1; mVertices[index++].mNormal = normalZMinus;

      mVertices[index].mPos = v1; mVertices[index++].mNormal = normalYMinus;
      mVertices[index].mPos = v5; mVertices[index++].mNormal = normalYMinus;
      mVertices[index].mPos = v6; mVertices[index++].mNormal = normalYMinus;
      mVertices[index].mPos = v6; mVertices[index++].mNormal = normalYMinus;
      mVertices[index].mPos = v2; mVertices[index++].mNormal = normalYMinus;
      mVertices[index].mPos = v1; mVertices[index++].mNormal = normalYMinus;

      mVertices[index].mPos = v2; mVertices[index++].mNormal = normalZPlus;
      mVertices[index].mPos = v6; mVertices[index++].mNormal = normalZPlus;
      mVertices[index].mPos = v7; mVertices[index++].mNormal = normalZPlus;
      mVertices[index].mPos = v7; mVertices[index++].mNormal = normalZPlus;
      mVertices[index].mPos = v3; mVertices[index++].mNormal = normalZPlus;
      mVertices[index].mPos = v2; mVertices[index++].mNormal = normalZPlus;

      mVertices[index].mPos = v3; mVertices[index++].mNormal = normalYPlus;
      mVertices[index].mPos = v7; mVertices[index++].mNormal = normalYPlus;
      mVertices[index].mPos = v8; mVertices[index++].mNormal = normalYPlus;
      mVertices[index].mPos = v8; mVertices[index++].mNormal = normalYPlus;
      mVertices[index].mPos = v4; mVertices[index++].mNormal = normalYPlus;
      mVertices[index].mPos = v3; mVertices[index++].mNormal = normalYPlus;

      mVertices[index].mPos = v6; mVertices[index++].mNormal = normalXPlus;
      mVertices[index].mPos = v5; mVertices[index++].mNormal = normalXPlus;
      mVertices[index].mPos = v8; mVertices[index++].mNormal = normalXPlus;
      mVertices[index].mPos = v8; mVertices[index++].mNormal = normalXPlus;
      mVertices[index].mPos = v7; mVertices[index++].mNormal = normalXPlus;
      mVertices[index].mPos = v6; mVertices[index++].mNormal = normalXPlus;

      mVao.reset(new mk::gl::Vao<T>(mVertices, GL_STATIC_DRAW));
    }

    template <typename T> float RectVolume<T>::sizeX()
    {
      return mSizeX;
    }

    template <typename T> float RectVolume<T>::sizeY()
    {
      return mSizeY;
    }

    template <typename T> float RectVolume<T>::sizeZ()
    {
      return mSizeZ;
    }

    template <typename T> void RectVolume<T>::invertNormals()
    {
      for (std::size_t i = 0; i < mVertices.size(); ++i)
      {
        mVertices[i].mNormal = -mVertices[i].mNormal;
      }

      mVao->refreshData(0, mVertices.data(), mVertices.size());
    }

    template <typename T> void RectVolume<T>::render()
    {
      mVao->bind();
      mVao->render(GL_TRIANGLES, mVertices.size());
    }

    // Template instantiations for vertex types defined in core/VertexTypes.h

    template class RectVolume<mk::core::VertexPN>;
    template class RectVolume<mk::core::VertexPNT>;
  }
}
