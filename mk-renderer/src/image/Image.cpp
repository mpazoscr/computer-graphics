#include "Image.hpp"

#include <cstring>

namespace mk
{
  namespace image
  {
    Image::Image(std::size_t width, std::size_t height, GLenum format, const uint8_t* imageData)
    : mWidth(width),
      mHeight(height),
      mGlFormat(format)
    {
      std::size_t dataSize = width * height * ((format == GL_RGBA) ? 4 : 3);

      mData = new uint8_t[dataSize];
      std::memcpy(mData, imageData, dataSize);
    }

    Image::~Image()
    {
      delete[] mData;
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

    const unsigned char* Image::data() const
    {
      return mData;
    }
  }
}
