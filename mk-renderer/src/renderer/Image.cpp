#include "Image.hpp"

#include <cstring>
#include <algorithm>
#include <utility>

namespace mk
{
  namespace renderer
  {
    Image::Image(std::size_t width, std::size_t height, GLenum format, const uint8_t* imageData)
    : mWidth(width),
      mHeight(height),
      mGlFormat(format),
      mData()
    {
      const std::size_t dataSize = width * height * ((format == GL_RGBA) ? 4 : 3);

      mData.reserve(dataSize);
      std::copy(imageData, imageData + dataSize, mData.begin());
    }

    std::size_t Image::getWidth() const
    {
      return mWidth;
    }

    std::size_t Image::getHeight() const
    {
      return mHeight;
    }

    GLenum Image::getFormat() const
    {
      return mGlFormat;
    }

    const uint8_t* Image::data() const
    {
      return mData.data();
    }
  }
}
