#include "FLIPSolver2D.hpp"

#include <algorithm>

namespace mk
{
  namespace physics
  {
    namespace
    {
      const float kEpsilon = 1e-16f;
      const float kGravity = 9.81f;
      const int kPcgMaxIterations = 100;
      const double kPcgTolerance = 1e-4;
      const double kPcgEpsilon = 1e-6;
    }

    FLIPSolver2D::Particles::Particles()
    : np(0)
    {
    }

    void FLIPSolver2D::Particles::addParticle(const glm::fvec2& x_, const glm::fvec2& u_)
    {
      x.push_back(x_);
      u.push_back(u_);

      ++np;
    }

    void FLIPSolver2D::Particles::clearParticles()
    {
      x.clear();
      u.clear();

      np = 0;
    }

    FLIPSolver2D::FLIPSolver2D(int grid_width, int grid_height, float dx)
    : mGridWidth(grid_width),
      mGridHeight(grid_height),
      mDx(dx),
      mOverDx(1.0f / dx),
      mBoundaryVelocity(0.0f),
      mPicFlipFactor(1.0f),
      mVelX((grid_width + 1) * grid_height),
      mVelY(grid_width * (grid_height + 1)),
      mDeltaVelX(mVelX.size()),
      mDeltaVelY(mVelY.size()),
      mWeightSum((grid_width + 1) * (grid_height + 1)),
      mPhi(grid_width * grid_height),
      mCellType(grid_width * grid_height),
      mCellTypeAux(grid_width * grid_height),
      mP(grid_width * grid_height),
      mR(grid_width * grid_height),
      mS(grid_width * grid_height),
      mZ(grid_width * grid_height),
      mAux(grid_width * grid_height),
      mRhs(grid_width * grid_height),
      mPrecond(grid_width * grid_height),
      mCoefDiag(grid_width * grid_height),
      mCoefPlusI(grid_width * grid_height),
      mCoefPlusJ(grid_width * grid_height)
    {
      std::fill(mVelX.begin(), mVelX.end(), 0.0f);
      std::fill(mVelY.begin(), mVelY.end(), 0.0f);
      std::fill(mDeltaVelX.begin(), mDeltaVelX.end(), 0.0f);
      std::fill(mDeltaVelY.begin(), mDeltaVelY.end(), 0.0f);

      std::fill(mCellType.begin(), mCellType.end(), kCellTypeFluid);

      // Solid square surrounding the whole area and rectangle in the middle

      for (int i = 0; i < grid_width; i++)
      {
        mCellType[ix(i, 0)] = kCellTypeSolid;
        mCellType[ix(i, grid_height - 1)] = kCellTypeSolid;
      }

      for (int j = 0; j < grid_height; j++)
      {
        mCellType[ix(0, j)] = kCellTypeSolid;
        mCellType[ix(grid_width - 1, j)] = kCellTypeSolid;
      }
    }

    void FLIPSolver2D::setBoundaryVel(const glm::fvec2& vel)
    {
      mBoundaryVelocity = vel;
    }

    void FLIPSolver2D::simulate(float dt)
    {
      advectParticles(dt);
      particlesToGrid();
      storeVel();
      applyForce(dt, 0.0f, -kGravity);
      computeGridPhi();
      extrapolateVel();
      setBoundary();
      project(dt);
      extrapolateVel();
      subtractVel();
      gridToParticles();
    }

    float FLIPSolver2D::getPressure(int i, int j)
    {
      return static_cast<float>(mP[ix(i, j)]);
    }

    glm::fvec2 FLIPSolver2D::getVelocity(int i, int j)
    {
      return glm::fvec2((u(i, j) + u(i + 1, j)) * 0.5f, (v(i, j) + v(i, j + 1)) * 0.5f);
    }

    glm::fvec2 FLIPSolver2D::getVelocity(float i, float j)
    {
      return glm::fvec2(uVel(i, j), vVel(i, j));
    }

    CellType FLIPSolver2D::getCellType(int i, int j) const
    {
      return mCellType[ix(i, j)];
    }

    void FLIPSolver2D::setCellType(int i, int j, CellType type)
    {
      mCellType[ix(i, j)] = type;
    }

    void FLIPSolver2D::setPicFlipFactor(float factor)
    {
      mPicFlipFactor = glm::clamp(factor, 0.0f, 1.0f);
    }

    void FLIPSolver2D::gridHasChanged(int i, int j)
    {
      for (int p = 0; p < mParticles.np; p++)
      {
        const float i_p = mParticles.x[p].x * mOverDx;
        const float j_p = mParticles.x[p].y * mOverDx;

        if ((std::fabs(i_p - i) <= 1) && (std::fabs(j_p - j) <= 1))
        {
          mParticles.u[p].x = uVel(i_p, j_p);
          mParticles.u[p].y = vVel(i_p, j_p);
        }
      }
    }

    float FLIPSolver2D::timeStep()
    {
      float max_vel = 0.0f;

      for (int j = 0; j < mGridHeight; j++)
      for (int i = 0; i < mGridWidth; i++)
      {
        const float vel = glm::length(getVelocity(i, j));
        max_vel = std::max(max_vel, vel);
      }

      //return ( 5.0f * dx ) / ( max_vel + sqrt( 5.0f * dx * g ));

      const float max_v = std::max(mDx * kGravity, max_vel);

      return mDx / sqrt(std::max(max_v, kEpsilon));
    }

    float FLIPSolver2D::timeStepCFL()
    {
      float max_u = 0.0f;
      float max_v = 0.0f;

      for (std::size_t i = 0; i < mVelX.size(); i++)
      {
        max_u = std::max(max_u, std::fabs(mVelX[i]));
      }

      for (std::size_t i = 0; i < mVelY.size(); i++)
      {
        max_v = std::max(max_v, std::fabs(mVelY[i]));
      }

      const float max_vel = std::max(kGravity * mDx, max_u * max_u + max_v * max_v);

      return mDx / sqrt(std::max(max_vel, kEpsilon));
    }

    void FLIPSolver2D::swapVel()
    {
      std::swap(mVelX, mDeltaVelX);
      std::swap(mVelY, mDeltaVelY);
    }

    float FLIPSolver2D::uVel(float i, float j)
    {
      i = glm::clamp(i, 0.0f, static_cast<float>(mGridWidth));
      j = glm::clamp(j, 0.0f, static_cast<float>(mGridHeight));

      if ((j < 0.5f) || (j > (mGridHeight - 0.5f)))
      {
        int i0 = static_cast<int>(i);
        int i1 = i0 + 1;
        int jj = (j <= 0.5f) ? 0 : (mGridHeight - 1);

        float t0 = i - static_cast<float>(i0);
        float t1 = 1.0f - t0;

        return t1 * u(i0, jj) + t0 * u(i1, jj);
      }
      else
      {
        int i0 = static_cast<int>(i);
        int j0 = static_cast<int>(j - 0.5f);
        int i1 = i0 + 1;
        int j1 = j0 + 1;

        float t0 = i - static_cast<float>(i0);
        float t1 = 1.0f - t0;
        float s0 = (j - 0.5f) - static_cast<float>(j0);
        float s1 = 1.0f - s0;

        return	s1 * (t1 * u(i0, j0) + t0 * u(i1, j0)) + s0 * (t1 * u(i0, j1) + t0 * u(i1, j1));
      }
    }

    float FLIPSolver2D::vVel(float i, float j)
    {
      i = glm::clamp(i, 0.0f, static_cast<float>(mGridWidth));
      j = glm::clamp(j, 0.0f, static_cast<float>(mGridHeight));

      if ((i < 0.5f) || (i > (mGridWidth - 0.5f)))
      {
        int ii = (i <= 0.5f) ? 0 : (mGridWidth - 1);
        int j0 = static_cast<int>(j);
        int j1 = j0 + 1;

        float t0 = j - static_cast<float>(j0);
        float t1 = 1.0f - t0;

        return t1 * v(ii, j0) + t0 * v(ii, j1);
      }
      else
      {
        int i0 = static_cast<int>(i - 0.5f);
        int j0 = static_cast<int>(j);
        int i1 = i0 + 1;
        int j1 = j0 + 1;

        float t0 = (i - 0.5f) - static_cast<float>(i0);
        float t1 = 1.0f - t0;
        float s0 = j - static_cast<float>(j0);
        float s1 = 1.0f - s0;

        return	s1 * (t1 * v(i0, j0) + t0 * v(i1, j0)) + s0 * (t1 * v(i0, j1) + t0 * v(i1, j1));
      }
    }

    int FLIPSolver2D::uIndex_x(float x, float& wx)
    {
      float x_ = glm::clamp(x * mOverDx, 0.0f, static_cast<float>(mGridWidth) - mDx * 1e-5f);

      int i = static_cast<int>(x_);
      wx = x_ - i;

      return i;
    }

    int FLIPSolver2D::uIndex_y(float y, float& wy)
    {
      float y_ = y * mOverDx - 0.5f;

      int j = static_cast<int>(y_);

      if (j < 0)
      {
        wy = 0.0;
        return 0;
      }

      if (j > (mGridHeight - 2))
      {
        wy = 1.0f;
        return mGridHeight - 2;
      }

      wy = y_ - j;

      return j;
    }

    int FLIPSolver2D::vIndex_x(float x, float& wx)
    {
      float x_ = x * mOverDx - 0.5f;

      int i = static_cast<int>(x_);

      if (i < 0)
      {
        wx = 0.0;
        return 0;
      }

      if (i > (mGridWidth - 2))
      {
        wx = 1.0f;
        return mGridWidth - 2;
      }

      wx = x_ - i;

      return i;
    }

    int FLIPSolver2D::vIndex_y(float y, float& wy)
    {
      float y_ = glm::clamp(y * mOverDx, 0.0f, static_cast<float>(mGridHeight) - mDx * 1e-5f);

      int j = static_cast<int>(y_);
      wy = y_ - j;

      return j;
    }

    void FLIPSolver2D::applyForce(float dt, float ax, float ay)
    {
      for (int j = 0; j < mGridHeight; ++j)
      for (int i = 0; i < mGridWidth; ++i)
      {
        if (mCellType[ix(i, j)] == kCellTypeFluid)
        {
          if (ax != 0.0f)
          {
            u(i, j) += (dt * ax);

            if (i == (mGridWidth - 1))
            {
              u(i + 1, j) += (dt * ax);
            }
          }

          if (ay != 0.0f)
          {
            v(i, j) += (dt * ay);

            if (j == (mGridHeight - 1))
            {
              v(i, j + 1) += (dt * ay);
            }
          }
        }
      }
    }

    void FLIPSolver2D::setBoundary()
    {
      for (int j = 0; j < mGridHeight; ++j)
      for (int i = 0; i < mGridWidth; ++i)
      {
        if (mCellType[ix(i, j)] == kCellTypeSolid)
        {
          u(i, j) = mBoundaryVelocity.x;
          u(i + 1, j) = mBoundaryVelocity.x;

          v(i, j) = mBoundaryVelocity.y;
          v(i, j + 1) = mBoundaryVelocity.y;
        }
      }
    }

    void FLIPSolver2D::checkBoundary(float i_init_, float j_init_, float& i_end_, float& j_end_)
    {
      i_end_ = glm::clamp(i_end_, 0.0f, mGridWidth - 1.0f);
      j_end_ = glm::clamp(j_end_, 0.0f, mGridHeight - 1.0f);

      const int i_init = static_cast<int>(i_init_);
      const int j_init = static_cast<int>(j_init_);
      const int i_end = static_cast<int>(i_end_);
      const int j_end = static_cast<int>(j_end_);

      if ((i_init == i_end) && (j_init == j_end))
      {
        return;
      }

      if (mCellType[ix(i_end, j_end)] == kCellTypeSolid)
      {
        int i_sub = i_init - i_end;
        int j_sub = j_init - j_end;

        if ((i_sub != 0) || (j_sub != 0))
        {
          if (mCellType[ix(i_init + i_sub, j_init)] != kCellTypeSolid)
          {
            j_sub = 0;
          }
          else if (mCellType[ix(i_init, j_init + j_sub)] != kCellTypeSolid)
          {
            i_sub = 0;
          }
        }

        if (i_sub != 0)
        {
          i_end_ += ((i_end_ - i_end) + 0.1f) * i_sub;
        }
        if (j_sub != 0)
        {
          j_end_ += ((j_end_ - j_end) + 0.1f) * j_sub;
        }
      }
    }

    void FLIPSolver2D::advectParticles(float dt)
    {
      // Advect particles with five substeps of ( 0.2f * dt ) timestep

      for (int r = 0; r < 5; r++)
      for (int p = 0; p < mParticles.np; p++)
      {
        const float i_p = mParticles.x[p].x * mOverDx;
        const float j_p = mParticles.x[p].y * mOverDx;

        float i_mid = (i_p * mDx + uVel(i_p, j_p)	* dt * 0.2f * 0.5f) * mOverDx;
        float j_mid = (j_p * mDx + vVel(i_p, j_p)	* dt * 0.2f * 0.5f) * mOverDx;

        checkBoundary(i_p, j_p, i_mid, j_mid);

        float i_final = (i_p * mDx + uVel(i_mid, j_mid) * dt * 0.2f) * mOverDx;
        float j_final = (j_p * mDx + vVel(i_mid, j_mid) * dt * 0.2f) * mOverDx;

        checkBoundary(i_p, j_p, i_final, j_final);

        mParticles.x[p].x = i_final * mDx;
        mParticles.x[p].y = j_final * mDx;
      }
    }

    void FLIPSolver2D::particlesToGrid()
    {
      // Update u component

      std::fill(mVelX.begin(), mVelX.end(), 0.0f);
      std::fill(mWeightSum.begin(), mWeightSum.end(), 0.0f);

      for (int p = 0; p < mParticles.np; p++)
      {
        float wx, wy, w;

        int i = uIndex_x(mParticles.x[p].x, wx);
        int j = uIndex_y(mParticles.x[p].y, wy);

        w = (1.0f - wx) * (1.0f - wy);
        u(i, j) += mParticles.u[p].x * w;
        mWeightSum[ix_(i, j)] += w;

        w = wx * (1.0f - wy);
        u(i + 1, j) += mParticles.u[p].x * w;
        mWeightSum[ix_(i + 1, j)] += w;

        w = (1.0f - wx) * wy;
        u(i, j + 1) += mParticles.u[p].x * w;
        mWeightSum[ix_(i, j + 1)] += w;

        w = wx * wy;
        u(i + 1, j + 1) += mParticles.u[p].x * w;
        mWeightSum[ix_(i + 1, j + 1)] += w;
      }

      for (int j = 0; j < mGridHeight; j++)
      for (int i = 0; i < mGridWidth + 1; i++)
      {
        int ix = ix_(i, j);

        if (mWeightSum[ix] != 0)
        {
          u(i, j) /= mWeightSum[ix];
        }
      }

      // Update v component

      std::fill(mVelY.begin(), mVelY.end(), 0.0f);
      std::fill(mWeightSum.begin(), mWeightSum.end(), 0.0f);

      for (int p = 0; p < mParticles.np; p++)
      {
        float wx, wy, w;

        int i = vIndex_x(mParticles.x[p].x, wx);
        int j = vIndex_y(mParticles.x[p].y, wy);

        w = (1.0f - wx) * (1.0f - wy);
        v(i, j) += mParticles.u[p].y * w;
        mWeightSum[ix(i, j)] += w;

        w = wx * (1.0f - wy);
        v(i + 1, j) += mParticles.u[p].y * w;
        mWeightSum[ix(i + 1, j)] += w;

        w = (1.0f - wx) * wy;
        v(i, j + 1) += mParticles.u[p].y * w;
        mWeightSum[ix(i, j + 1)] += w;

        w = wx * wy;
        v(i + 1, j + 1) += mParticles.u[p].y * w;
        mWeightSum[ix(i + 1, j + 1)] += w;
      }

      for (int j = 0; j < mGridHeight + 1; j++)
      for (int i = 0; i < mGridWidth; i++)
      {
        int ix_ = ix(i, j);

        if (mWeightSum[ix_] != 0)
        {
          v(i, j) /= mWeightSum[ix_];
        }
      }

      // Mark fluid particles

      for (int j = 0; j < mGridHeight; j++)
      for (int i = 0; i < mGridWidth; i++)
      {
        int ix_ = ix(i, j);

        if (mCellType[ix_] != kCellTypeSolid)
        {
          mCellType[ix_] = kCellTypeAir;
        }
      }

      for (int p = 0; p < mParticles.np; p++)
      {
        float wx, wy;

        int i = uIndex_x(mParticles.x[p].x, wx);
        int j = vIndex_y(mParticles.x[p].y, wy);

        int ix_ = ix(i, j);

        if (mCellType[ix_] != kCellTypeSolid)
        {
          mCellType[ix_] = kCellTypeFluid;
        }
      }

      for (int j = 0; j < mGridHeight; j++)
      for (int i = 0; i < mGridWidth; i++)
      {
        int ix_ = ix(i, j);

        mCellTypeAux[ix_] = kCellTypeAir;

        if (mCellType[ix_] == kCellTypeAir)
        {
          int adjacent_fluid_cells = 0;
          int adjacent_solid_cells = 0;

          if (i > 0 && (mCellType[ix(i - 1, j)] == kCellTypeFluid))
            ++adjacent_fluid_cells;
          if (i > 0 && (mCellType[ix(i - 1, j)] == kCellTypeSolid))
            ++adjacent_solid_cells;

          if (i < mGridWidth - 1 && (mCellType[ix(i + 1, j)] == kCellTypeFluid))
            ++adjacent_fluid_cells;
          if (i < mGridWidth - 1 && (mCellType[ix(i + 1, j)] == kCellTypeSolid))
            ++adjacent_solid_cells;

          if (j > 0 && (mCellType[ix(i, j - 1)] == kCellTypeFluid))
            ++adjacent_fluid_cells;
          if (j > 0 && (mCellType[ix(i, j - 1)] == kCellTypeSolid))
            ++adjacent_solid_cells;

          if (j < mGridHeight - 1 && (mCellType[ix(i, j + 1)] == kCellTypeFluid))
            ++adjacent_fluid_cells;
          if (j < mGridHeight - 1 && (mCellType[ix(i, j + 1)] == kCellTypeSolid))
            ++adjacent_solid_cells;

          if (adjacent_fluid_cells >= 3 || (adjacent_fluid_cells + adjacent_solid_cells >= 4))
            mCellTypeAux[ix_] = kCellTypeFluid;
        }
      }

      for (int j = 0; j < mGridHeight; j++)
      for (int i = 0; i < mGridWidth; i++)
      {
        int ix_ = ix(i, j);

        if (mCellTypeAux[ix_] == kCellTypeFluid)
        {
          mCellType[ix_] = kCellTypeFluid;
        }
      }
    }

    void FLIPSolver2D::gridToParticles()
    {
      for (int p = 0; p < mParticles.np; p++)
      {
        float i_p = mParticles.x[p].x * mOverDx;
        float j_p = mParticles.x[p].y * mOverDx;

        // PIC

        float u_pic = uVel(i_p, j_p);
        float v_pic = vVel(i_p, j_p);

        // FLIP

        swapVel();

        float u_flip = mParticles.u[p].x + uVel(i_p, j_p);
        float v_flip = mParticles.u[p].y + vVel(i_p, j_p);

        swapVel();

        // Lerp between both to control numerical viscosity

        mParticles.u[p].x = mPicFlipFactor * u_pic + (1.0f - mPicFlipFactor) * u_flip;
        mParticles.u[p].y = mPicFlipFactor * v_pic + (1.0f - mPicFlipFactor) * v_flip;
      }

      for (int j = 0; j < mGridHeight; j++)
      for (int i = 0; i < mGridWidth; i++)
      {
        int ix_ = ix(i, j);

        mCellTypeAux[ix_] = kCellTypeAir;

        if (mCellType[ix_] == kCellTypeAir)
        {
          int adjacent_fluid_cells = 0;
          int adjacent_solid_cells = 0;

          if (i > 0 && (mCellType[ix(i - 1, j)] == kCellTypeFluid))
            ++adjacent_fluid_cells;
          if (i > 0 && (mCellType[ix(i - 1, j)] == kCellTypeSolid))
            ++adjacent_solid_cells;

          if (i < mGridWidth - 1 && (mCellType[ix(i + 1, j)] == kCellTypeFluid))
            ++adjacent_fluid_cells;
          if (i < mGridWidth - 1 && (mCellType[ix(i + 1, j)] == kCellTypeSolid))
            ++adjacent_solid_cells;

          if (j > 0 && (mCellType[ix(i, j - 1)] == kCellTypeFluid))
            ++adjacent_fluid_cells;
          if (j > 0 && (mCellType[ix(i, j - 1)] == kCellTypeSolid))
            ++adjacent_solid_cells;

          if (j < mGridHeight - 1 && (mCellType[ix(i, j + 1)] == kCellTypeFluid))
            ++adjacent_fluid_cells;
          if (j < mGridHeight - 1 && (mCellType[ix(i, j + 1)] == kCellTypeSolid))
            ++adjacent_solid_cells;

          if (adjacent_fluid_cells >= 3 || (adjacent_fluid_cells + adjacent_solid_cells >= 4))
            mCellTypeAux[ix_] = kCellTypeFluid;
        }
      }

      for (int j = 0; j < mGridHeight; j++)
      for (int i = 0; i < mGridWidth; i++)
      {
        int ix_ = ix(i, j);

        if (mCellTypeAux[ix_] == kCellTypeFluid)
          mCellType[ix_] = kCellTypeFluid;
      }
    }

    void FLIPSolver2D::storeVel()
    {
      mDeltaVelX.assign(mVelX.begin(), mVelX.end());
      mDeltaVelY.assign(mVelY.begin(), mVelY.end());
    }

    void FLIPSolver2D::subtractVel()
    {
      for (std::size_t i = 0; i < mVelX.size(); i++)
      {
        mDeltaVelX[i] = mVelX[i] - mDeltaVelX[i];
      }

      for (std::size_t i = 0; i < mVelY.size(); i++)
      {
        mDeltaVelY[i] = mVelY[i] - mDeltaVelY[i];
      }
    }

    float FLIPSolver2D::computePhi(float a, float b, float current)
    {
      float dif = a - b;

      if (std::fabs(dif) >= mDx)
      {
        return std::min(a, b) + mDx;
      }

      return std::min(current, (a + b + sqrt(2 * mDx * mDx - dif * dif)) / 2.0f);

      /*float phi_ = min_( a, b ) + 1.0f;

      if( phi_ > max_( a, b ))
        phi_ = ( a + b + ( float )sqrt( 2.0 - (( a - b ) * ( a - b )))) / 2.0f;

      if( phi_ < current )
        return phi_;

      return current;*/
    }

    void FLIPSolver2D::computeGridPhi()
    {
      // Initialize distance function

      float max_dim = static_cast<float>(mGridWidth + mGridHeight + 2);

      for (int j = 0; j < mGridHeight; j++)
      for (int i = 0; i < mGridWidth; i++)
      {
        int ix_ = ix(i, j);

        if (mCellType[ix_] == kCellTypeFluid)
        {
          mPhi[ix_] = -0.5f;
        }
        else
        {
          mPhi[ix_] = max_dim;
        }
      }

      for (int r = 0; r < 2; r++)
      {
        // Sweep in all directions

        for (int j = 1; j < mGridHeight; j++)
        for (int i = 1; i < mGridWidth; i++)
        {
          int ix_ = ix(i, j);

          if (mCellType[ix_] != kCellTypeFluid)
          {
            mPhi[ix_] = computePhi(mPhi[ix(i - 1, j)], mPhi[ix(i, j - 1)], mPhi[ix_]);
          }
        }

        for (int j = mGridHeight - 2; j >= 0; j--)
        for (int i = 1; i < mGridWidth; i++)
        {
          int ix_ = ix(i, j);

          if (mCellType[ix_] != kCellTypeFluid)
          {
            mPhi[ix_] = computePhi(mPhi[ix(i - 1, j)], mPhi[ix(i, j + 1)], mPhi[ix_]);
          }
        }

        for (int j = 1; j < mGridHeight; j++)
        for (int i = mGridWidth - 2; i >= 0; i--)
        {
          int ix_ = ix(i, j);

          if (mCellType[ix_] != kCellTypeFluid)
          {
            mPhi[ix_] = computePhi(mPhi[ix(i + 1, j)], mPhi[ix(i, j - 1)], mPhi[ix_]);
          }
        }

        for (int j = mGridHeight - 2; j >= 0; j--)
        for (int i = mGridWidth - 2; i >= 0; i--)
        {
          int ix_ = ix(i, j);

          if (mCellType[ix_] != kCellTypeFluid)
          {
            mPhi[ix_] = computePhi(mPhi[ix(i + 1, j)], mPhi[ix(i, j + 1)], mPhi[ix_]);
          }
        }
      }
    }

    void FLIPSolver2D::sweepU(int i0, int i1, int j0, int j1)
    {
      int di = (i0 < i1) ? 1 : -1;
      int dj = (j0 < j1) ? 1 : -1;

      float dp, dq, alpha;

      for (int j = j0; j != j1; j += dj)
      for (int i = i0; i != i1; i += di)
      {
        if ((mCellType[ix(i - 1, j)] == kCellTypeAir) && (mCellType[ix(i, j)] == kCellTypeAir))
        {
          dp = di * (mPhi[ix(i, j)] - mPhi[ix(i - 1, j)]);

          if (dp < 0)
          {
            continue;
          }

          dq = 0.5f * (mPhi[ix(i - 1, j)] + mPhi[ix(i, j)] - mPhi[ix(i - 1, j - dj)] - mPhi[ix(i, j - dj)]);

          if (dq < 0)
          {
            continue;
          }

          if ((dp + dq) < kEpsilon)
          {
            alpha = 0.5f;
          }
          else
          {
            alpha = dp / (dp + dq);
          }

          u(i, j) = alpha * u(i - di, j) + (1.0f - alpha) * u(i, j - dj);
        }
      }
    }

    void FLIPSolver2D::sweepV(int i0, int i1, int j0, int j1)
    {
      int di = (i0 < i1) ? 1 : -1;
      int dj = (j0 < j1) ? 1 : -1;

      float dp, dq, alpha;

      for (int j = j0; j != j1; j += dj)
      for (int i = i0; i != i1; i += di)
      {
        if ((mCellType[ix(i, j - 1)] == kCellTypeAir) && (mCellType[ix(i, j)] == kCellTypeAir))
        {
          dq = dj * (mPhi[ix(i, j)] - mPhi[ix(i, j - 1)]);

          if (dq < 0)
          {
            continue;
          }

          dp = 0.5f * (mPhi[ix(i, j - 1)] + mPhi[ix(i, j)] - mPhi[ix(i - di, j - 1)] - mPhi[ix(i - di, j)]);

          if (dp < 0)
          {
            continue;
          }

          if (std::fabs(dp + dq) < kEpsilon)
          {
            alpha = 0.5f;
          }
          else
          {
            alpha = dp / (dp + dq);
          }

          v(i, j) = alpha * v(i - di, j) + (1.0f - alpha) * v(i, j - dj);
        }
      }
    }

    void FLIPSolver2D::extrapolateVel()
    {
      for (int r = 0; r < 4; r++)
      {
        sweepU(1, mGridWidth, 1, mGridHeight - 1);
        sweepU(1, mGridWidth, mGridHeight - 2, 0);
        sweepU(mGridWidth - 1, 0, 1, mGridHeight - 1);
        sweepU(mGridWidth - 1, 0, mGridHeight - 2, 0);

        for (int i = 0; i < mGridWidth + 1; i++)
        {
          u(i, 0) = u(i, 1);
          u(i, mGridHeight - 1) = u(i, mGridHeight - 2);
        }
        for (int j = 0; j < mGridHeight; j++)
        {
          u(0, j) = u(1, j);
          u(mGridWidth, j) = u(mGridWidth - 1, j);
        }

        sweepV(1, mGridWidth - 1, 1, mGridHeight);
        sweepV(1, mGridWidth - 1, mGridHeight - 1, 0);
        sweepV(mGridWidth - 2, 0, 1, mGridHeight);
        sweepV(mGridWidth - 2, 0, mGridHeight - 1, 0);

        for (int i = 0; i < mGridWidth; i++)
        {
          v(i, 0) = v(i, 1);
          v(i, mGridHeight) = v(i, mGridHeight - 1);
        }

        for (int j = 0; j < mGridHeight + 1; j++)
        {
          v(0, j) = v(1, j);
          v(mGridWidth - 1, j) = v(mGridWidth - 2, j);
        }
      }
    }

    void FLIPSolver2D::project(float dt)
    {
      // Set the right hand side of the equation system

      mRhs.clear();

      for (int j = 0; j < mGridHeight; j++)
      for (int i = 0; i < mGridWidth; i++)
      {
        int ix_ = i + j * mGridWidth;

        if (mCellType[ix_] == kCellTypeFluid)
        {
          mRhs[ix_] = (u(i + 1, j) - u(i, j) + v(i, j + 1) - v(i, j));

          if ((i > 0) && (mCellType[ix(i - 1, j)] == kCellTypeSolid))
          {
            mRhs[ix_] -= (mBoundaryVelocity.x - u(i, j));
          }
          if ((i < (mGridWidth - 1)) && (mCellType[ix(i + 1, j)] == kCellTypeSolid))
          {
            mRhs[ix_] -= (u(i + 1, j) - mBoundaryVelocity.x);
          }
          if ((j > 0) && (mCellType[ix(i, j - 1)] == kCellTypeSolid))
          {
            mRhs[ix_] -= (mBoundaryVelocity.y - v(i, j));
          }
          if ((j < (mGridHeight - 1)) && (mCellType[ix(i, j + 1)] == kCellTypeSolid))
          {
            mRhs[ix_] -= (v(i, j + 1) - mBoundaryVelocity.y);
          }
        }
      }

      // Set the coefficients

      mCoefDiag.clear();
      mCoefPlusI.clear();
      mCoefPlusJ.clear();

      for (int j = 0; j < mGridHeight; j++)
      for (int i = 0; i < mGridWidth; i++)
      {
        if (mCellType[ix(i, j)] == kCellTypeFluid)
        {
          if (mCellType[ix(i + 1, j)] != kCellTypeSolid)
          {
            mCoefDiag[ix(i, j)] += 1.0f;

            if (mCellType[ix(i + 1, j)] == kCellTypeFluid)
            {
              mCoefPlusI[ix(i, j)] = -1.0f;
            }
          }

          if (mCellType[ix(i - 1, j)] != kCellTypeSolid)
          {
            mCoefDiag[ix(i, j)] += 1.0f;
          }

          if (mCellType[ix(i, j + 1)] != kCellTypeSolid)
          {
            mCoefDiag[ix(i, j)] += 1.0f;

            if (mCellType[ix(i, j + 1)] == kCellTypeFluid)
            {
              mCoefPlusJ[ix(i, j)] = -1.0f;
            }
          }

          if (mCellType[ix(i, j - 1)] != kCellTypeSolid)
          {
            mCoefDiag[ix(i, j)] += 1.0f;
          }
        }
      }

      // Solve for pressure with PCG algorithm

      solvePressure();

      // Apply pressure to update velocity

      for (int j = 0; j < mGridHeight; j++)
      for (int i = 0; i < mGridWidth; i++)
      {
        int ix_ = i + j * mGridWidth;

        if (mCellType[ix_] == kCellTypeFluid)
        {
          float pressure = static_cast<float>(mP[ix_]);

          u(i, j) += pressure;
          u(i + 1, j) -= pressure;
          v(i, j) += pressure;
          v(i, j + 1) -= pressure;
        }
      }

      setBoundary();
    }

    void FLIPSolver2D::solvePressure()
    {
      mP.clear();

      double r_max = 0.0;

      for (unsigned int i = 0; i < mRhs.size(); i++)
      {
        if (std::fabs(mRhs[i]) > r_max)
        {
          r_max = std::fabs(mRhs[i]);
        }
      }

      if (r_max == 0.0)
      {
        return;
      }

      mR = mRhs;

      double tolerance = kPcgTolerance * r_max;

      calcPrecond();
      applyPrecond();

      mS = mZ;

      double sigma = inner_prod(mZ, mR);

      if (sigma == 0.0)
      {
        return;
      }

      for (int i = 0; i < kPcgMaxIterations; i++)
      {
        applyA();

        double alpha = sigma / inner_prod(mZ, mS);

        mP += (alpha * mS);
        mR -= (alpha * mZ);

        double error = 0.0f;

        for (int j = 0; j < mGridWidth * mGridHeight; j++)
        {
          double abs_r = std::fabs(mR[j]);

          if (abs_r > error)
          {
            error = abs_r;
          }
        }

        if (error <= tolerance)
        {
          break;
        }

        applyPrecond();

        double sigma_new = inner_prod(mZ, mR);
        double beta = sigma_new / sigma;

        mS = mZ + beta * mS;

        sigma = sigma_new;
      }
    }

    void FLIPSolver2D::calcPrecond()
    {
      double tuning_const = 0.99;
      double safety_const = 0.25;

      mPrecond.clear();

      for (int j = 0; j < mGridHeight; j++)
      for (int i = 0; i < mGridWidth; i++)
      {
        int ix = i + j * mGridWidth;

        if (mCellType[ix] == kCellTypeFluid)
        {
          int ix_plus_i = (i - 1) + j * mGridWidth;
          int ix_plus_j = i + (j - 1) * mGridWidth;

          mPrecond[ix] = mCoefDiag[ix];

          if (i > 0)
          {
            double plus_i = mCoefPlusI[ix_plus_i] * mPrecond[ix_plus_i];
            mPrecond[ix] += (-plus_i * plus_i - tuning_const *
              (mCoefPlusI[ix_plus_i] * mCoefPlusJ[ix_plus_i] * mPrecond[ix_plus_i] * mPrecond[ix_plus_i]));
          }
          if (j > 0)
          {
            double plus_j = mCoefPlusJ[ix_plus_j] * mPrecond[ix_plus_j];
            mPrecond[ix] += (-plus_j * plus_j - tuning_const *
              (mCoefPlusJ[ix_plus_j] * mCoefPlusI[ix_plus_j] * mPrecond[ix_plus_j] * mPrecond[ix_plus_j]));
          }

          if (mPrecond[ix] < safety_const * mCoefDiag[ix])
          {
            mPrecond[ix] = mCoefDiag[ix];
          }

          mPrecond[ix] = 1.0 / sqrt(mPrecond[ix] + kPcgEpsilon);
        }
      }
    }

    void FLIPSolver2D::applyPrecond()
    {
      mAux.clear();

      for (int j = 0; j < mGridHeight; j++)
      for (int i = 0; i < mGridWidth; i++)
      {
        int ix = i + j * mGridWidth;

        if (mCellType[ix] == kCellTypeFluid)
        {
          int ix_plus_i = (i - 1) + j * mGridWidth;
          int ix_plus_j = i + (j - 1) * mGridWidth;

          double t = mR[ix];

          if (i > 0)
          {
            t -= (mCoefPlusI[ix_plus_i] * mPrecond[ix_plus_i] * mAux[ix_plus_i]);
          }
          if (j > 0)
          {
            t -= (mCoefPlusJ[ix_plus_j] * mPrecond[ix_plus_j] * mAux[ix_plus_j]);
          }

          mAux[ix] = t * mPrecond[ix];
        }
      }

      mZ.clear();

      for (int j = mGridHeight; j >= 0; --j)
      for (int i = mGridWidth; i >= 0; --i)
      {
        int ix = i + j * mGridWidth;

        if (mCellType[ix] == kCellTypeFluid)
        {
          int ix_plus_i = (i + 1) + j * mGridWidth;
          int ix_plus_j = i + (j + 1) * mGridWidth;

          double t = mAux[ix];

          if (i < (mGridWidth - 1))
          {
            t -= (mCoefPlusI[ix] * mPrecond[ix] * mZ[ix_plus_i]);
          }
          if (j < (mGridHeight - 1))
          {
            t -= (mCoefPlusJ[ix] * mPrecond[ix] * mZ[ix_plus_j]);
          }

          mZ[ix] = t * mPrecond[ix];
        }
      }
    }

    void FLIPSolver2D::applyA()
    {
      mZ.clear();

      for (int j = 0; j < mGridHeight; j++)
      for (int i = 0; i < mGridWidth; i++)
      {
        int ix = i + j * mGridWidth;

        if (mCellType[ix] == kCellTypeFluid)
        {
          int ix_plus_i = (i + 1) + j * mGridWidth;
          int ix_plus_j = i + (j + 1) * mGridWidth;
          int ix_minus_i = (i - 1) + j * mGridWidth;
          int ix_minus_j = i + (j - 1) * mGridWidth;

          mZ[ix] = mS[ix] * mCoefDiag[ix];

          if (i > 0)
          {
            mZ[ix] += mS[ix_minus_i] * mCoefPlusI[ix_minus_i];
          }
          if (j > 0)
          {
            mZ[ix] += mS[ix_minus_j] * mCoefPlusJ[ix_minus_j];
          }
          if (i < (mGridWidth - 1))
          {
            mZ[ix] += mS[ix_plus_i] * mCoefPlusI[ix];
          }
          if (j < (mGridHeight - 1))
          {
            mZ[ix] += mS[ix_plus_j] * mCoefPlusJ[ix];
          }
        }
      }
    }
  }
}
