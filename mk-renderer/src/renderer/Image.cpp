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
    
    Image::Image(Image&& image)
    {
      mWidth = image.mWidth;
      mHeight = image.mHeight;
      mGlFormat = image.mGlFormat;
      mData = std::move(image.mData);
      
      image.mWidth = 0;
      image.mHeight = 0;
      image.mGlFormat = 0;
    }
    
    Image& Image::operator=(Image&& image)
    {
      mWidth = image.mWidth;
      mHeight = image.mHeight;
      mGlFormat = image.mGlFormat;
      mData = std::move(image.mData);
      
      image.mWidth = 0;
      image.mHeight = 0;
      image.mGlFormat = 0;

      return *this;
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
      return !mData.empty() ? mData.data() : nullptr;
    }
  }
}
