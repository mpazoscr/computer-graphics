#include <memory>
#include <random>
#include <functional>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "demofw/glfw/BaseDemoApp.hpp"
#include "renderer/VertexTypes.hpp"
#include "renderer/gl/Vao.hpp"
#include "renderer/gl/ShaderProgram.hpp"
#include "renderer/assets/ResourceLoader.hpp"
#include "physics/fluids/FLIPSolver2D.hpp"

namespace
{
  const double kFramerate = 1.0 / 35.0;

  const int kGridWidth = 50;
  const int kGridHeight = 50;
  const int kRenderGridCellSize = 10;

  const glm::vec4 kColourFluid(0.4f, 0.4f, 1.0f, 1.0f);
  const glm::vec4 kColourSolid(0.0f, 0.3f, 0.0f, 1.0f);
  const glm::vec4 kColourParticles(1.0f, 0.0f, 0.0f, 1.0f);

  const float kPaticleSize(3.0f);

  const float kSolverGridSize = 0.02f;
  const float kFluidDensity = 1.0f;
  const float kPicFlipFactor = 0.0f; // It's value must lie within [0, 1].

  class RenderGrid2D
  {
  public:
    RenderGrid2D(int width, int height, int cellSize)
    : mWidth(width),
      mHeight(height),
      mCellSize(cellSize),
      mVertices(width * height * 4),
      mVao()
    {
      std::vector<unsigned int> indices(width * height * 6);

      int vertexCounter = 0;
      int indexCounter = 0;

      for (int j = 0; j < height; j++)
      for (int i = 0; i < width; i++)
      {
        // Checkers pattern to make it clear that the colour is no initialised

        const glm::vec4 colour = (i + j % 2) ? glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) : glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        mVertices[vertexCounter + 0].mPos = glm::vec3(i * cellSize, j * cellSize, 0.0f);
        mVertices[vertexCounter + 1].mPos = glm::vec3(i * cellSize + cellSize, j * cellSize, 0.0f);
        mVertices[vertexCounter + 2].mPos = glm::vec3(i * cellSize + cellSize, j * cellSize + cellSize, 0.0f);
        mVertices[vertexCounter + 3].mPos = glm::vec3(i * cellSize, j * cellSize + cellSize, 0.0f);

        mVertices[vertexCounter + 0].mColour = colour;
        mVertices[vertexCounter + 1].mColour = colour;
        mVertices[vertexCounter + 2].mColour = colour;
        mVertices[vertexCounter + 3].mColour = colour;

        indices[indexCounter++] = vertexCounter + 0;
        indices[indexCounter++] = vertexCounter + 1;
        indices[indexCounter++] = vertexCounter + 2;
        indices[indexCounter++] = vertexCounter + 2;
        indices[indexCounter++] = vertexCounter + 3;
        indices[indexCounter++] = vertexCounter + 0;

        vertexCounter += 4;
      }

      mVao.reset(new mk::renderer::gl::Vao<mk::renderer::VertexPC>(mVertices, indices, GL_DYNAMIC_DRAW));
    }

    int getWidth() const
    {
      return mWidth;
    }

    int getHeight() const
    {
      return mHeight;
    }

    int getCellSize() const
    {
      return mCellSize;
    }

    void setColour(int i, int j, const glm::vec4& colour)
    {
      const int vertexIndex = i + j * mWidth;

      mVertices[vertexIndex + 0].mColour = colour;
      mVertices[vertexIndex + 1].mColour = colour;
      mVertices[vertexIndex + 2].mColour = colour;
      mVertices[vertexIndex + 3].mColour = colour;
    }

    void upload()
    {      
      mVao->refreshData(0, mVertices.data(), mVertices.size());
    }

    void render()
    {
      mVao->bind();
      mVao->render(GL_TRIANGLES, mVertices.size() * 2);
    }

  private:
    int mWidth;
    int mHeight;
    int mCellSize;

    std::vector<mk::renderer::VertexPC> mVertices;
    std::unique_ptr<mk::renderer::gl::Vao<mk::renderer::VertexPC>> mVao;
  };

  class RenderParticles
  {
  public:
    RenderParticles()
    : mParticles(),
      mParticlesVao()
    {
    }

    void update(const FLIPSolver2D& flipSolver)
    {
      mParticles.resize(flipSolver.particles.np);

      const float renderGridCellSize = static_cast<float>(kRenderGridCellSize);

      for (int p = 0; p < flipSolver.particles.np; p++)
      {
        const float x = flipSolver.particles.x[p].x * (renderGridCellSize / kSolverGridSize);
        const float y = flipSolver.particles.x[p].y * (renderGridCellSize / kSolverGridSize);

        mParticles[p].mPos = glm::vec3(x, y, 0.0f);
        mParticles[p].mColour = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
      }
    }

    void upload()
    {
      if (!mParticlesVao)
      {
        mParticlesVao.reset(new mk::renderer::gl::Vao<mk::renderer::VertexPC>(mParticles, GL_DYNAMIC_DRAW));
      }
      else
      {
        mParticlesVao->refreshData(0, mParticles.data(), mParticles.size());
      }
    }

    void render()
    {
      mParticlesVao->bind();
      mParticlesVao->render(GL_POINTS, mParticles.size());
    }

  private:
    std::vector<mk::renderer::VertexPC> mParticles;
    std::unique_ptr<mk::renderer::gl::Vao<mk::renderer::VertexPC>> mParticlesVao;
  };

  class FLIPDemo2D : public mk::demofw::glfw::BaseDemoApp
  {
  public:
    FLIPDemo2D(const std::string& title, int gridWidth, int gridHeight, int renderGridCellSize)
    : mk::demofw::glfw::BaseDemoApp(title, gridWidth * renderGridCellSize, gridHeight * renderGridCellSize),
      mFlipSolver(ADVECTION_SIMPLE, gridWidth, gridHeight, kSolverGridSize, kFluidDensity),
      mRenderGrid(gridWidth, gridHeight, renderGridCellSize),
      mRenderParticles(),
      mColouredVertexProgram(),
      mRandomDevice(),
      mMersenneTwister(mRandomDevice()),
      mUniformDist(0.0f, 1.0f),
      mDrawParticles(false)
    {
      // The solver grid is internally initialised as solid in the domain boundaries and fluid in the rest
      // of cells (internal cells). Here, we set all internal cells as air (empty) cells.

      for (int j = 1; j < gridHeight - 1; j++)
      for (int i = 1; i < gridWidth - 1; i++)
      {
        mFlipSolver.setCellType(i, j, CELL_AIR);
      }

      // No velocity for the boundary

      mFlipSolver.setBoundaryVel(0.0f, 0.0f);
      mFlipSolver.setPicFlipFactor(kPicFlipFactor);

      // Load shader

      mColouredVertexProgram.attachVertexShader(mk::renderer::assets::ResourceLoader::loadShaderSource("coloured_vertex.vert"));
      mColouredVertexProgram.attachFragmentShader(mk::renderer::assets::ResourceLoader::loadShaderSource("coloured_vertex.frag"));
      mColouredVertexProgram.link();

      setCustomKeyPressedCallback(std::bind(&FLIPDemo2D::keyPressedCallback, this, std::placeholders::_1));
    }

    virtual void update(double elapsedTime, double globalTime)
    {
      processMouseClicks();
      simulate(elapsedTime);
    }

    virtual void render()
    {
      const float width = static_cast<float>(mRenderGrid.getWidth() * mRenderGrid.getCellSize());
      const float height = static_cast<float>(mRenderGrid.getHeight() * mRenderGrid.getCellSize());

      mRenderGrid.upload();
      mRenderParticles.upload();

      glm::mat4 viewMatrix(1.0f);
      glm::mat4 projMatrix = glm::ortho(0.0f, width, 0.0f, height);
      
      mColouredVertexProgram.use();
      mColouredVertexProgram.setUniformMatrix4fv("view", glm::value_ptr(viewMatrix));
      mColouredVertexProgram.setUniformMatrix4fv("projection", glm::value_ptr(projMatrix));

      mRenderGrid.render();
      mRenderParticles.render();
    }

  private:
    void keyPressedCallback(int key)
    {
      switch (key)
      {
      case 'p':
        mDrawParticles = !mDrawParticles;
        break;
      case 'c':
        clear();
        break;
      }
    }

    void setSolidCell(int i, int j)
    {
      mFlipSolver.setCellType(i, j, CELL_SOLID);
      mRenderGrid.setColour(i, j, kColourSolid);
    }

    void setFluidCell(int i, int j, const glm::vec2& vel)
    {
      addParticle(i, j, vel);
      mRenderGrid.setColour(i, j, kColourFluid);
    }

    void addParticle(int i, int j, const glm::vec2& vel)
    {
      const float iF = static_cast<float>(i);
      const float jF = static_cast<float>(j);
      const float particlesDimX = 2.0f;
      const float particlesDimY = 2.0f;

      for (float rx = 0; rx < particlesDimX; rx++)
      for (float ry = 0; ry < particlesDimY; ry++)
      {
        // Randomly jitter particles

        const float x = (iF + (rx + 0.1f + 0.8f * mUniformDist(mMersenneTwister)) / particlesDimX) * kSolverGridSize;
        const float y = (jF + (ry + 0.1f + 0.8f * mUniformDist(mMersenneTwister)) / particlesDimY) * kSolverGridSize;

        mFlipSolver.particles.addParticle(glm::vec2(x, y), vel);
        mFlipSolver.setCellType(i, j, CELL_FLUID);
      }
    }

    void clear()
    {
      const int gridWidth = mRenderGrid.getWidth();
      const int gridHeight = mRenderGrid.getHeight();

      mFlipSolver.particles.clearParticles();

      for (int j = 0; j < gridHeight; j++)
      for (int i = 0; i < gridWidth; i++)
      {
        mFlipSolver.u(i, j) = 0.0f;
        mFlipSolver.v(i, j) = 0.0f;

        if (i == (gridWidth - 1))
        {
          mFlipSolver.u(i + 1, j) = 0.0f;
        }

        if (j == (gridHeight - 1))
        {
          mFlipSolver.v(i, j + 1) = 0.0f;
        }

        if ((i > 0) && (i < gridWidth - 1) && (j > 0) && (j < gridHeight - 1))
        {
          mFlipSolver.setCellType(i, j, CELL_AIR);
        }
      }
    }

    void processMouseClicks()
    {
      const bool isMouseRightClicked = mMouseProvider->isRightClicked();
      const bool isMouseLeftClicked = mMouseProvider->isLeftClicked();

      if (isMouseRightClicked || isMouseLeftClicked)
      {
        const glm::ivec2 mousePos = mMouseProvider->getMousePosition();
        const int gridWidth = mRenderGrid.getWidth();
        const int gridHeight = mRenderGrid.getHeight();
        const int cellSize = mRenderGrid.getCellSize();

        const int iMouse = mousePos.x / cellSize;
        const int jMouse = ((gridHeight * cellSize) - mousePos.y) / cellSize;

        for (int j = 0; j < gridHeight; j++)
        for (int i = 0; i < gridWidth; i++)
        {
          if ((abs(i - iMouse) <= 3) && (abs(j - jMouse) <= 3) && (mFlipSolver.getCellType(i, j) == CELL_AIR))
          {
            if (isMouseRightClicked)
            {
              setSolidCell(i, j);
            }
            else
            {
              setFluidCell(i, j, glm::vec2(0.0f, -2.0f));
            }
          }
        }
      }
    }

    void simulate(double elapsedTime)
    {
      double dt;
      double t = 0;
      bool finished = false;

      while (!finished)
      {
        dt = 2. * mFlipSolver.timeStepCFL();

        if (t + dt >= elapsedTime)
        {
          dt = elapsedTime - t;
          finished = true;
        }
        else if (t + 1.5 * dt >= elapsedTime)
        {
          dt = 0.5 * (elapsedTime - t);
        }

        mFlipSolver.fluidStepFlip(static_cast<float>(dt));
        t += dt;
      }

  //		dt = mFlipSolver.timeStepCFL();
  //		mFlipSolver.fluidStepFlip(dt);
    }

  private:
    FLIPSolver2D mFlipSolver;
    RenderGrid2D mRenderGrid;
    RenderParticles mRenderParticles;
    mk::renderer::gl::ShaderProgram mColouredVertexProgram;
    std::random_device mRandomDevice;
    std::mt19937 mMersenneTwister;
    std::uniform_real_distribution<float> mUniformDist;
    bool mDrawParticles;
  };
}

int main(int argc, char** argv)
{
  FLIPDemo2D flipDemo2D("Fluid Fun", kGridWidth, kGridHeight, kRenderGridCellSize);
  flipDemo2D.doRenderLoop(kFramerate);

  return 0;
}
