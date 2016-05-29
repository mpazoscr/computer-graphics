#ifndef SRC_SCENE_SKYBOX_HPP_
#define SRC_SCENE_SKYBOX_HPP_

#include <array>

#include "core/VertexTypes.h"
#include "image/EnvironmentMap.hpp"
#include "gl/ShaderProgram.h"
#include "mesh/Quad.hpp"
#include "scene/Camera.h"

namespace mk
{
  namespace scene
  {
    /**
     * Skybox
     */
    class Skybox
    {
    public:
      /**
       * Constructs a skybox centered at the origin and with a given size.
       * @param size Size of each side of the skybox.
       */
      Skybox(float size, image::EnvironmentMap envMap, const scene::ICamera& camera);

      /**
       * Renders the skybox.
       */
      void render();

    private:
      image::EnvironmentMap mEnvMap;
      mk::gl::ShaderProgram mShader;
      const scene::ICamera& mCamera;
      std::array<mesh::Quad<core::VertexPN>, 6> mQuads;
    };
  }
}

#endif  // SRC_SCENE_SKYBOX_HPP_
