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

        void addParticle(const glm::fvec2& pos, const glm::fvec2& vel);
        void clearParticles();

      public:
        std::vector<glm::fvec2> positions;
        std::vector<glm::fvec2> velocities;
        int	numParticles;
      };

    public:
      FLIPSolver2D(int grid_width, int grid_height, float dx);

      float timeStep();
      void simulate(float dt);
      void setBoundaryVel(const glm::fvec2& vel);
      float getPressure(int i, int j);
      glm::fvec2 getVelocity(int i, int j);
      glm::fvec2 getVelocity(float i, float j);
      CellType getCellType(int i, int j) const;
      void setCellType(int i, int j, CellType type);
      void setPicFlipFactor(float factor);

      float& u(int i, int j);
      float& v(int i, int j);
      int ix(int i, int j) const;
      int ixBig(int i, int j);

    public:
      Particles mParticles;

    private:
      void applyForce(float dt, float ax, float ay);
      void setBoundary();
      void project(float dt);

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
      void fillHoles();

      void solvePressure();
      void calcPrecond();
      void applyPrecond();
      void applyA();

      float uVel(float i, float j);
      float vVel(float i, float j);
      int uIndex_x(float x, float& wx);
      int uIndex_y(float y, float& wy);
      int vIndex_x(float x, float& wx);
      int vIndex_y(float y, float& wy);
      void swapVel();

    private:
      int mGridWidth;
      int mGridHeight;
      int mGridSize;
      float mDx;
      float mOverDx;
      glm::fvec2 mBoundaryVelocity;
      float mPicFlipFactor;
      std::vector<float> mVelX;
      std::vector<float> mVelY;
      std::vector<float> mDeltaVelX;
      std::vector<float> mDeltaVelY;
      std::vector<float> mWeightSum;
      std::vector<float> mPhi;
      std::vector<CellType> mCellType;
      std::vector<CellType> mCellTypeAux;

      boost::numeric::ublas::vector<double> mP;
      boost::numeric::ublas::vector<double> mR;
      boost::numeric::ublas::vector<double> mS;
      boost::numeric::ublas::vector<double> mZ;
      boost::numeric::ublas::vector<double> mAux;
      boost::numeric::ublas::vector<double> mRhs;
      boost::numeric::ublas::vector<double> mPrecond;
      boost::numeric::ublas::vector<double> mCoefDiag;
      boost::numeric::ublas::vector<double> mCoefPlusI;
      boost::numeric::ublas::vector<double> mCoefPlusJ;
    };
  }
}

#endif  // SRC_PHYSICS_FLUIDS_FLIPSOLVER2D_H_
