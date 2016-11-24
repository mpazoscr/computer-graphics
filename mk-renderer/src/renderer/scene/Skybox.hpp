#ifndef SRC_RENDERER_SCENE_SKYBOX_HPP_
#define SRC_RENDERER_SCENE_SKYBOX_HPP_

#include <array>

#include "renderer/VertexTypes.hpp"
#include "renderer/gl/CubeMap.hpp"
#include "renderer/gl/ShaderProgram.hpp"
#include "renderer/mesh/Quad.hpp"
#include "renderer/scene/Camera.hpp"

namespace mk
{
  namespace renderer
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
         * @param cubeMap The cube map to texture the skybox.
         * @param camera Camera used to retrieve the view and projection matrices.
         */
        Skybox(const gl::CubeMap& cubeMap, const scene::ICamera& camera);

        /**
         * Renders the skybox.
         */
        void render();

      private:
        const gl::CubeMap& mCubeMap;
        gl::ShaderProgram mShader;
        const scene::ICamera& mCamera;
        std::array<mesh::Quad<VertexPN>, 6> mQuads;
      };
    }
  }
}

#endif  // SRC_RENDERER_SCENE_SKYBOX_HPP_
