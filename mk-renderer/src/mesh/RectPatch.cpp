#include "RectPatch.hpp"

#include <glm/vec3.hpp>
#include <glm/glm.hpp>

#include "core/VertexTypes.hpp"

namespace mk
{
  namespace mesh
  {
    template <typename T> RectPatch<T>::RectPatch(int n) : RectPatch(n, n)
    {
    }

    template <typename T> RectPatch<T>::RectPatch(int n, int m) : RectPatch(n, m, 1.0f)
    {
    }

    template <typename T> RectPatch<T>::RectPatch(int n, int m, float scale)
    : mVao(),
      mVertices(n * m),
      mColumns(n),
      mRows(m)
    {
      float N = static_cast<float>(n) - 1.0f;
      float M = static_cast<float>(m) - 1.0f;

      for (int i = 0; i < m; ++i)
      for (int j = 0; j < n; ++j)
      {
        mVertices[i * n + j].mPos = glm::vec3(static_cast<float>(j) - N * 0.5f,
                                              0.0f,
                                              static_cast<float>(i) - M * 0.5f) * scale;
      }

      computeNormals();

      std::vector<unsigned int> indices(n * m + (n + 1) * (m - 2));

      int index = 0;

      for (int i = 0; i < m - 1; ++i)
      for (int j = 0; j < n; ++j)
      {
        int isEvenRow = (i % 2 == 0);
        int column = j * isEvenRow + (n - 1 - j) * (1 - isEvenRow);
        int row = i * isEvenRow + (i + 1) * (1 - isEvenRow);
        int next = isEvenRow * 2 - 1;

        indices[index++] = static_cast<unsigned int>(row * n + column);
        indices[index++] = static_cast<unsigned int>((row + next) * n + column);

        if (j == (n - 1) && i < (m - 2))
        {
          indices[index++] = static_cast<unsigned int>((row + isEvenRow) * n + column);
        }
      }

      mVao.reset(new mk::gl::Vao<T>(mVertices, indices, GL_DYNAMIC_DRAW));
    }

    template <typename T> int RectPatch<T>::n()
    {
      return mColumns;
    }

    template <typename T> int RectPatch<T>::m()
    {
      return mRows;
    }

    template <typename T> const T* RectPatch<T>::data()
    {
      return mVertices.data();
    }

    template <typename T> typename RectPatch<T>::Accessor RectPatch<T>::operator[](int row)
    {
      return Accessor(mVertices.data() + row * mColumns);
    }

    template <typename T> void RectPatch<T>::computeNormals()
    {
      for (int i = 0; i < mRows; ++i)
      for (int j = 0; j < mColumns; ++j)
      {
        const int index = i * mColumns + j;

        glm::vec3 left;
        glm::vec3 right;
        glm::vec3 top;
        glm::vec3 bottom;

        left = (0 == j) ? mVertices[index].mPos : mVertices[index - 1].mPos;
        right = (mColumns - 1 == j) ? mVertices[index].mPos : mVertices[index + 1].mPos;
        top = (0 == i) ? mVertices[index].mPos : mVertices[(i - 1) * mColumns + j].mPos;
        bottom = (mRows - 1 == i) ? mVertices[index].mPos : mVertices[(i + 1) * mColumns + j].mPos;

        mVertices[index].mNormal = glm::normalize(glm::cross(bottom - top, right - left));
      }
    }

    template <typename T> void RectPatch<T>::refreshGPU()
    {
      mVao->bind();
      mVao->refreshData(0, mVertices.data(), mVertices.size());
    }

    template <typename T> void RectPatch<T>::render()
    {
      const int nIndices = mColumns * mRows + (mColumns + 1) * (mRows - 2);

      mVao->bind();
      mVao->render(GL_TRIANGLE_STRIP, nIndices);
    }

    template <typename T> GLuint RectPatch<T>::getVboId()
    {
      return mVao->getVboId();
    }

    // Template instantiations for vertex types defined in core/VertexTypes.h

    template class RectPatch<mk::core::VertexPN>;
    template class RectPatch<mk::core::VertexPC>;
    template class RectPatch<mk::core::VertexPNT>;
  }
}
