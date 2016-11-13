#ifndef SRC_IMAGE_IMAGE_H_
#define SRC_IMAGE_IMAGE_H_

#include <GL/glew.h>

#include <cstdint>
#include <cstddef>

namespace mk
{
  namespace image
  {
    /**
     * Class representing a 2D texture stored in GPU.
     */
    class Image
    {
    public:
      /**
       * Initialises this object copying the input data
       * @param width Width of the image in pixels
       * @param height Height of the image in pixels
       * @param format GL Format of the image indicating the pixel size
       * @param imageData Image data of size (width * height * format)
       */
      Image(std::size_t width, std::size_t height, GLenum format, const std::uint8_t* imageData);

      /**
       * Frees the image data.
       */
      ~Image();

      /**
       * @return The width of the image in pixels
       */
      std::size_t getWidth() const;

      /**
       * @return The height of the image in pixels
       */
      std::size_t getHeight() const;

      /**
       * @return The GL format of the image
       */
      GLenum getFormat() const;

      /**
       * @return The image data
       */
      const unsigned char* data() const;

    private:
      const std::size_t mWidth;
      const std::size_t mHeight;
      const GLenum mGlFormat;
      std::uint8_t* mData;
    };
  }
}

#endif  // SRC_IMAGE_IMAGE_H_
