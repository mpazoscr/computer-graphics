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

    void FLIPSolver2D::Particles::addParticle(const glm::vec2& x_, const glm::vec2& u_)
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
      p(grid_width * grid_height),
      r(grid_width * grid_height),
      s(grid_width * grid_height),
      z(grid_width * grid_height),
      aux(grid_width * grid_height),
      rhs(grid_width * grid_height),
      precond(grid_width * grid_height),
      coef_diag(grid_width * grid_height),
      coef_plus_i(grid_width * grid_height),
      coef_plus_j(grid_width * grid_height)
    {
      u_size = (grid_width + 1) * grid_height;
      v_size = grid_width * (grid_height + 1);
      p_sum_size = (grid_width + 1) * (grid_height + 1);

      u_ = new float[u_size];
      v_ = new float[v_size];
      du = new float[u_size];
      dv = new float[v_size];
      p_sum_den = new float[p_sum_size];
      phi = new float[grid_width * grid_height];
      cell_type = new short[grid_width * grid_height];
      cell_type_aux = new short[grid_width * grid_height];

      memset(u_, 0, u_size * sizeof(float));
      memset(v_, 0, v_size * sizeof(float));
      memset(du, 0, u_size * sizeof(float));
      memset(dv, 0, v_size * sizeof(float));

      for (int j = 0; j < grid_height; j++)
      for (int i = 0; i < grid_width; i++)
      {
        cell_type[ix(i, j)] = kCellTypeFluid;
      }

      // Solid square surrounding the whole area and rectangle in the middle

      for (int i = 0; i < grid_width; i++)
      {
        cell_type[ix(i, 0)] = kCellTypeSolid;
        cell_type[ix(i, grid_height - 1)] = kCellTypeSolid;
      }

      for (int j = 0; j < grid_height; j++)
      {
        cell_type[ix(0, j)] = kCellTypeSolid;
        cell_type[ix(grid_width - 1, j)] = kCellTypeSolid;
      }
    }

    FLIPSolver2D::~FLIPSolver2D()
    {
      delete[] u_;
      delete[] v_;
      delete[] du;
      delete[] dv;
      delete[] p_sum_den;
      delete[] phi;
      delete[] cell_type;
      delete[] cell_type_aux;
    }

    void FLIPSolver2D::setBoundaryVel(const glm::fvec2& vel)
    {
      mBoundaryVelocity = vel;
    }

    void FLIPSolver2D::fluidStepFlip(float dt)
    {
      advectParticles(dt);
      particlesToGrid();
      storeVel();
      applyForce(dt, 0.0f, -kGravity);
      computeGridPhi();
      extrapolateVel();
      setBoundary();
      project(dt);
      //extrapolateVel();
      subtractVel();
      gridToParticles();
    }

    float FLIPSolver2D::getPressure(int i, int j)
    {
      return static_cast<float>(p[ix(i, j)]);
    }

    glm::vec2 FLIPSolver2D::getVelocity(int i, int j)
    {
      return glm::vec2((u(i, j) + u(i + 1, j)) * 0.5f, (v(i, j) + v(i, j + 1)) * 0.5f);
    }

    glm::vec2 FLIPSolver2D::getVelocity(float i, float j)
    {
      return glm::vec2(uVel(i, j), vVel(i, j));
    }

    short FLIPSolver2D::getCellType(int i, int j) const
    {
      return cell_type[ix(i, j)];
    }

    void FLIPSolver2D::setCellType(int i, int j, short type)
    {
      cell_type[ix(i, j)] = type;
    }

    void FLIPSolver2D::setPicFlipFactor(float factor)
    {
      mPicFlipFactor = glm::clamp(factor, 0.0f, 1.0f);
    }

    void FLIPSolver2D::gridHasChanged(int i, int j)
    {
      for (int p = 0; p < particles.np; p++)
      {
        const float i_p = particles.x[p].x * mOverDx;
        const float j_p = particles.x[p].y * mOverDx;

        if ((std::fabs(i_p - i) <= 1) && (std::fabs(j_p - j) <= 1))
        {
          particles.u[p].x = uVel(i_p, j_p);
          particles.u[p].y = vVel(i_p, j_p);
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

      for (int i = 0; i < u_size; i++)
      {
        max_u = std::max(max_u, std::fabs(u_[i]));
      }

      for (int i = 0; i < v_size; i++)
      {
        max_v = std::max(max_v, std::fabs(v_[i]));
      }

      const float max_vel = std::max(kGravity * mDx, max_u * max_u + max_v * max_v);

      return mDx / sqrt(std::max(max_vel, kEpsilon));
    }

    void FLIPSolver2D::swapVel()
    {
      std::swap(u_, du);
      std::swap(v_, dv);
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

        u_min = std::min(u(i0, jj), u(i1, jj));
        u_max = std::max(u(i0, jj), u(i1, jj));

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

        u_min = std::min(std::min(u(i0, j0), u(i1, j0)), std::min(u(i0, j1), u(i1, j1)));
        u_max = std::max(std::max(u(i0, j0), u(i1, j0)), std::max(u(i0, j1), u(i1, j1)));

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

        v_min = std::min(v(ii, j0), v(ii, j1));
        v_max = std::max(v(ii, j0), v(ii, j1));

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

        v_min = std::min(std::min(v(i0, j0), v(i1, j0)), std::min(v(i0, j1), v(i1, j1)));
        v_max = std::max(std::max(v(i0, j0), v(i1, j0)), std::max(v(i0, j1), v(i1, j1)));

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
        if (cell_type[ix(i, j)] == kCellTypeFluid)
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

            if (j == mGridHeight - 1)
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
        if (cell_type[ix(i, j)] == kCellTypeSolid)
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

      if (cell_type[ix(i_end, j_end)] == kCellTypeSolid)
      {
        int i_sub = i_init - i_end;
        int j_sub = j_init - j_end;

        if ((i_sub != 0) || (j_sub != 0))
        {
          if (cell_type[ix(i_init + i_sub, j_init)] != kCellTypeSolid)
          {
            j_sub = 0;
          }
          else if (cell_type[ix(i_init, j_init + j_sub)] != kCellTypeSolid)
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
      for (int p = 0; p < particles.np; p++)
      {
        const float i_p = particles.x[p].x * mOverDx;
        const float j_p = particles.x[p].y * mOverDx;

        float i_mid = (i_p * mDx + uVel(i_p, j_p)	* dt * 0.2f * 0.5f) * mOverDx;
        float j_mid = (j_p * mDx + vVel(i_p, j_p)	* dt * 0.2f * 0.5f) * mOverDx;

        checkBoundary(i_p, j_p, i_mid, j_mid);

        float i_final = (i_p * mDx + uVel(i_mid, j_mid) * dt * 0.2f) * mOverDx;
        float j_final = (j_p * mDx + vVel(i_mid, j_mid) * dt * 0.2f) * mOverDx;

        checkBoundary(i_p, j_p, i_final, j_final);

        particles.x[p].x = i_final * mDx;
        particles.x[p].y = j_final * mDx;
      }
    }

    void FLIPSolver2D::particlesToGrid()
    {
      // Update u component

      memset(u_, 0, u_size * sizeof(float));
      memset(p_sum_den, 0, p_sum_size * sizeof(float));

      for (int p = 0; p < particles.np; p++)
      {
        float wx, wy, w;

        int i = uIndex_x(particles.x[p].x, wx);
        int j = uIndex_y(particles.x[p].y, wy);

        w = (1.0f - wx) * (1.0f - wy);
        u(i, j) += particles.u[p].x * w;
        p_sum_den[ix_(i, j)] += w;

        w = wx * (1.0f - wy);
        u(i + 1, j) += particles.u[p].x * w;
        p_sum_den[ix_(i + 1, j)] += w;

        w = (1.0f - wx) * wy;
        u(i, j + 1) += particles.u[p].x * w;
        p_sum_den[ix_(i, j + 1)] += w;

        w = wx * wy;
        u(i + 1, j + 1) += particles.u[p].x * w;
        p_sum_den[ix_(i + 1, j + 1)] += w;
      }

      for (int j = 0; j < mGridHeight; j++)
      for (int i = 0; i < mGridWidth + 1; i++)
      {
        int ix = ix_(i, j);

        if (p_sum_den[ix] != 0)
        {
          u(i, j) /= p_sum_den[ix];
        }
      }

      // Update v component

      memset(v_, 0, v_size * sizeof(float));
      memset(p_sum_den, 0, p_sum_size * sizeof(float));

      for (int p = 0; p < particles.np; p++)
      {
        float wx, wy, w;

        int i = vIndex_x(particles.x[p].x, wx);
        int j = vIndex_y(particles.x[p].y, wy);

        w = (1.0f - wx) * (1.0f - wy);
        v(i, j) += particles.u[p].y * w;
        p_sum_den[ix(i, j)] += w;

        w = wx * (1.0f - wy);
        v(i + 1, j) += particles.u[p].y * w;
        p_sum_den[ix(i + 1, j)] += w;

        w = (1.0f - wx) * wy;
        v(i, j + 1) += particles.u[p].y * w;
        p_sum_den[ix(i, j + 1)] += w;

        w = wx * wy;
        v(i + 1, j + 1) += particles.u[p].y * w;
        p_sum_den[ix(i + 1, j + 1)] += w;
      }

      for (int j = 0; j < mGridHeight + 1; j++)
      for (int i = 0; i < mGridWidth; i++)
      {
        int ix_ = ix(i, j);

        if (p_sum_den[ix_] != 0)
        {
          v(i, j) /= p_sum_den[ix_];
        }
      }

      // Mark fluid particles

      for (int j = 0; j < mGridHeight; j++)
      for (int i = 0; i < mGridWidth; i++)
      {
        int ix_ = ix(i, j);

        if (cell_type[ix_] != kCellTypeSolid)
        {
          cell_type[ix_] = kCellTypeAir;
        }
      }

      for (int p = 0; p < particles.np; p++)
      {
        float wx, wy;

        int i = uIndex_x(particles.x[p].x, wx);
        int j = vIndex_y(particles.x[p].y, wy);

        int ix_ = ix(i, j);

        if (cell_type[ix_] != kCellTypeSolid)
        {
          cell_type[ix_] = kCellTypeFluid;
        }
      }

      for (int j = 0; j < mGridHeight; j++)
      for (int i = 0; i < mGridWidth; i++)
      {
        int ix_ = ix(i, j);

        cell_type_aux[ix_] = kCellTypeAir;

        if (cell_type[ix_] == kCellTypeAir)
        {
          int adjacent_fluid_cells = 0;
          int adjacent_solid_cells = 0;

          if (i > 0 && (cell_type[ix(i - 1, j)] == kCellTypeFluid))
            ++adjacent_fluid_cells;
          if (i > 0 && (cell_type[ix(i - 1, j)] == kCellTypeSolid))
            ++adjacent_solid_cells;

          if (i < mGridWidth - 1 && (cell_type[ix(i + 1, j)] == kCellTypeFluid))
            ++adjacent_fluid_cells;
          if (i < mGridWidth - 1 && (cell_type[ix(i + 1, j)] == kCellTypeSolid))
            ++adjacent_solid_cells;

          if (j > 0 && (cell_type[ix(i, j - 1)] == kCellTypeFluid))
            ++adjacent_fluid_cells;
          if (j > 0 && (cell_type[ix(i, j - 1)] == kCellTypeSolid))
            ++adjacent_solid_cells;

          if (j < mGridHeight - 1 && (cell_type[ix(i, j + 1)] == kCellTypeFluid))
            ++adjacent_fluid_cells;
          if (j < mGridHeight - 1 && (cell_type[ix(i, j + 1)] == kCellTypeSolid))
            ++adjacent_solid_cells;

          if (adjacent_fluid_cells >= 3 || (adjacent_fluid_cells + adjacent_solid_cells >= 4))
            cell_type_aux[ix_] = kCellTypeFluid;
        }
      }

      for (int j = 0; j < mGridHeight; j++)
      for (int i = 0; i < mGridWidth; i++)
      {
        int ix_ = ix(i, j);

        if (cell_type_aux[ix_] == kCellTypeFluid)
        {
          cell_type[ix_] = kCellTypeFluid;
        }
      }
    }

    void FLIPSolver2D::gridToParticles()
    {
      for (int p = 0; p < particles.np; p++)
      {
        float i_p = particles.x[p].x * mOverDx;
        float j_p = particles.x[p].y * mOverDx;

        // PIC

        float u_pic = uVel(i_p, j_p);
        float v_pic = vVel(i_p, j_p);

        // FLIP

        swapVel();

        float u_flip = particles.u[p].x + uVel(i_p, j_p);
        float v_flip = particles.u[p].y + vVel(i_p, j_p);

        swapVel();

        // Lerp between both to control numerical viscosity

        particles.u[p].x = mPicFlipFactor * u_pic + (1.0f - mPicFlipFactor) * u_flip;
        particles.u[p].y = mPicFlipFactor * v_pic + (1.0f - mPicFlipFactor) * v_flip;
      }

      for (int j = 0; j < mGridHeight; j++)
      for (int i = 0; i < mGridWidth; i++)
      {
        int ix_ = ix(i, j);

        cell_type_aux[ix_] = kCellTypeAir;

        if (cell_type[ix_] == kCellTypeAir)
        {
          int adjacent_fluid_cells = 0;
          int adjacent_solid_cells = 0;

          if (i > 0 && (cell_type[ix(i - 1, j)] == kCellTypeFluid))
            ++adjacent_fluid_cells;
          if (i > 0 && (cell_type[ix(i - 1, j)] == kCellTypeSolid))
            ++adjacent_solid_cells;

          if (i < mGridWidth - 1 && (cell_type[ix(i + 1, j)] == kCellTypeFluid))
            ++adjacent_fluid_cells;
          if (i < mGridWidth - 1 && (cell_type[ix(i + 1, j)] == kCellTypeSolid))
            ++adjacent_solid_cells;

          if (j > 0 && (cell_type[ix(i, j - 1)] == kCellTypeFluid))
            ++adjacent_fluid_cells;
          if (j > 0 && (cell_type[ix(i, j - 1)] == kCellTypeSolid))
            ++adjacent_solid_cells;

          if (j < mGridHeight - 1 && (cell_type[ix(i, j + 1)] == kCellTypeFluid))
            ++adjacent_fluid_cells;
          if (j < mGridHeight - 1 && (cell_type[ix(i, j + 1)] == kCellTypeSolid))
            ++adjacent_solid_cells;

          if (adjacent_fluid_cells >= 3 || (adjacent_fluid_cells + adjacent_solid_cells >= 4))
            cell_type_aux[ix_] = kCellTypeFluid;
        }
      }

      for (int j = 0; j < mGridHeight; j++)
      for (int i = 0; i < mGridWidth; i++)
      {
        int ix_ = ix(i, j);

        if (cell_type_aux[ix_] == kCellTypeFluid)
          cell_type[ix_] = kCellTypeFluid;
      }
    }

    void FLIPSolver2D::storeVel()
    {
      memcpy(du, u_, u_size * sizeof(float));
      memcpy(dv, v_, v_size * sizeof(float));
    }

    void FLIPSolver2D::subtractVel()
    {
      for (int i = 0; i < u_size; i++)
      {
        du[i] = u_[i] - du[i];
      }

      for (int i = 0; i < v_size; i++)
      {
        dv[i] = v_[i] - dv[i];
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

        if (cell_type[ix_] == kCellTypeFluid)
        {
          phi[ix_] = -0.5f;
        }
        else
        {
          phi[ix_] = max_dim;
        }
      }

      for (int r = 0; r < 2; r++)
      {
        // Sweep in all directions

        for (int j = 1; j < mGridHeight; j++)
        for (int i = 1; i < mGridWidth; i++)
        {
          int ix_ = ix(i, j);

          if (cell_type[ix_] != kCellTypeFluid)
          {
            phi[ix_] = computePhi(phi[ix(i - 1, j)], phi[ix(i, j - 1)], phi[ix_]);
          }
        }

        for (int j = mGridHeight - 2; j >= 0; j--)
        for (int i = 1; i < mGridWidth; i++)
        {
          int ix_ = ix(i, j);

          if (cell_type[ix_] != kCellTypeFluid)
          {
            phi[ix_] = computePhi(phi[ix(i - 1, j)], phi[ix(i, j + 1)], phi[ix_]);
          }
        }

        for (int j = 1; j < mGridHeight; j++)
        for (int i = mGridWidth - 2; i >= 0; i--)
        {
          int ix_ = ix(i, j);

          if (cell_type[ix_] != kCellTypeFluid)
          {
            phi[ix_] = computePhi(phi[ix(i + 1, j)], phi[ix(i, j - 1)], phi[ix_]);
          }
        }

        for (int j = mGridHeight - 2; j >= 0; j--)
        for (int i = mGridWidth - 2; i >= 0; i--)
        {
          int ix_ = ix(i, j);

          if (cell_type[ix_] != kCellTypeFluid)
          {
            phi[ix_] = computePhi(phi[ix(i + 1, j)], phi[ix(i, j + 1)], phi[ix_]);
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
        if (cell_type[ix(i - 1, j)] == kCellTypeAir && cell_type[ix(i, j)] == kCellTypeAir)
        {
          dp = di * (phi[ix(i, j)] - phi[ix(i - 1, j)]);

          if (dp < 0)
          {
            continue;
          }

          dq = 0.5f * (phi[ix(i - 1, j)] + phi[ix(i, j)] - phi[ix(i - 1, j - dj)] - phi[ix(i, j - dj)]);

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
        if (cell_type[ix(i, j - 1)] == kCellTypeAir && cell_type[ix(i, j)] == kCellTypeAir)
        {
          dq = dj * (phi[ix(i, j)] - phi[ix(i, j - 1)]);

          if (dq < 0)
          {
            continue;
          }

          dp = 0.5f * (phi[ix(i, j - 1)] + phi[ix(i, j)] - phi[ix(i - di, j - 1)] - phi[ix(i - di, j)]);

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

      rhs.clear();

      for (int j = 0; j < mGridHeight; j++)
      for (int i = 0; i < mGridWidth; i++)
      {
        int ix_ = i + j * mGridWidth;

        if (cell_type[ix_] == kCellTypeFluid)
        {
          rhs[ix_] = (u(i + 1, j) - u(i, j) + v(i, j + 1) - v(i, j));

          if ((i > 0) && (cell_type[ix(i - 1, j)] == kCellTypeSolid))
          {
            rhs[ix_] -= (mBoundaryVelocity.x - u(i, j));
          }
          if ((i < (mGridWidth - 1)) && (cell_type[ix(i + 1, j)] == kCellTypeSolid))
          {
            rhs[ix_] -= (u(i + 1, j) - mBoundaryVelocity.x);
          }
          if ((j > 0) && (cell_type[ix(i, j - 1)] == kCellTypeSolid))
          {
            rhs[ix_] -= (mBoundaryVelocity.y - v(i, j));
          }
          if ((j < (mGridHeight - 1)) && (cell_type[ix(i, j + 1)] == kCellTypeSolid))
          {
            rhs[ix_] -= (v(i, j + 1) - mBoundaryVelocity.y);
          }
        }
      }

      // Set the coefficients

      coef_diag.clear();
      coef_plus_i.clear();
      coef_plus_j.clear();

      for (int j = 0; j < mGridHeight; j++)
      for (int i = 0; i < mGridWidth; i++)
      {
        if (cell_type[ix(i, j)] == kCellTypeFluid)
        {
          if (cell_type[ix(i + 1, j)] != kCellTypeSolid)
          {
            coef_diag[ix(i, j)] += 1.0f;

            if (cell_type[ix(i + 1, j)] == kCellTypeFluid)
            {
              coef_plus_i[ix(i, j)] = -1.0f;
            }
          }

          if (cell_type[ix(i - 1, j)] != kCellTypeSolid)
          {
            coef_diag[ix(i, j)] += 1.0f;
          }

          if (cell_type[ix(i, j + 1)] != kCellTypeSolid)
          {
            coef_diag[ix(i, j)] += 1.0f;

            if (cell_type[ix(i, j + 1)] == kCellTypeFluid)
            {
              coef_plus_j[ix(i, j)] = -1.0f;
            }
          }

          if (cell_type[ix(i, j - 1)] != kCellTypeSolid)
          {
            coef_diag[ix(i, j)] += 1.0f;
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

        if ((cell_type[ix_]) == kCellTypeFluid)
        {
          float pressure = static_cast<float>(p[ix_]);

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
      p.clear();

      double r_max = 0.0;

      for (unsigned int i = 0; i < rhs.size(); i++)
      {
        if (std::fabs(rhs[i]) > r_max)
        {
          r_max = std::fabs(rhs[i]);
        }
      }

      if (r_max == 0.0)
      {
        return;
      }

      r = rhs;

      double tolerance = kPcgTolerance * r_max;

      calcPrecond();
      applyPrecond();

      s = z;

      double sigma = inner_prod(z, r);

      if (sigma == 0.0)
      {
        return;
      }

      for (int i = 0; i < kPcgMaxIterations; i++)
      {
        applyA();

        double alpha = sigma / inner_prod(z, s);

        p += (alpha * s);
        r -= (alpha * z);

        double error = 0.0f;

        for (int j = 0; j < mGridWidth * mGridHeight; j++)
        {
          double abs_r = std::fabs(r[j]);

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

        double sigma_new = inner_prod(z, r);
        double beta = sigma_new / sigma;

        s = z + beta * s;

        sigma = sigma_new;
      }
    }

    void FLIPSolver2D::calcPrecond()
    {
      double tuning_const = 0.99;
      double safety_const = 0.25;

      precond.clear();

      for (int j = 0; j < mGridHeight; j++)
      for (int i = 0; i < mGridWidth; i++)
      {
        int ix = i + j * mGridWidth;

        if (cell_type[ix] == kCellTypeFluid)
        {
          int ix_plus_i = (i - 1) + j * mGridWidth;
          int ix_plus_j = i + (j - 1) * mGridWidth;

          precond[ix] = coef_diag[ix];

          if (i > 0)
          {
            double plus_i = coef_plus_i[ix_plus_i] * precond[ix_plus_i];
            precond[ix] += (-plus_i * plus_i - tuning_const *
              (coef_plus_i[ix_plus_i] * coef_plus_j[ix_plus_i] * precond[ix_plus_i] * precond[ix_plus_i]));
          }
          if (j > 0)
          {
            double plus_j = coef_plus_j[ix_plus_j] * precond[ix_plus_j];
            precond[ix] += (-plus_j * plus_j - tuning_const *
              (coef_plus_j[ix_plus_j] * coef_plus_i[ix_plus_j] * precond[ix_plus_j] * precond[ix_plus_j]));
          }

          if (precond[ix] < safety_const * coef_diag[ix])
          {
            precond[ix] = coef_diag[ix];
          }

          precond[ix] = 1.0 / sqrt(precond[ix] + kPcgEpsilon);
        }
      }
    }

    void FLIPSolver2D::applyPrecond()
    {
      aux.clear();

      for (int j = 0; j < mGridHeight; j++)
      for (int i = 0; i < mGridWidth; i++)
      {
        int ix = i + j * mGridWidth;

        if (cell_type[ix] == kCellTypeFluid)
        {
          int ix_plus_i = (i - 1) + j * mGridWidth;
          int ix_plus_j = i + (j - 1) * mGridWidth;

          double t = r[ix];

          if (i > 0)
          {
            t -= (coef_plus_i[ix_plus_i] * precond[ix_plus_i] * aux[ix_plus_i]);
          }
          if (j > 0)
          {
            t -= (coef_plus_j[ix_plus_j] * precond[ix_plus_j] * aux[ix_plus_j]);
          }

          aux[ix] = t * precond[ix];
        }
      }

      z.clear();

      for (int j = mGridHeight; j >= 0; --j)
      for (int i = mGridWidth; i >= 0; --i)
      {
        int ix = i + j * mGridWidth;

        if (cell_type[ix] == kCellTypeFluid)
        {
          int ix_plus_i = (i + 1) + j * mGridWidth;
          int ix_plus_j = i + (j + 1) * mGridWidth;

          double t = aux[ix];

          if (i < (mGridWidth - 1))
          {
            t -= (coef_plus_i[ix] * precond[ix] * z[ix_plus_i]);
          }
          if (j < (mGridHeight - 1))
          {
            t -= (coef_plus_j[ix] * precond[ix] * z[ix_plus_j]);
          }

          z[ix] = t * precond[ix];
        }
      }
    }

    void FLIPSolver2D::applyA()
    {
      z.clear();

      for (int j = 0; j < mGridHeight; j++)
      for (int i = 0; i < mGridWidth; i++)
      {
        int ix = i + j * mGridWidth;

        if (cell_type[ix] == kCellTypeFluid)
        {
          int ix_plus_i = (i + 1) + j * mGridWidth;
          int ix_plus_j = i + (j + 1) * mGridWidth;
          int ix_minus_i = (i - 1) + j * mGridWidth;
          int ix_minus_j = i + (j - 1) * mGridWidth;

          z[ix] = s[ix] * coef_diag[ix];

          if (i > 0)
          {
            z[ix] += s[ix_minus_i] * coef_plus_i[ix_minus_i];
          }
          if (j > 0)
          {
            z[ix] += s[ix_minus_j] * coef_plus_j[ix_minus_j];
          }
          if (i < (mGridWidth - 1))
          {
            z[ix] += s[ix_plus_i] * coef_plus_i[ix];
          }
          if (j < (mGridHeight - 1))
          {
            z[ix] += s[ix_plus_j] * coef_plus_j[ix];
          }
        }
      }
    }
  }
}
