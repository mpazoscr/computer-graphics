#ifndef SRC_MESH_RECTPATCH_H_
#define SRC_MESH_RECTPATCH_H_

#include <vector>
#include <memory>

#include "gl/Vao.hpp"

namespace mk
{
  namespace mesh
  {
    /**
     * NxM Patch modeled as a single triangle strip.
     *
     * In the rest of the documentation, N can be referred to as number of columns, and
     * M as number of rows. The patch is created such that the side parallel to the X axis has size N
     * and the side parallel to the Z axix has size M.
     *
     * Useful to model height fields: terrain, ocean, ...
     */
    template <typename T> class RectPatch
    {
    public:
      /**
       * Helper class to support RectPatch in providing the operator[][]
       */
      class Accessor
      {
      public:
        /**
         * Constructs an Accessor object given a pointer to a specific row in the patch.
         *
         * @param row A pointer to a specific row in the patch.
         */
        Accessor(T* row) : mRow(row) {}

        /**
         * Operator[] which returns the single requested element.
         *
         * @param column Index of the requested vertex inside the selected row.
         */
        T& operator[](int column) { return mRow[column]; }

      private:
        T* mRow;
      };

      /**
       * Constructs a flat (i.e. Y coordinate equal to 0) nxn patch centered in (0, 0, 0).
       *
       * This function also computes the initial normals and allocates all the required GPU resources.
       *
       * @param n Number of vertices per patch side.
       */
      RectPatch(int n);

      /**
       * Constructs a flat (i.e. Y coordinate equal to 0) nxm patch centered in (0, 0, 0).
       *
       * This function also computes the initial normals and allocates all the required GPU resources.
       *
       * @param n Number of vertices of one side of the patch.
       * @param m Number of vertices of the other side of the patch.
       */
      RectPatch(int n, int m);

      /**
       * Constructs a flat (i.e. Y coordinate equal to 0) nxm patch centered in (0, 0, 0).
       *
       * This function also computes the initial normals and allocates all the required GPU resources.
       *
       * @param n Number of vertices of one side of the patch.
       * @param m Number of vertices of the other side of the patch.
       * @param scale Scale factor to be applied to the coordinates of the patch
       */
      RectPatch(int n, int m, float scale);

      /**
       * @return The size of the side of the RectPatch that is parallel to the X axis.
       */
      int n();

      /**
       * @return The size of the side of the RectPatch that is parallel to the Z axis.
       */
      int m();

      /**
       * @return A raw pointer to the buffer that contains the vertex data.
       */
      const T* data();

      /**
       * Operator[] that returns a proxy object whose operator[] will return a vertex of the patch.
       * @param row Index of the row where the requested vertex is.
       * @return Accessor object whose operator[] will return a vertex of the patch.
       */
      Accessor operator[](int row);

      /**
       * Compute the vertices normals and store them in the internal buffer.
       *
       * The computed normals will point in the +Y direction. This function does not
       * send the new data to the GPU.
       */
      void computeNormals();

      /**
       * Refreshes the contents of the corresponding buffers in GPU.
       *
       * It is responsability of the user to call this method after vertex data or normals have been
       * modified.
       */
      void refreshGPU();

      /**
       * Renders the patch using the underlying Vao.
       */
      void render();

      /**
       * @return The id of the underlying OpenGL VBO.
       */
      mk::gl::Vao<T>& getVao();

    private:
      std::unique_ptr<mk::gl::Vao<T>> mVao;
      std::vector<T> mVertices;
      int mColumns;
      int mRows;
    };
  }
}

#endif  // SRC_MESH_RECTPATCH_H_
