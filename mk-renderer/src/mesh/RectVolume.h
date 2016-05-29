#ifndef SRC_MESH_RECTVOLUME_H_
#define SRC_MESH_RECTVOLUME_H_

#include <vector>
#include <memory>

#include "gl/Vao.h"

namespace mk
{
  namespace mesh
  {
    /**
     * sizeX x sizeY x sizeZ rectangular parallelepiped.
     */
    template <typename T> class RectVolume
    {
    public:
      /**
       * Constructs a cube of size x size x size centered at (0, 0, 0) with normals pointing outwards.
       *
       * This function also computes the normals and allocates all the required GPU resources.
       *
       * @param size Size of the cube.
       */
      RectVolume(float size);

      /**
       * Constructs a parallelepiped of sizeX x sizeY x sizeZ centered at (0, 0, 0) with normals pointing outwards.
       *
       * This function also computes the normals and allocates all the required GPU resources.
       *
       * @param sizeX Size of the side of the paralleliped parallel to the X axis.
       * @param sizeY Size of the side of the paralleliped parallel to the Y axis.
       * @param sizeZ Size of the side of the paralleliped parallel to the Z axis.
       */
      RectVolume(float sizeX, float sizeY, float sizeZ);

      /**
       * @return The size of the side of the RectVolume that is parallel to the X axis.
       */
      float sizeX();

      /**
       * @return The size of the side of the RectVolume that is parallel to the Y axis.
       */
      float sizeY();

      /**
       * @return The size of the side of the RectVolume that is parallel to the Z axis.
       */
      float sizeZ();

      /**
       * Inverts the direction of the normals
       */
      void invertNormals();

      /**
       * Renders the parallelepiped using the underlying Vao.
       */
      void render();

    private:
      std::unique_ptr<mk::gl::Vao<T>> mVao;
      std::vector<T> mVertices;
      float mSizeX;
      float mSizeY;
      float mSizeZ;
    };
  }
}

#endif  // SRC_MESH_RECTVOLUME_H_