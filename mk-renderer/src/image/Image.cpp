#include "Image.hpp"

#include <cstring>

namespace mk
{
  namespace image
  {
    Image::Image(int width, int height, GLenum format, const uint8_t* imageData)
    : mWidth(width),
      mHeight(height),
      mGlFormat(format)
    {
      int dataSize = width * height * ((format == GL_RGBA) ? 4 : 3);

      mData = new uint8_t[dataSize];
      std::memcpy(mData, imageData, dataSize);
    }

    Image::~Image()
    {
      delete[] mData;
    }

    int Image::getWidth() const
    {
      return mWidth;
    }

    int Image::getHeight() const
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
