#ifndef SRC_PHYSICS_FLUIDS_FLIPSOLVER2D_H_
#define SRC_PHYSICS_FLUIDS_FLIPSOLVER2D_H_

#include <vector>

#include <boost/numeric/ublas/vector.hpp>
#include <glm/glm.hpp>

namespace mk
{
  namespace physics
  {
    enum CellType
    {
      kCellTypeAir = 0,
      kCellTypeFluid,
      kCellTypeSolid
    };

    class FLIPSolver2D
    {
    public:
      struct Particles
      {
      public:
        Particles();

        void addParticle(const glm::vec2& x_, const glm::vec2& u_);
        void clearParticles();

      public:
        std::vector<glm::vec2> x;
        std::vector<glm::vec2> u;
        int	np;
      };

    public:
      FLIPSolver2D(int grid_width, int grid_height, float dx);
      ~FLIPSolver2D();

      float timeStep();
      float timeStepCFL();
      void fluidStepFlip(float dt);
      void setBoundaryVel(const glm::fvec2& vel);
      float getPressure(int i, int j);
      glm::vec2 getVelocity(int i, int j);
      glm::vec2 getVelocity(float i, float j);
      short	getCellType(int i, int j) const;
      void  setCellType(int i, int j, short type);
      void setPicFlipFactor(float factor);
      void gridHasChanged(int i, int j);

      float& u(int i, int j)
      {
        return u_[i + j * (mGridWidth + 1)];
      }

      float& v(int i, int j)
      {
        return v_[i + j * mGridWidth];
      }

      int ix(int i, int j) const
      {
        return i + j * mGridWidth;
      }

      int ix_(int i, int j)
      {
        return i + j * (mGridWidth + 1);
      }

    public:
      Particles particles;

    private:
      // Basic solver steps

      void applyForce(float dt, float ax, float ay);
      void setBoundary();
      void project(float dt);

      // PIC/FLIP methods

      void checkBoundary(float i_init_, float j_init_, float& i_end_, float& j_end_);
      void advectParticles(float dt);
      void particlesToGrid();
      void storeVel();
      float computePhi(float a, float b, float current);
      void computeGridPhi();
      void sweepU(int i0, int i1, int j0, int j1);
      void sweepV(int i0, int i1, int j0, int j1);
      void extrapolateVel();
      void subtractVel();
      void gridToParticles();

      // Projection sub-steps

      void solvePressure();
      void calcPrecond();
      void applyPrecond();
      void applyA();

      // Grid helper methods

      float uVel(float i, float j);
      float vVel(float i, float j);
      int uIndex_x(float x, float& wx);
      int uIndex_y(float y, float& wy);
      int vIndex_x(float x, float& wx);
      int vIndex_y(float y, float& wy);
      void swapVel();

      private:
        // Grid vars

        int mGridWidth;
        int mGridHeight;
        float mDx;
        float mOverDx;
        glm::fvec2 mBoundaryVelocity;
        float mPicFlipFactor;
        float u_max, u_min;
        float v_max, v_min;
        int u_size;
        int v_size;
        int p_sum_size;
        float* u_;
        float* v_;
        float* du;
        float* dv;
        float* p_sum_den;
        float* phi;
        short* cell_type;
        short* cell_type_aux;

        // PCG vars

        boost::numeric::ublas::vector<double> p;
        boost::numeric::ublas::vector<double> r;
        boost::numeric::ublas::vector<double> s;
        boost::numeric::ublas::vector<double> z;
        boost::numeric::ublas::vector<double> aux;
        boost::numeric::ublas::vector<double> rhs;
        boost::numeric::ublas::vector<double> precond;
        boost::numeric::ublas::vector<double> coef_diag;
        boost::numeric::ublas::vector<double> coef_plus_i;
        boost::numeric::ublas::vector<double> coef_plus_j;
    };
  }
}

#endif  // SRC_PHYSICS_FLUIDS_FLIPSOLVER2D_H_
