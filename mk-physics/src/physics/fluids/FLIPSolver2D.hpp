#ifndef SRC_PHYSICS_FLUIDS_FLIPSOLVER2D_H_
#define SRC_PHYSICS_FLUIDS_FLIPSOLVER2D_H_

#include <vector>

#include <boost/numeric/ublas/vector.hpp>
#include <glm/glm.hpp>

#define CELL_FLUID	1
#define CELL_AIR	0
#define CELL_SOLID	2

#define PCG_MAX_ITERATIONS	100
#define PCG_TOLERANCE		1e-4
#define PCG_EPSILON			1e-6

enum
{
	ADVECTION_SIMPLE = 0,
	ADVECTION_MACCORMACK
};

const float g = 9.8f;

struct CParticles {

	std::vector<glm::vec2>	x;
	std::vector<glm::vec2>	u;
	std::vector<float>  q;
	int					np;

	CParticles() : np( 0 )
	{
	}

	void addParticle(const glm::vec2& x_, const glm::vec2& u_, float q_ = 0 )
	{
		x.push_back( x_ );
		u.push_back( u_ );
		q.push_back( q_ );

		++np;
	}

	void clearParticles()
	{
		x.clear();
		u.clear();
		q.clear();

		np = 0;
	}
};

class FLIPSolver2D
{
public:

	CParticles particles;

   FLIPSolver2D( int advection_type, int grid_width, int grid_height, float dx, float density );
  ~FLIPSolver2D();

    float	timeStep		();
    float	timeStepCFL		();
	void	fluidStepBasic	( float dt );
	void	fluidStepFlip	( float dt );
	void	advectStep		( float dt, float* q );
	void	setBoundaryVel	( float usolid, float vsolid );
	float	getPressure		( int i, int j );
  glm::vec2	getVelocity		( int i, int j );
  glm::vec2	getVelocity		( float i, float j );
	float	getQ			( int i, int j );
	void    setQ			( int i, int j, float q_ );
	short	getCellType		( int i, int j ) const;
	void	setCellType		( int i, int j, short type );
	void	setPicFlipFactor( float factor );
	void	gridHasChanged  ( int i, int j );

	inline float& u( int i , int j )
	{
		return u_[( i ) + ( j ) * ( grid_width + 1 )];
	}

	inline float& v( int i , int j )
	{
		return v_[( i ) + ( j ) * ( grid_width )];
	}

	inline int ix( int i, int j ) const
	{
		return i + j * grid_width;
	}

	inline int ix_( int i, int j )
	{
		return i + j * ( grid_width + 1 );
	}

private:

	long	total_time;
	long	project_time;

	// Grid vars

	int		advection_type;
	int		grid_width;
	int		grid_height;
	float	dx;
	float	over_dx;
	float	density;
	float	u_solid;
	float	v_solid;
	float	pic_flip_factor;
	float	u_max, u_min;
	float	v_max, v_min;
	float	q_max, q_min;
	int		u_size;
	int		v_size;
	int		p_sum_size;
	float*	u_;
	float*	v_;
	float*	du;
	float*	dv;
	float*	p_sum_den;
	float*	phi;
	float*  q;
	float*  q_sum_den;
	short*	cell_type;
	short*	cell_type_aux;

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

	// Basic solver steps

	void	selfAdvectSimple	( float dt );
	void	selfAdvectMacCormack( float dt );
	void	advectStepSimple	( float dt, float* q );
	void	advectStepMacCormack( float dt, float* q );
	void	selfSemiLagAdvect	( float dt, short sign, float i, float j, float u_vel, float v_vel, float& u_, float& v_ );
	float	bilerp				( float i, float j, float* q );
	void	applyForce			( float dt, float ax, float ay );
	void	setBoundary			();
	void	project				( float dt );

	// PIC/FLIP methods

	void	checkBoundary	( float i_init_, float j_init_, float& i_end_, float& j_end_ );
	void	advectParticles	( float dt );
	void	particlesToGrid	();
	void	storeVel		();
	float	computePhi		( float a, float b, float current );
	void	computeGridPhi	();
	void	sweepU			( int i0, int i1, int j0, int j1 );
	void	sweepV			( int i0, int i1, int j0, int j1 );
	void	extrapolateVel	();
	void	subtractVel		();
	void	gridToParticles ();

	// Projection sub-steps

	void	solvePressure	();
	void	calcPrecond		();
	void	applyPrecond	();
	void	applyA			();

	// Grid helper methods

    void    debug_info      ( char* header, bool debug_pressure );

	inline float	uVel	( float i, float j );
	inline float	vVel	( float i, float j );
	inline int		uIndex_x( float x, float& wx );
	inline int		uIndex_y( float y, float& wy );
	inline int		vIndex_x( float x, float& wx );
	inline int		vIndex_y( float y, float& wy );
	inline float	clamp	( float value, float min, float max );
	inline int		clamp	( int value, int min, int max );
	inline void		swapVel	();

	inline float min_( float a, float b)
	{
		return a < b ? a : b;
	}

	inline float max_( float a, float b)
	{
		return a > b ? a : b;
	}
};

#endif  // SRC_PHYSICS_FLUIDS_FLIPSOLVER2D_H_
