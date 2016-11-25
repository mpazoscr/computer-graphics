#ifndef SRC_RENDERER_IMAGE_H_
#define SRC_RENDERER_IMAGE_H_

#include <GL/glew.h>

#include <cstdint>
#include <cstddef>
#include <vector>

namespace mk
{
  namespace renderer
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
       * Default destructor.
       */
      ~Image() = default;

      /**
       * Disable copy construction.
       */
      Image(const Image&) = delete;

      /**
       * Disable assignment.
       */
      Image& operator=(const Image&) = delete;
      
      /**
       * Move constructor.
       * @param image Instance to be moved.
       */
      Image(Image&& image);
      
      /**
       * Move assignment operator.
       * @param image Instance to be moved.
       */
      Image& operator=(Image&& image);

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
      const uint8_t* data() const;

    private:
      std::size_t mWidth;
      std::size_t mHeight;
      GLenum mGlFormat;
      std::vector<std::uint8_t> mData;
    };
  }
}

#endif  // SRC_RENDERER_IMAGE_H_
