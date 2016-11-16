#ifndef SRC_CORE_RESOURCE_LOADER_H_
#define SRC_CORE_RESOURCE_LOADER_H_

#include <string>
#include <stdexcept>
#include <array>

#include "image/Image.hpp"
#include "image/EnvironmentMap.hpp"

namespace mk
{
  namespace assets
  {
    class ResourceLoader
    {
    public:
      class ResourceNotFound : public std::exception
      {
      public:
        ResourceNotFound(const std::string& resourceName);
        virtual const char* what() const noexcept;

      private:
        const std::string mWhat;
      };

      class ResourceInvalid : public std::exception
      {
      public:
        ResourceInvalid(const std::string& resourceName);
        virtual const char* what() const noexcept;

      private:
        const std::string mWhat;
      };

    public:
      /**
       * Loads the source code of a shader given its file name
       * @param shaderFileName Name of the file containig the shader
       * @return Shader source code
       * @throw ResourceLoader::ResourceNotFound if the file is not found
       */
      static std::string loadShaderSource(const std::string& shaderFileName);

      /**
       * Loads the image data given the image file name
       * @param imageFileName Name of the file containig the image
       * @return Image data
       * @throw ResourceLoader::ResourceNotFound if the file is not found
       */
      static image::Image loadImage(const std::string& imageFileName);

      /**
       * Loads six images with prefix given by the input parameter and suffixes -xpos, -xneg, -ypos, ...
       * @param imagesFileNamePrefix Prefix of the files names containing the environment map
       * @param extension Extension of the image files (including the dot!)
       * @return The environment map
       * @throw ResourceLoader::ResourceNotFound if the file is not found
       */
      static image::EnvironmentMap loadEnvironmentMap(const std::string& imagesFileNamePrefix, const std::string& extension);

    private:
      ResourceLoader();

      static std::string resolveImagePath(const std::string& imageFileName);
      static std::string resolveShaderPath(const std::string& shaderFileName);
      static std::string resolveModelPath(const std::string& modelFileName);
      static std::string resolveResource(const std::string& resourceFileName, const std::string& resourceType);
    };
  }
}

#endif // SRC_CORE_RESOURCE_LOADER_H_
