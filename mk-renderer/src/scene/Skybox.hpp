#ifndef SRC_SCENE_SKYBOX_HPP_
#define SRC_SCENE_SKYBOX_HPP_

#include <array>

#include "core/VertexTypes.hpp"
#include "image/EnvironmentMap.hpp"
#include "gl/ShaderProgram.hpp"
#include "mesh/Quad.hpp"
#include "scene/Camera.hpp"

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
       */
      Skybox(image::EnvironmentMap envMap, const scene::ICamera& camera);

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
