#include "RectPatch.hpp"

#include <utility>

#include <glm/vec3.hpp>
#include <glm/glm.hpp>

#include "renderer/VertexTypes.hpp"

namespace mk
{
  namespace renderer
  {
    namespace mesh
    {
      template <typename T> RectPatch<T>::RectPatch() 
      : mVao(),
        mVertices(),
        mWidth(0),
        mHeight(0)
      {
      }

      template <typename T> RectPatch<T>::RectPatch(int size)
      : RectPatch(size, size)
      {
      }

      template <typename T> RectPatch<T>::RectPatch(int width, int height)
      : mVao(),
        mVertices(width * height),
        mWidth(width),
        mHeight(height)
      {
        recreate();
      }
      
      template <typename T> RectPatch<T>::RectPatch(RectPatch<T>&& rectPatch)
      {
        mVao = std::move(rectPatch.mVao);
        mVertices = std::move(rectPatch.mVertices);
        mWidth = rectPatch.mWidth;
        mHeight = rectPatch.mHeight;
        
        rectPatch.mWidth = 0;
        rectPatch.mHeight = 0;
      }
      
      template <typename T> RectPatch<T>& RectPatch<T>::operator=(RectPatch<T>&& rectPatch)
      {
        mVao = std::move(rectPatch.mVao);
        mVertices = std::move(rectPatch.mVertices);
        mWidth = rectPatch.mWidth;
        mHeight = rectPatch.mHeight;

        rectPatch.mWidth = 0;
        rectPatch.mHeight = 0;

        return *this;
      }

      template <typename T> int RectPatch<T>::getWidth()
      {
        return mWidth;
      }

      template <typename T> int RectPatch<T>::getHeight()
      {
        return mHeight;
      }

      template <typename T> void RectPatch<T>::resize(int width, int height)
      {
        if (mVao)
        {
          mVao.reset();
        }

        mVertices.resize(width * height);
        mWidth = width;
        mHeight = height;

        recreate();
      }

      template <typename T> const T* RectPatch<T>::data()
      {
        return !mVertices.empty() ? mVertices.data() : nullptr;
      }

      template <typename T> typename RectPatch<T>::Accessor RectPatch<T>::operator[](int row)
      {
        return Accessor(mVertices.data() + row * mWidth);
      }

      template <typename T> void RectPatch<T>::computeNormals()
      {
        for (int i = 0; i < mHeight; ++i)
        for (int j = 0; j < mWidth; ++j)
        {
          const int index = i * mWidth + j;

          const glm::vec3 left = (0 == j) ? mVertices[index].mPos : mVertices[index - 1].mPos;
          const glm::vec3 right = (mWidth - 1 == j) ? mVertices[index].mPos : mVertices[index + 1].mPos;
          const glm::vec3 top = (0 == i) ? mVertices[index].mPos : mVertices[(i - 1) * mWidth + j].mPos;
          const glm::vec3 bottom = (mHeight - 1 == i) ? mVertices[index].mPos : mVertices[(i + 1) * mWidth + j].mPos;

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
        const int nIndices = mWidth * mHeight + (mWidth + 1) * (mHeight - 2);

        mVao->bind();
        mVao->render(GL_TRIANGLE_STRIP, nIndices);
      }

      template <typename T> gl::Vao<T>& RectPatch<T>::getVao()
      {
        assert(mVao && "Called getVao on an unsized RectPatch");
        return *mVao.get();
      }

      template <typename T> void RectPatch<T>::recreate()
      {
        int n = mWidth;
        int m = mHeight;

        float N = static_cast<float>(n) - 1.0f;
        float M = static_cast<float>(m) - 1.0f;

        for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
        {
          mVertices[i * n + j].mPos = glm::vec3(static_cast<float>(j) - N * 0.5f,
                                                0.0f,
                                                static_cast<float>(i) - M * 0.5f);
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

        mVao.reset(new gl::Vao<T>(mVertices, indices, GL_DYNAMIC_DRAW));
      }

      template class RectPatch<VertexPN>;
      template class RectPatch<VertexPNT>;
    }
  }
}
