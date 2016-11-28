#ifndef SRC_RENDERER_MESH_RECTPATCH_H_
#define SRC_RENDERER_MESH_RECTPATCH_H_

#include <vector>
#include <memory>

#include "renderer/gl/Vao.hpp"

namespace mk
{
  namespace renderer
  {
    namespace mesh
    {
      /**
       * Width x Height Patch modeled as a single triangle strip.
       *
       * The patch is created such that the side parallel to the X axis has size Width vertices
       * and the side parallel to the Z axis has size Height.
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
          Accessor(T* row) : mRow(row)
          {
          }

          /**
           * Operator[] which returns the single requested element.
           *
           * @param column Index of the requested vertex inside the selected row.
           */
          T& operator[](int column)
          {
            return mRow[column];
          }

        private:
          T* mRow;
        };

        /**
         * Constructs an empty RectPatch. If this constructor is used, the function resize 
         * must be called to configure the patch size before doing any other operation.
         */
        RectPatch();

        /**
         * Constructs a flat (i.e. Y coordinate equal to 0) size x size patch centered in (0, 0, 0).
         *
         * This function also computes the initial normals and allocates all the required GPU resources.
         *
         * @param size Number of vertices per patch side.
         */
        RectPatch(int size);

        /**
         * Constructs a flat (i.e. Y coordinate equal to 0) nxm patch centered in (0, 0, 0).
         *
         * This function also computes the initial normals and allocates all the required GPU resources.
         *
         * @param width Number of vertices of the side of the RectPatch that is parallel to the X axis.
         * @param height Number of vertices of the side of the RectPatch that is parallel to the Z axis.
         */
        RectPatch(int width, int height);
        
        /**
         * Default destructor.
         */
        ~RectPatch() = default;
        
        /**
         * Disable copy construction.
         */
        RectPatch(const RectPatch&) = delete;
        
        /**
         * Disable assignment.
         */
        RectPatch& operator=(const RectPatch&) = delete;
        
        /**
         * Move constructor.
         * @param rectPatch Instance to be moved.
         */
        RectPatch(RectPatch&& rectPatch);
        
        /**
         * Move assignment operator.
         * @param rectPatch Instance to be moved.
         */
        RectPatch& operator=(RectPatch&& rectPatch);

        /**
         * @return The size of the side of the RectPatch that is parallel to the X axis.
         */
        int getWidth();

        /**
         * @return The size of the side of the RectPatch that is parallel to the Z axis.
         */
        int getHeight();

        /**
         * Discards any previous data (GPU resources included) and recreates the
         * patch with the given dimensions.
         * @param width Number of vertices of the side of the RectPatch that is parallel to the X axis.
         * @param height Number of vertices of the side of the RectPatch that is parallel to the Z axis.
         */
        void resize(int width, int height);

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
         * @warning This function should only be called after either the constructor accepting 
         *          the size or the resize function have been called.
         */
        gl::Vao<T>& getVao();

      private:
        void recreate();

      private:
        std::unique_ptr<gl::Vao<T>> mVao;
        std::vector<T> mVertices;
        int mWidth;
        int mHeight;
      };
    }
  }
}

#endif  // SRC_RENDERER_MESH_RECTPATCH_H_
