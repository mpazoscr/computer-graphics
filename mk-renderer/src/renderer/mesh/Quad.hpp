#ifndef SRC_RENDERER_MESH_QUAD_HPP_
#define SRC_RENDERER_MESH_QUAD_HPP_

#include <glm/vec3.hpp>
#include <memory>
#include <array>

#include "renderer/gl/Vao.hpp"

namespace mk
{
  namespace renderer
  {
    namespace mesh
    {
      /**
       * Simple quad defined by bottom/left-top/right
       */
      template <typename T> class Quad
      {
      public:
        /**
         * Constructs a quad from the given 4 points.
         *
         * This function also computes the normal pointing to the viewer and allocates all the required GPU resources.
         *
         * @param points The 4 point that form the quad. The points should be ordered counter clockwise from
         * the viewer's perspective.
         */
        Quad(const std::array<glm::vec3, 4>& points);

        /**
         * Inverts the direction of the normal
         */
        void invertNormal();

        /**
         * Renders the quad using the underlying Vao.
         */
        void render();

      private:
        std::unique_ptr<gl::Vao<T>> mVao;
        std::vector<T> mVertices;

      private:
        void initMesh(const std::array<glm::vec3, 4>& points);
        void initNormals(const std::array<glm::vec3, 4>& points);
        void refreshGPU();
      };
    }
  }
}

#endif  // SRC_RENDERER_MESH_QUAD_HPP_
