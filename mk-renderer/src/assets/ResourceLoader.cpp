#include "ResourceLoader.hpp"

#include <fstream>
#include <cassert>

#include "ImageMagick-6/Magick++.h"



#include <iostream>

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

    void ResourceLoader::Initialize()
    {
      Magick::InitializeMagick(nullptr);
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

      try
      {
         Magick::Image image(imageFullPath);
         image.magick("RGBA");

         Magick::Blob blob;
         image.write(&blob);

         return image::Image(image.columns(), image.rows(), GL_RGBA, reinterpret_cast<const uint8_t*>(blob.data()));
      }
      catch (...)
      {
        throw ResourceLoader::ResourceInvalid(imageFullPath);
      }
    }

    image::EnvironmentMap ResourceLoader::loadEnvironmentMap(const std::string& imageFileName, const std::string& extension)
    {
      std::string imageZNegFileName = resolveImagePath(imageFileName + "-zneg" + extension);
      std::string imageZPosFileName = resolveImagePath(imageFileName + "-zpos" + extension);
      std::string imageYNegFileName = resolveImagePath(imageFileName + "-yneg" + extension);
      std::string imageYPosFileName = resolveImagePath(imageFileName + "-ypos" + extension);
      std::string imageXNegFileName = resolveImagePath(imageFileName + "-xneg" + extension);
      std::string imageXPosFileName = resolveImagePath(imageFileName + "-xpos" + extension);

      try
      {
        Magick::Image imageZNeg(imageZNegFileName);
        Magick::Image imageZPos(imageZPosFileName);
        Magick::Image imageYNeg(imageYNegFileName);
        Magick::Image imageYPos(imageYPosFileName);
        Magick::Image imageXNeg(imageXNegFileName);
        Magick::Image imageXPos(imageXPosFileName);

        imageZNeg.magick("RGB");
        imageZPos.magick("RGB");
        imageYNeg.magick("RGB");
        imageYPos.magick("RGB");
        imageXNeg.magick("RGB");
        imageXPos.magick("RGB");

        Magick::Blob blobZNeg;
        Magick::Blob blobZPos;
        Magick::Blob blobYNeg;
        Magick::Blob blobYPos;
        Magick::Blob blobXNeg;
        Magick::Blob blobXPos;

        imageZNeg.write(&blobZNeg);
        imageZPos.write(&blobZPos);
        imageYNeg.write(&blobYNeg);
        imageYPos.write(&blobYPos);
        imageXNeg.write(&blobXNeg);
        imageXPos.write(&blobXPos);

        image::EnvironmentMap envMap;

        envMap.setMinusZ(imageZNeg.columns(), imageZNeg.rows(), GL_RGB, blobZNeg.data());
        envMap.setPlusZ(imageZPos.columns(), imageZPos.rows(), GL_RGB, blobZPos.data());
        envMap.setMinusY(imageYNeg.columns(), imageYNeg.rows(), GL_RGB, blobYNeg.data());
        envMap.setPlusY(imageYPos.columns(), imageYPos.rows(), GL_RGB, blobYPos.data());
        envMap.setMinusX(imageXNeg.columns(), imageXNeg.rows(), GL_RGB, blobXNeg.data());
        envMap.setPlusX(imageXPos.columns(), imageXPos.rows(), GL_RGB, blobXPos.data());

        return envMap;
      }
      catch (...)
      {
        throw ResourceLoader::ResourceInvalid(imageFileName);
      }
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
        fileName = resourceType + std::string("/") + resourceFileName;
        fileStream.open(fileName.c_str());

        if (!fileStream)
        {
          fileName = std::string("resources/") + resourceType + std::string("/") + resourceFileName;
          fileStream.open(fileName.c_str());

          if (!fileStream)
          {
            throw ResourceLoader::ResourceNotFound(resourceFileName);
          }
        }
      }

      return fileName;
    }

    ResourceLoader::ResourceNotFound::ResourceNotFound(const std::string& resourceName)
    : mWhat(std::string("Could not find resource: ") + resourceName)
    {
      // do nothing
    }

    const char* ResourceLoader::ResourceNotFound::what() const noexcept
    {
      return mWhat.c_str();
    }

    ResourceLoader::ResourceInvalid::ResourceInvalid(const std::string& resourceName)
    : mWhat(std::string("Invalid resource: ") + resourceName)
    {
      // do nothing
    }

    const char* ResourceLoader::ResourceInvalid::what() const noexcept
    {
      return mWhat.c_str();
    }
  }
}
