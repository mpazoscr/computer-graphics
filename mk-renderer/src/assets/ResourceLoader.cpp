#include "ResourceLoader.hpp"

#include <fstream>
#include <cstddef>
#include <array>

#include <SOIL.h>

namespace mk
{
  namespace assets
  {
    namespace
    {
      const std::string kTexture = "textures";
      const std::string kShader = "shaders";
      const std::string kModel = "models";
    }

    std::string ResourceLoader::loadShaderSource(const std::string& shaderFileName)
    {
      std::string shaderFullPath = resolveShaderPath(shaderFileName);
      std::ifstream fileStream(shaderFullPath.c_str());

      return std::string((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
    }

    image::Image ResourceLoader::loadImage(const std::string& imageFileName)
    {
      std::string imageFullPath = resolveImagePath(imageFileName);

      int width;
      int height;
      int channels;

      unsigned char* imageData = SOIL_load_image(imageFullPath.c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);
      if (!imageData || (channels != 3) || (channels != 4))
      {
        throw ResourceLoader::ResourceInvalid(imageFullPath);
      }

      image::Image loadedImage = image::Image(width, height, (channels == 3) ? GL_RGB : GL_RGBA, reinterpret_cast<const uint8_t*>(imageData));
      SOIL_free_image_data(imageData);

      return loadedImage;
    }

    image::EnvironmentMap ResourceLoader::loadEnvironmentMap(const std::string& imageFileName, const std::string& extension)
    {
      std::string imageXPosFileName = resolveImagePath(imageFileName + "-xpos" + extension);
      std::string imageXNegFileName = resolveImagePath(imageFileName + "-xneg" + extension);
      std::string imageYPosFileName = resolveImagePath(imageFileName + "-ypos" + extension);
      std::string imageYNegFileName = resolveImagePath(imageFileName + "-yneg" + extension);
      std::string imageZPosFileName = resolveImagePath(imageFileName + "-zpos" + extension);
      std::string imageZNegFileName = resolveImagePath(imageFileName + "-zneg" + extension);

      std::array<std::string, 6> cubeMapImagesFilesNames =
      {{
        imageXPosFileName,
        imageXNegFileName,
        imageYPosFileName,
        imageYNegFileName,
        imageZPosFileName,
        imageZNegFileName
      }};

      image::EnvironmentMap envMap;

      for (std::size_t i = 0; i < cubeMapImagesFilesNames.size(); ++i)
      {
        int width;
        int height;
        int channels;

        unsigned char* imageData = SOIL_load_image(cubeMapImagesFilesNames[i].c_str(), &width, &height, &channels, SOIL_LOAD_RGB);
        if (!imageData)
        {
          throw ResourceLoader::ResourceInvalid(cubeMapImagesFilesNames[i]);
        }

        envMap.setImage(static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i), width, height, GL_RGB, imageData);
        SOIL_free_image_data(imageData);
      }

      return envMap;
    }

    std::string ResourceLoader::resolveImagePath(const std::string& imageFileName)
    {
      return resolveResource(imageFileName, kTexture);
    }

    std::string ResourceLoader::resolveShaderPath(const std::string& shaderFileName)
    {
      return resolveResource(shaderFileName, kShader);
    }

    std::string ResourceLoader::resolveModelPath(const std::string& modelFileName)
    {
      return resolveResource(modelFileName, kModel);
    }

    std::string ResourceLoader::resolveResource(const std::string& resourceFileName, const std::string& resourceType)
    {
      std::string fileName = resourceFileName;
      std::ifstream fileStream(fileName.c_str());

      if (!fileStream)
      {
        fileName = resourceType + std::string("/") + fileName;
        fileStream.open(fileName.c_str());

        if (!fileStream)
        {
          fileName = std::string("resources/") + fileName;
          fileStream.open(fileName.c_str());

          if (!fileStream)
          {
            fileName = std::string("../") + fileName;
            fileStream.open(fileName.c_str());

            if (!fileStream)
            {
              throw ResourceLoader::ResourceNotFound(resourceFileName);
            }
          }
        }
      }

      return fileName;
    }

    ResourceLoader::ResourceNotFound::ResourceNotFound(const std::string& resourceName)
    : mWhat(std::string("Could not find resource: ") + resourceName)
    {
    }

    const char* ResourceLoader::ResourceNotFound::what() const noexcept
    {
      return mWhat.c_str();
    }

    ResourceLoader::ResourceInvalid::ResourceInvalid(const std::string& resourceName)
    : mWhat(std::string("Invalid resource: ") + resourceName)
    {
    }

    const char* ResourceLoader::ResourceInvalid::what() const noexcept
    {
      return mWhat.c_str();
    }
  }
}
