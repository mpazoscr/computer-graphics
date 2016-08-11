#include <stdio.h>
#include <string.h>
#include "FLIPSolver2D.hpp"
#include "CTimer.hpp"

FLIPSolver2D::FLIPSolver2D( int advection_type, int grid_width, int grid_height, float dx, float density ) :
	advection_type( advection_type ), grid_width( grid_width ), grid_height( grid_height ), dx( dx ), over_dx( 1.0f / dx ), density( density ),
	p( grid_width * grid_height ), r( grid_width * grid_height ), s( grid_width * grid_height ),
	z( grid_width * grid_height ), aux( grid_width * grid_height ), rhs( grid_width * grid_height ), precond( grid_width * grid_height ),
	coef_diag( grid_width * grid_height ), coef_plus_i( grid_width * grid_height ),
	coef_plus_j( grid_width * grid_height )
{
	u_size			= ( grid_width + 1 ) * grid_height;
	v_size			= grid_width * ( grid_height + 1 );
	p_sum_size		= ( grid_width + 1 ) * ( grid_height + 1 );
	u_solid			= 0.0f;
	v_solid			= 0.0f;
	pic_flip_factor	= 1.0f;

	u_				= new float[u_size];
	v_				= new float[v_size];
	du				= new float[u_size];
	dv				= new float[v_size];
	p_sum_den		= new float[p_sum_size];
	phi				= new float[grid_width * grid_height];
	q				= new float[grid_width * grid_height];
	q_sum_den   	= new float[grid_width * grid_height];
	cell_type		= new short[grid_width * grid_height];
	cell_type_aux	= new short[grid_width * grid_height];

	memset( u_, 0, u_size * sizeof( float ));
	memset( v_, 0, v_size * sizeof( float ));
	memset( du, 0, u_size * sizeof( float ));
	memset( dv, 0, v_size * sizeof( float ));

	memset( q, 0, grid_width * grid_height * sizeof( float ));

	for( int j = 0; j < grid_height; j++ )
	for( int i = 0; i < grid_width ; i++ )
		cell_type[ix( i , j )] = CELL_FLUID;

	// Solid square surrounding the whole area and rectangle in the middle

	for( int i = 0; i < grid_width; i++ )
	{
		cell_type[ix( i, 0 )]				= CELL_SOLID;
		cell_type[ix( i, grid_height - 1 )]	= CELL_SOLID;
	}

	for( int j = 0; j < grid_height; j++ )
	{
		cell_type[ix( 0, j )]				= CELL_SOLID;
		cell_type[ix( grid_width - 1, j )]	= CELL_SOLID;
	}
}

FLIPSolver2D::~FLIPSolver2D()
{
	if( u_ )			delete[] u_;
	if( v_ )			delete[] v_;
	if( du )			delete[] du;
	if( dv )			delete[] dv;
	if( p_sum_den )		delete[] p_sum_den;
	if( phi )			delete[] phi;
	if( q )				delete[] q;
	if( cell_type )		delete[] cell_type;
	if( cell_type_aux )	delete[] cell_type_aux;
}

void FLIPSolver2D::setBoundaryVel( float usolid, float vsolid )
{
	u_solid = usolid;
	v_solid = vsolid;
}

void FLIPSolver2D::fluidStepBasic( float dt )
{
	CTimer total_timer ( TIMER_HIGH_PRECISION );
	CTimer advect_timer( TIMER_HIGH_PRECISION );

	total_timer .start();
	advect_timer.start();

	if( advection_type == ADVECTION_MACCORMACK )
		selfAdvectMacCormack( dt );
	else
		selfAdvectSimple( dt );

	long advect_time = advect_timer.getTimeElapsedMilliSec();

	applyForce	( dt, 0.0f, -g );
	setBoundary	( );
	project		( dt );

	total_time = total_timer.getTimeElapsedMilliSec();

//	printf( "total time: %ld ms. advect time: %ld ms. pcg time: %ld ms. percentage: %g\n", total_time, advect_time, project_time, (( double )project_time / ( double )total_time ) * 100.0 );
}

void FLIPSolver2D::debug_info( char* header, bool debug_pressure )
{
    FILE* f_out = fopen( "output.txt", "a" );

    if( !f_out )
        return;

    fprintf( f_out, "\n%s\n\n", header );

    if( !debug_pressure )
    {
        for( int p = 0; p < particles.np; p++ )
            fprintf( f_out, "particle[%d] : (%.05f, %.05f) (%.05f, %.05f)\n", p, particles.x[p].x, particles.x[p].y, particles.u[p].x, particles.u[p].y );

        fprintf( f_out, "\n" );

        for( int i = 0; i < u_size; i++ )
            fprintf( f_out, "u[%d] : %.05f\n", i, u_[i] );

        for( int i = 0; i < v_size; i++ )
            fprintf( f_out, "v[%d] : %.05f\n", i, v_[i] );
    }
    else
    {
        for( int i = 0; i < grid_width * grid_height; i++ )
            fprintf( f_out, "p[%d] : %.05f\n", i, p[i]);
    }

	fclose( f_out );
}

void FLIPSolver2D::fluidStepFlip( float dt )
{
	CTimer total_timer      ( TIMER_HIGH_PRECISION );
	CTimer advect_timer     ( TIMER_HIGH_PRECISION );
	CTimer transfer_timer   ( TIMER_HIGH_PRECISION );
	CTimer transfer_timer2	( TIMER_HIGH_PRECISION );
	CTimer extrapolate_timer( TIMER_HIGH_PRECISION );

	total_timer.start();

	long advect_time;
	long transfer_time;
	long extrapolate_time;

	advect_timer.start();
	advectParticles	( dt );
	advect_time = advect_timer.getTimeElapsedMilliSec();

	transfer_timer.start();
	particlesToGrid	();
	transfer_time = transfer_timer.getTimeElapsedMilliSec();

	storeVel		();
	applyForce		( dt, 0.0f, -g );

	extrapolate_timer.start();
	computeGridPhi	();
	extrapolateVel	();
	extrapolate_time = extrapolate_timer.getTimeElapsedMilliSec();

	setBoundary		();
	project			( dt );
	//extrapolateVel	();
	subtractVel		();

	transfer_timer2.start();
	gridToParticles	();
	transfer_time += transfer_timer2.getTimeElapsedMilliSec();

	total_time = total_timer.getTimeElapsedMilliSec();

//	printf( "tot: %ld ms. adv: %ld ms. trans: %ld ms. extr: %ld ms. pcg: %ld ms. perc.: %g\n", total_time, advect_time, transfer_time, extrapolate_time, project_time, (( double )project_time / ( double )total_time ) * 100.0 );
}

float FLIPSolver2D::getPressure( int i, int j )
{
	return ( float )p[ix( i, j )];
}

vec2f FLIPSolver2D::getVelocity( int i, int j )
{
	return vec2f(( u( i, j ) + u( i + 1, j )) * 0.5f, ( v( i, j ) + v( i, j + 1 )) * 0.5f );
}

vec2f FLIPSolver2D::getVelocity( float i, float j )
{
	return vec2f( uVel( i, j ), vVel( i, j ));
}

float FLIPSolver2D::getQ( int i, int j )
{
	return q[ix( i, j )];
}

void FLIPSolver2D::setQ( int i, int j, float q_ )
{
	q[ix( i, j )] = q_;
}

short FLIPSolver2D::getCellType( int i, int j )
{
	return cell_type[ix( i, j )];
}

void FLIPSolver2D::setCellType( int i, int j, short type )
{
	cell_type[ix( i, j )] = type;
}

void FLIPSolver2D::setPicFlipFactor( float factor )
{
	pic_flip_factor = clamp( 0.0f, 1.0f, factor );
}

void FLIPSolver2D::gridHasChanged( int i, int j )
{
	for( int p = 0; p < particles.np; p++ )
	{
		float i_p = particles.x[p].x * over_dx;
		float j_p = particles.x[p].y * over_dx;

		if( fabs( i_p - i ) > 1 || fabs( j_p - j ) > 1 )
			continue;

		particles.u[p].x = uVel( i_p, j_p );
		particles.u[p].y = vVel( i_p, j_p );
		particles.q[p]   = bilerp( i_p, j_p, q );
	}
}

float FLIPSolver2D::timeStep()
{
	float max_vel = 0.0f;

	for( int j = 0; j < grid_height; j++ )
	for( int i = 0; i < grid_width ; i++ )
	{
		float vel = getVelocity( i, j ).norm();

		if( fabs( vel ) > max_vel )
			max_vel = fabs( vel );
	}

	//return ( 5.0f * dx ) / ( max_vel + sqrt( 5.0f * dx * g ));

	float max_v = max_( dx * g, max_vel );

	if( max_v < 1e-16f )
		max_v = 1e-16f;

	return dx / sqrt( max_v );
}

float FLIPSolver2D::timeStepCFL()
{
	float max_u = 0.0f;
	float max_v = 0.0f;

	for( int i = 0; i < u_size; i++ )
	{
		if( !( fabs( u_[i] ) <= max_u ))
			max_u = fabs( u_[i] );
	}

	for( int i = 0; i < v_size; i++ )
	{
		if( !( fabs( v_[i] ) <= max_v ))
			max_v = fabs( v_[i] );
	}

	float max_vel = max_( g * dx, max_u * max_u + max_v * max_v );

	if( max_vel < 1e-16f )
		max_vel = 1e-16f;

	return dx / sqrt( max_vel );
}

float FLIPSolver2D::clamp( float value, float min, float max )
{
	if( value < min ) return min;
	if( value > max ) return max;

	return value;
}

int FLIPSolver2D::clamp( int value, int min, int max )
{
	if( value < min ) return min;
	if( value > max ) return max;

	return value;
}

void FLIPSolver2D::swapVel()
{
	float* u_tmp = u_;
	float* v_tmp = v_;

	u_ = du;
	v_ = dv;

	du = u_tmp;
	dv = v_tmp;
}

float FLIPSolver2D::uVel( float i, float j )
{
	i = clamp( i, 0.0f, ( float )grid_width  );
	j = clamp( j, 0.0f, ( float )grid_height );

	if( j < 0.5f || j > ( grid_height - 0.5f ))
	{
		int i0	= ( int )i;
		int i1	= i0 + 1;
		int jj	= ( j <= 0.5f ) ? 0 : ( grid_height - 1 );

		float t0 = i - ( float )i0;
		float t1 = 1.0f - t0;

		u_min = min( u( i0, jj ), u( i1, jj ));
		u_max = max( u( i0, jj ), u( i1, jj ));

		return t1 * u( i0, jj ) + t0 * u( i1, jj );
	}
	else
	{
		int i0 = ( int )i;
		int j0 = ( int )( j - 0.5f );
		int i1 = i0 + 1;
		int j1 = j0 + 1;

		float t0 = i - ( float )i0;
		float t1 = 1.0f - t0;
		float s0 = ( j - 0.5f ) - ( float )j0;
		float s1 = 1.0f - s0;

		u_min = min( min( u( i0, j0 ), u( i1, j0 )), min( u( i0, j1 ), u( i1, j1 )));
		u_max = max( max( u( i0, j0 ), u( i1, j0 )), max( u( i0, j1 ), u( i1, j1 )));

		return	s1 * ( t1 * u( i0, j0 ) + t0 * u( i1, j0 )) +
				s0 * ( t1 * u( i0, j1 ) + t0 * u( i1, j1 ));
	}
}

float FLIPSolver2D::vVel( float i, float j )
{
	i = clamp( i, 0.0f, ( float )grid_width  );
	j = clamp( j, 0.0f, ( float )grid_height );

	if( i < 0.5f || i > ( grid_width - 0.5f ))
	{
		int ii	= ( i <= 0.5f ) ? 0 : ( grid_width - 1 );
		int j0	= ( int )j;
		int j1	= j0 + 1;

		float t0 = j - ( float )j0;
		float t1 = 1.0f - t0;

		v_min = min( v( ii, j0 ), v( ii, j1 ));
		v_max = max( v( ii, j0 ), v( ii, j1 ));

		return t1 * v( ii, j0 ) + t0 * v( ii, j1 );
	}
	else
	{
		int i0 = ( int )( i - 0.5f );
		int j0 = ( int )j;
		int i1 = i0 + 1;
		int j1 = j0 + 1;

		float t0 = ( i - 0.5f ) - ( float )i0;
		float t1 = 1.0f - t0;
		float s0 = j - ( float )j0;
		float s1 = 1.0f - s0;

		v_min = min( min( v( i0, j0 ), v( i1, j0 )), min( v( i0, j1 ), v( i1, j1 )));
		v_max = max( max( v( i0, j0 ), v( i1, j0 )), max( v( i0, j1 ), v( i1, j1 )));

		return	s1 * ( t1 * v( i0, j0 ) + t0 * v( i1, j0 )) +
				s0 * ( t1 * v( i0, j1 ) + t0 * v( i1, j1 ));
	}
}

int FLIPSolver2D::uIndex_x( float x, float& wx )
{
	float x_ = clamp( x * over_dx, 0.0f, ( float )grid_width - dx * 1e-5f );

	int i = ( int )x_;
	wx = x_ - i;

	return i;
}

int FLIPSolver2D::uIndex_y( float y, float& wy )
{
	float y_ = y * over_dx - 0.5f;

	int j = ( int )y_;

	if( j < 0 )
	{
		wy = 0.0;
		return 0;
	}

	if( j > grid_height - 2 )
	{
		wy = 1.0f;
		return grid_height - 2;
	}

	wy = y_ - j;

	return j;
}

int FLIPSolver2D::vIndex_x( float x, float& wx )
{
	float x_ = x * over_dx - 0.5f;

	int i = ( int )x_;

	if( i < 0 )
	{
		wx = 0.0;
		return 0;
	}

	if( i > grid_width - 2 )
	{
		wx = 1.0f;
		return grid_width - 2;
	}

	wx = x_ - i;

	return i;
}

int FLIPSolver2D::vIndex_y( float y, float& wy )
{
	float y_ = clamp( y * over_dx, 0.0f, ( float )grid_height - dx * 1e-5f );

	int j = ( int )y_;
	wy = y_ - j;

	return j;
}

void FLIPSolver2D::selfSemiLagAdvect( float dt, short sign, float i, float j, float u_vel, float v_vel, float& i_, float& j_ )
{
	// Semi-Lagrangian advection 2nd order integration

	float i_mid	= ( i * dx + sign * u_vel * dt * 0.5f ) * over_dx;
	float j_mid	= ( j * dx + sign * v_vel * dt * 0.5f ) * over_dx;
	
	i_	= ( i * dx + sign * uVel( i_mid, j_mid ) * dt ) * over_dx;
	j_	= ( j * dx + sign * vVel( i_mid, j_mid ) * dt ) * over_dx;
}

void FLIPSolver2D::selfAdvectMacCormack( float dt )
{
	// MacCormack advection

	for( int j = 0; j < grid_height	+ 1; j++ )
	for( int i = 0; i < grid_width	+ 1; i++ )
	{
		int ix_ = ix( i, j );

		if( i < grid_width && j < grid_height && cell_type[ix_] == CELL_SOLID )
			continue;

		if( j < grid_height )
		{
			float u0 = u	( i, j );
			float v0 = vVel (( float )i, ( float )j + 0.5f );

			float i_, j_;

			selfSemiLagAdvect( dt, -1, ( float )i, ( float )j + 0.5f, u0, v0, i_, j_ );

			float u1 = uVel( i_, j_ );
			float v1 = vVel( i_, j_ );

			float uMax = u_max;
			float uMin = u_min;

			selfSemiLagAdvect( dt, 1, i_, j_, u1, v1, i_, j_ );

			float u2 = uVel( i_, j_ );
			float e  = ( u2 - u0 ) * 0.5f;
			
			u( i, j ) = u1 + e;

			if( u( i, j ) < uMin || u( i, j ) > uMax )
				u( i, j ) = u1;
		}

		if( i < grid_width )
		{
			float u0 = uVel (( float )i + 0.5f, ( float )j );
			float v0 = v	( i, j );

			float i_, j_;

			selfSemiLagAdvect( dt, -1, ( float )i + 0.5f, ( float )j, u0, v0, i_, j_ );

			float u1 = uVel( i_, j_ );
			float v1 = vVel( i_, j_ );

			float vMax = v_max;
			float vMin = v_min;

			selfSemiLagAdvect( dt, 1, i_, j_, u1, v1, i_, j_ );

			float v2 = vVel( i_, j_ );
			float e  = ( v2 - v0 ) * 0.5f;

			v( i, j ) = v1 + e;

			if( v( i, j ) < vMin || v( i, j ) > vMax )
				v( i, j ) = v1;
		}
	}
}

void FLIPSolver2D::selfAdvectSimple( float dt )
{
	// Simple Semi-Lagrangian advection
	
	for( int j = 0; j < grid_height	+ 1; j++ )
	for( int i = 0; i < grid_width	+ 1; i++ )
	{
		int ix_ = ix( i, j );

		if( i < grid_width && j < grid_height && cell_type[ix_] == CELL_SOLID )
			continue;

		if( j < grid_height )
		{
			float u_vel	= u		( i, j );
			//float v_vel = vVel  ( i * dx, ( j + 0.5f ) * dx );
			float v_vel = vVel  (( float )i, ( float )j + 0.5f );

			float i_, j_;

			selfSemiLagAdvect( dt, -1, ( float )i, ( float )j + 0.5f, u_vel, v_vel, i_, j_ );

			u( i, j ) = uVel( i_, j_ );
		}

		if( i < grid_width )
		{
			//float u_vel	= uVel  (( i + 0.5f ) * dx, j * dx );
			float u_vel	= uVel  (( float )i + 0.5f, ( float )j );
			float v_vel	= v		( i, j );

			float i_, j_;

			selfSemiLagAdvect( dt, -1, ( float )i + 0.5f, ( float )j, u_vel, v_vel, i_, j_ );

			v( i, j ) = vVel( i_, j_ );
		}
	}
}

float FLIPSolver2D::bilerp( float i, float j, float* q )
{
	float i_final = clamp( i, 0.5f, ( float )grid_width  - 0.5f );
	float j_final = clamp( j, 0.5f, ( float )grid_height - 0.5f );

	int i0 = ( int )( i_final - 0.5f );
	int j0 = ( int )( j_final - 0.5f );
	int i1 = ( i0 == grid_width  - 1 ) ? grid_width  - 1 : i0 + 1;
	int j1 = ( j0 == grid_height - 1 ) ? grid_height - 1 : j0 + 1;

	float t0 = ( i_final - 0.5f ) - ( float )i0;
	float t1 = 1.0f	- t0;
	float s0 = ( j_final - 0.5f ) - ( float )j0;
	float s1 = 1.0f	- s0;

	q_min = min( min( q[ix( i0, j0 )], q[ix( i1, j0 )]), min( q[ix( i0, j1 )], q[ix( i1, j1 )]));
	q_max = max( max( q[ix( i0, j0 )], q[ix( i1, j0 )]), max( q[ix( i0, j1 )], q[ix( i1, j1 )]));

	return	s1 * ( t1 * q[ix( i0, j0 )] + t0 * q[ix( i1, j0 )] ) +
			s0 * ( t1 * q[ix( i0, j1 )] + t0 * q[ix( i1, j1 )] );
}

void FLIPSolver2D::advectStep( float dt, float* q )
{
	if( advection_type == ADVECTION_MACCORMACK )
		advectStepMacCormack( dt, q );
	else
		advectStepSimple( dt, q );
}

void FLIPSolver2D::advectStepMacCormack( float dt, float* q )
{
	for( int j = 0; j < grid_height	; j++ )
	for( int i = 0; i < grid_width	; i++ )
	{
		if( cell_type[ix( i , j )] != CELL_FLUID )
			continue;

		vec2f vel = getVelocity( i, j );
		float q0  = q[ix( i, j )];

		float i_, j_;

		selfSemiLagAdvect( dt, -1, ( float )i + 0.5f, ( float )j + 0.5f, vel.x, vel.y, i_, j_ );

		float q1 = bilerp( i_, j_, q );

		float qMin = q_min;
		float qMax = q_max;

		float u1 = uVel( i_, j_ );
		float v1 = vVel( i_, j_ );

		selfSemiLagAdvect( dt, 1, i_, j_, u1, v1, i_, j_ );

		float q2 = bilerp( i_, j_, q );

		float e	 = ( q2 - q0 ) * 0.5f;
		float qf = q1 + e;

		if( qf < qMin || qf > qMax )
			q[ix( i, j )] = q1;
		else
			q[ix( i, j )] = qf;
	}
}

void FLIPSolver2D::advectStepSimple( float dt, float* q )
{
	for( int j = 0; j < grid_height	; j++ )
	for( int i = 0; i < grid_width	; i++ )
	{
		if( cell_type[ix( i , j )] != CELL_FLUID )
			continue;

		vec2f vel = getVelocity( i, j );

		float i_, j_;

		selfSemiLagAdvect( dt, -1, ( float )i + 0.5f, ( float )j + 0.5f, vel.x, vel.y, i_, j_ );

		q[ix( i, j )] = bilerp( i_, j_, q );
	}
}

/*void FLIPSolver2D::selfAdvect( float dt )
{
	// Semi-Lagrangian advection 2nd order integration

	for( int j = 0; j < grid_height	+ 1; j++ )
	for( int i = 0; i < grid_width	+ 1; i++ )
	{
		int ix_ = ix( i, j );

		if( i < grid_width && j < grid_height && cell_type[ix_] == CELL_SOLID )
			continue;

		if( j < grid_height )
		{
			float u_vel	= u		( i, j );
			float v_vel = vVel  ( i * dx, ( j + 0.5f ) * dx );

			float i_mid		= ((( float )i )		* dx - u_vel * dt * 0.5f )			* over_dx;
			float j_mid		= ((( float )j + 0.5f ) * dx - v_vel * dt * 0.5f )			* over_dx;
			float i_final	= ((( float )i )		* dx - uVel( i_mid, j_mid ) * dt )	* over_dx;
			float j_final	= ((( float )j + 0.5f )	* dx - vVel( i_mid, j_mid ) * dt )	* over_dx;

			u( i, j ) = uVel( i_final, j_final );
		}

		if( i < grid_width )
		{
			float u_vel	= uVel  (( i + 0.5f ) * dx, j * dx );
			float v_vel	= v		( i, j );

			float i_mid		= ((( float )i + 0.5f )	* dx - u_vel * dt * 0.5f )			* over_dx;
			float j_mid		= ((( float )j )		* dx - v_vel * dt * 0.5f )			* over_dx;
			float i_final	= ((( float )i + 0.5f )	* dx - uVel( i_mid, j_mid ) * dt )	* over_dx;
			float j_final	= ((( float )j )		* dx - vVel( i_mid, j_mid ) * dt )	* over_dx;

			v( i, j ) = vVel( i_final, j_final );
		}
	}
}

void FLIPSolver2D::advectStep( float dt, float* q )
{
	for( int j = 0; j < grid_height	; j++ )
	for( int i = 0; i < grid_width	; i++ )
	{
		if( cell_type[ix( i , j )] != CELL_FLUID )
			continue;

		vec2f vel = getVelocity( i, j );

		float i_mid		= (( float )( i + 0.5f ) * dx - vel.x * dt * 0.5f )			* over_dx;
		float j_mid		= (( float )( j + 0.5f ) * dx - vel.y * dt * 0.5f )			* over_dx;
		float i_final	= (( float )( i + 0.5f ) * dx - uVel( i_mid, j_mid ) * dt )	* over_dx;
		float j_final	= (( float )( j + 0.5f ) * dx - vVel( i_mid, j_mid ) * dt )	* over_dx;

		i_final = clamp( i_final, 0.5f, ( float )grid_width  - 0.5f );
		j_final = clamp( j_final, 0.5f, ( float )grid_height - 0.5f );

		int i0 = ( int )( i_final - 0.5f );
		int j0 = ( int )( j_final - 0.5f );
		int i1 = ( i0 == grid_width  - 1 ) ? grid_width  - 1 : i0 + 1;
		int j1 = ( j0 == grid_height - 1 ) ? grid_height - 1 : j0 + 1;

		float t0 = ( i_final - 0.5f ) - ( float )i0;
		float t1 = 1.0f	- t0;
		float s0 = ( j_final - 0.5f ) - ( float )j0;
		float s1 = 1.0f	- s0;

		q[ix( i, j )] = s1 * ( t1 * q[ix( i0, j0 )] + t0 * q[ix( i1, j0 )] ) +
						s0 * ( t1 * q[ix( i0, j1 )] + t0 * q[ix( i1, j1 )] );
	}
}*/

void FLIPSolver2D::applyForce( float dt, float ax, float ay )
{
	for( int j = 0; j < grid_height; j++ )
	for( int i = 0; i < grid_width ; i++ )
	{
		if( cell_type[ix( i, j )] == CELL_SOLID )
			continue;

		if( ax != 0.0f )
		{
			u( i, j ) += ( dt * ax );

			if( i == grid_width - 1 )
				u( i + 1, j ) += ( dt * ax );
		}

		if( ay != 0.0f )
		{
			v( i, j ) += ( dt * ay );

			if( j == grid_height - 1 )
				v( i, j + 1 ) += ( dt * ay );
		}
	}
}

void FLIPSolver2D::setBoundary()
{
	for( int j = 0; j < grid_height; j++ )
	for( int i = 0; i < grid_width ; i++ )
	{
		/*if( cell_type[ix( i, j )] == CELL_SOLID )
		{
		    if( i == 0 || i == grid_width - 1 )
		    {
                u( i    , j ) = 0.0f;
                u( i + 1, j ) = 0.0f;
		    }

		    if( j == 0 || j == grid_height - 1 )
            {
                v( i, j     ) = 0.0f;
                v( i, j + 1 ) = 0.0f;
            }
		}*/

		if( cell_type[ix( i, j )] == CELL_SOLID )
		{
			u( i	, j ) = u_solid;
			u( i + 1, j ) = u_solid;

			v( i, j		) = v_solid;
			v( i, j + 1 ) = v_solid;
		}
	}
}

void FLIPSolver2D::checkBoundary( float i_init_, float j_init_, float& i_end_, float& j_end_ )
{
	i_end_ = clamp( i_end_, 0.0, grid_width - 1.0f );
	j_end_ = clamp( j_end_, 0.0, grid_width - 1.0f );

	int i_init  = ( int )i_init_;
	int j_init  = ( int )j_init_;
	int i_end	= ( int )i_end_;
	int j_end	= ( int )j_end_;

	if( i_init == i_end && j_init == j_end )
		return;

	if( cell_type[ix( i_end, j_end )] == CELL_SOLID )
	{
		int i_sub = i_init - i_end;
		int j_sub = j_init - j_end;

		if( i_sub != 0 || j_sub != 0 )
		{
			if( cell_type[ix( i_init + i_sub, j_init )] != CELL_SOLID )
				j_sub = 0;
			else if( cell_type[ix( i_init, j_init + j_sub )] != CELL_SOLID )
				i_sub = 0;
		}

		if( i_sub != 0 )
			i_end_ += (( i_end_ - i_end ) + 0.1f ) * i_sub;
		if( j_sub != 0 )
			j_end_ += (( j_end_ - j_end ) + 0.1f ) * j_sub;
	}
}

void FLIPSolver2D::advectParticles( float dt )
{
	/*// Hardcoded boundary!

	float i_min = 1.1f;
	float j_min = 1.1f;
	float i_max = grid_width  - 1.1f;
	float j_max = grid_height - 1.1f;*/

	// Advect particles with five substeps of ( 0.2f * dt ) timestep

	for( int r = 0; r < 5			; r++ )
	for( int p = 0; p < particles.np; p++ )
	{
		float i_p = particles.x[p].x * over_dx;
		float j_p = particles.x[p].y * over_dx;

		float i_mid	= ( i_p * dx + uVel( i_p, j_p )	* dt * 0.2f * 0.5f ) * over_dx;
		float j_mid	= ( j_p * dx + vVel( i_p, j_p )	* dt * 0.2f * 0.5f ) * over_dx;

		//i_mid = clamp( i_mid, i_min, i_max );
		//j_mid = clamp( j_mid, j_min, j_max );

		checkBoundary( i_p, j_p, i_mid, j_mid );

		float i_final = ( i_p * dx + uVel( i_mid, j_mid ) * dt * 0.2f ) * over_dx;
		float j_final = ( j_p * dx + vVel( i_mid, j_mid ) * dt * 0.2f ) * over_dx;

		//i_final = clamp( i_final, i_min, i_max );
		//j_final = clamp( j_final, j_min, j_max );

		checkBoundary( i_p, j_p, i_final, j_final );

		particles.x[p].x = i_final * dx;
		particles.x[p].y = j_final * dx;
	}
}

void FLIPSolver2D::particlesToGrid()
{
/*	memset( q			, 0, grid_width * grid_height * sizeof( float ));
	memset( q_sum_den	, 0, grid_width * grid_height * sizeof( float ));

	for( int p = 0; p < particles.np; p++ )
	{
		float x = clamp( particles.x[p].x * over_dx - 0.5f, 0.0f, ( float )grid_width  - dx * 1e-5f );
		float y = clamp( particles.x[p].y * over_dx - 0.5f, 0.0f, ( float )grid_height - dx * 1e-5f );

		int i = ( int )x;
		int j = ( int )y;

		float wx = x - i;
		float wy = y - j;
		float w;

		w  = ( 1.0f - wx ) * ( 1.0f - wy );
		q		 [ix( i, j )] += particles.q[p] * w;
		q_sum_den[ix( i, j )] += w;

		w = wx * ( 1.0f - wy );
		q		 [ix( i + 1, j )] += particles.q[p] * w;
		q_sum_den[ix( i + 1, j )] += w;

		w = ( 1.0f - wx ) * wy;
		q		 [ix( i, j + 1 )] += particles.q[p] * w;
		q_sum_den[ix( i, j + 1 )] += w;

		w = wx * wy;
		q		 [ix( i + 1, j + 1 )] += particles.q[p] * w;
		q_sum_den[ix( i + 1, j + 1 )] += w;
	}

	for( int j = 0; j < grid_height ; j++ )
	for( int i = 0; i < grid_width	; i++ )
	{
		int ix_ = ix( i, j );

		if( q_sum_den[ix_] != 0 )
			q[ix_] /= q_sum_den[ix_];
	}
*/
	// Update u component

	memset( u_			, 0, u_size		* sizeof( float ));
	memset( p_sum_den	, 0, p_sum_size * sizeof( float ));

	for( int p = 0; p < particles.np; p++ )
	{
		float wx, wy, w;

		int i = uIndex_x( particles.x[p].x, wx );
		int j = uIndex_y( particles.x[p].y, wy );

		w  = ( 1.0f - wx ) * ( 1.0f - wy );
		u( i, j ) += particles.u[p].x * w;
		p_sum_den[ix_( i, j )] += w;

		w = wx * ( 1.0f - wy );
		u( i + 1, j ) += particles.u[p].x * w;
		p_sum_den[ix_( i + 1, j )] += w;

		w = ( 1.0f - wx ) * wy;
		u( i, j + 1 ) += particles.u[p].x * w;
		p_sum_den[ix_( i, j + 1 )] += w;

		w = wx * wy;
		u( i + 1, j + 1 ) += particles.u[p].x * w;
		p_sum_den[ix_( i + 1, j + 1 )] += w;
	}

	for( int j = 0; j < grid_height		; j++ )
	for( int i = 0; i < grid_width + 1	; i++ )
	{
		int ix = ix_( i, j );

		if( p_sum_den[ix] != 0 )
			u( i, j ) /= p_sum_den[ix];
	}

	// Update v component

	memset( v_			, 0, v_size		* sizeof( float ));
	memset( p_sum_den	, 0, p_sum_size * sizeof( float ));

	for( int p = 0; p < particles.np; p++ )
	{
		float wx, wy, w;

		int i = vIndex_x( particles.x[p].x, wx );
		int j = vIndex_y( particles.x[p].y, wy );

		w  = ( 1.0f - wx ) * ( 1.0f - wy );
		v( i, j ) += particles.u[p].y * w;
		p_sum_den[ix( i, j )] += w;

		w = wx * ( 1.0f - wy );
		v( i + 1, j ) += particles.u[p].y * w;
		p_sum_den[ix( i + 1, j )] += w;

		w = ( 1.0f - wx ) * wy;
		v( i, j + 1 ) += particles.u[p].y * w;
		p_sum_den[ix( i, j + 1 )] += w;

		w = wx * wy;
		v( i + 1, j + 1 ) += particles.u[p].y * w;
		p_sum_den[ix( i + 1, j + 1 )] += w;
	}

	for( int j = 0; j < grid_height + 1	; j++ )
	for( int i = 0; i < grid_width		; i++ )
	{
		int ix_ = ix( i, j );

		if( p_sum_den[ix_] != 0 )
			v( i, j ) /= p_sum_den[ix_];
	}

	// Mark fluid particles

	for( int j = 0; j < grid_height; j++ )
	for( int i = 0; i < grid_width ; i++ )
	{
		int ix_ = ix( i, j );

		if( cell_type[ix_] != CELL_SOLID )
			cell_type[ix_] = CELL_AIR;
	}

	for( int p = 0; p < particles.np; p++ )
	{
		float wx, wy;

		int i = uIndex_x( particles.x[p].x, wx );
		int j = vIndex_y( particles.x[p].y, wy );

		int ix_ = ix( i, j );

		if( cell_type[ix_] != CELL_SOLID )
			cell_type[ix_] = CELL_FLUID;
	}

	for( int j = 0; j < grid_height; j++ )
	for( int i = 0; i < grid_width ; i++ )
	{
		int ix_ = ix( i, j );

        cell_type_aux[ix_] = CELL_AIR;

		if( cell_type[ix_] == CELL_AIR )
		{
            int adjacent_fluid_cells = 0;
            int adjacent_solid_cells = 0;

            if( i > 0 && ( cell_type[ix( i - 1, j )] == CELL_FLUID ))
                ++adjacent_fluid_cells;
            if( i > 0 && ( cell_type[ix( i - 1, j )] == CELL_SOLID ))
                ++adjacent_solid_cells;

            if( i < grid_width - 1  && ( cell_type[ix( i + 1, j )] == CELL_FLUID ))
                ++adjacent_fluid_cells;
            if( i < grid_width - 1  && ( cell_type[ix( i + 1, j )] == CELL_SOLID ))
                ++adjacent_solid_cells;

            if( j > 0 && ( cell_type[ix( i, j - 1)] == CELL_FLUID ))
                ++adjacent_fluid_cells;
            if( j > 0 && ( cell_type[ix( i, j - 1)] == CELL_SOLID ))
                ++adjacent_solid_cells;

            if( j < grid_height - 1 && ( cell_type[ix( i, j + 1)] == CELL_FLUID ))
                ++adjacent_fluid_cells;
            if( j < grid_height - 1 && ( cell_type[ix( i, j + 1)] == CELL_SOLID ))
                ++adjacent_solid_cells;

            if( adjacent_fluid_cells >= 3 || ( adjacent_fluid_cells + adjacent_solid_cells >= 4 ))
                cell_type_aux[ix_] = CELL_FLUID;
		}
	}

	for( int j = 0; j < grid_height; j++ )
	for( int i = 0; i < grid_width ; i++ )
	{
	    int ix_ = ix( i, j );

	    if( cell_type_aux[ix_] == CELL_FLUID )
            cell_type[ix_] = CELL_FLUID;
	}
}

void FLIPSolver2D::gridToParticles()
{
	for( int p = 0; p < particles.np; p++ )
	{
		float i_p = particles.x[p].x * over_dx;
		float j_p = particles.x[p].y * over_dx;

		// PIC

		float u_pic = uVel( i_p, j_p );
		float v_pic = vVel( i_p, j_p );

		// FLIP

		swapVel();

		float u_flip = particles.u[p].x + uVel( i_p, j_p );
		float v_flip = particles.u[p].y + vVel( i_p, j_p );

		swapVel();

		// Lerp between both to control numerical viscosity

		particles.u[p].x = pic_flip_factor * u_pic + ( 1.0f - pic_flip_factor ) * u_flip;
		particles.u[p].y = pic_flip_factor * v_pic + ( 1.0f - pic_flip_factor ) * v_flip;
//		particles.q[p]   = 0.05f * bilerp( i_p, j_p, q ) + 0.95f * particles.q[p];
	}

	for( int j = 0; j < grid_height; j++ )
	for( int i = 0; i < grid_width ; i++ )
	{
		int ix_ = ix( i, j );

        cell_type_aux[ix_] = CELL_AIR;

		if( cell_type[ix_] == CELL_AIR )
		{
            int adjacent_fluid_cells = 0;
            int adjacent_solid_cells = 0;

            if( i > 0 && ( cell_type[ix( i - 1, j )] == CELL_FLUID ))
                ++adjacent_fluid_cells;
            if( i > 0 && ( cell_type[ix( i - 1, j )] == CELL_SOLID ))
                ++adjacent_solid_cells;

            if( i < grid_width - 1  && ( cell_type[ix( i + 1, j )] == CELL_FLUID ))
                ++adjacent_fluid_cells;
            if( i < grid_width - 1  && ( cell_type[ix( i + 1, j )] == CELL_SOLID ))
                ++adjacent_solid_cells;

            if( j > 0 && ( cell_type[ix( i, j - 1)] == CELL_FLUID ))
                ++adjacent_fluid_cells;
            if( j > 0 && ( cell_type[ix( i, j - 1)] == CELL_SOLID ))
                ++adjacent_solid_cells;

            if( j < grid_height - 1 && ( cell_type[ix( i, j + 1)] == CELL_FLUID ))
                ++adjacent_fluid_cells;
            if( j < grid_height - 1 && ( cell_type[ix( i, j + 1)] == CELL_SOLID ))
                ++adjacent_solid_cells;

            if( adjacent_fluid_cells >= 3 || ( adjacent_fluid_cells + adjacent_solid_cells >= 4 ))
                cell_type_aux[ix_] = CELL_FLUID;
		}
	}

	for( int j = 0; j < grid_height; j++ )
	for( int i = 0; i < grid_width ; i++ )
	{
	    int ix_ = ix( i, j );

	    if( cell_type_aux[ix_] == CELL_FLUID )
            cell_type[ix_] = CELL_FLUID;
	}
}

void FLIPSolver2D::storeVel()
{
	memcpy( du, u_, u_size * sizeof( float ));
	memcpy( dv, v_, v_size * sizeof( float ));
}

void FLIPSolver2D::subtractVel()
{
	for( int i = 0; i < u_size; i++ )
		du[i] = u_[i] - du[i];

	for( int i = 0; i < v_size; i++ )
		dv[i] = v_[i] - dv[i];
}

float FLIPSolver2D::computePhi( float a, float b, float current )
{
	float dif = a - b;

	if( fabs( dif ) >= dx )
		return min_( a, b ) + dx;

	return min_( current, ( a + b + sqrt( 2 * dx * dx - dif * dif )) / 2.0f );

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

	float max_dim = ( float )( grid_width + grid_height + 2 );

	for( int j = 0; j < grid_height; j++ )
	for( int i = 0; i < grid_width ; i++ )
	{
		int ix_ = ix( i, j );

		if( cell_type[ix_] == CELL_FLUID )
			phi[ix_] = -0.5f;
		else
			phi[ix_] = max_dim;
	}

	for( int r = 0; r < 2; r++ )
	{
		// Sweep in all directions

		for( int j = 1; j < grid_height; j++ )
		for( int i = 1; i < grid_width; i++ )
		{
			int ix_ = ix( i, j );

			if( cell_type[ix_] != CELL_FLUID )
				phi[ix_] = computePhi( phi[ix( i - 1, j )], phi[ix( i, j - 1 )], phi[ix_] );
		}

		for( int j = grid_height - 2; j >= 0		; j-- )
		for( int i = 1				; i < grid_width; i++ )
		{
			int ix_ = ix( i, j );

			if( cell_type[ix_] != CELL_FLUID )
				phi[ix_] = computePhi( phi[ix( i - 1, j )], phi[ix( i, j + 1 )], phi[ix_] );
		}

		for( int j = 1				; j < grid_height; j++ )
		for( int i = grid_width - 2	; i >= 0		 ; i-- )
		{
			int ix_ = ix( i, j );

			if( cell_type[ix_] != CELL_FLUID )
				phi[ix_] = computePhi( phi[ix( i + 1, j )], phi[ix( i, j - 1 )], phi[ix_] );
		}

		for( int j = grid_height - 2; j >= 0; j-- )
		for( int i = grid_width  - 2; i >= 0; i-- )
		{
			int ix_ = ix( i, j );

			if( cell_type[ix_] != CELL_FLUID )
				phi[ix_] = computePhi( phi[ix( i + 1, j )], phi[ix( i, j + 1 )], phi[ix_] );
		}
	}
}

void FLIPSolver2D::sweepU( int i0, int i1, int j0, int j1 )
{
	int di = ( i0 < i1 ) ? 1 : -1;
	int dj = ( j0 < j1 ) ? 1 : -1;

	float dp, dq, alpha;

	for( int j = j0; j != j1; j += dj )
	for( int i = i0; i != i1; i += di )
	{
		if( cell_type[ix( i - 1, j)] == CELL_AIR && cell_type[ix( i, j )] == CELL_AIR )
		{
			dp = di * ( phi[ix( i, j )] - phi[ix( i - 1, j )]);

			if( dp < 0 )
				continue;

			dq = 0.5f * ( phi[ix( i - 1, j )] + phi[ix( i, j )] - phi[ix( i - 1, j - dj )] - phi[ix( i, j - dj )]);

			if( dq < 0 )
				continue;

			if( dp + dq == 0 )
				alpha = 0.5f;
			else
				alpha = dp / ( dp + dq );

			u( i, j ) = alpha * u( i - di, j ) + ( 1.0f - alpha ) * u( i, j - dj );
		}

		/*if( cell_type[ix( i + di, j)] == CELL_AIR && cell_type[ix( i, j )] == CELL_AIR )
		{
			dp = di * ( phi[ix( i, j )] - phi[ix( i + di, j )]);

			if( dp < 0 )
				continue;

			dq = 0.5f * ( phi[ix( i + di, j )] + phi[ix( i, j )] - phi[ix( i + di, j - dj )] - phi[ix( i, j - dj )]);

			if( dq < 0 )
				continue;

			if( dp + dq == 0 )
				alpha = 0.5f;
			else
				alpha = dp / ( dp + dq );

			u( i, j ) = alpha * u( i - di, j ) + ( 1.0f - alpha ) * u( i, j - dj );
		}*/
	}
}

void FLIPSolver2D::sweepV( int i0, int i1, int j0, int j1 )
{
	int di = ( i0 < i1 ) ? 1 : -1;
	int dj = ( j0 < j1 ) ? 1 : -1;

	float dp, dq, alpha;

	for( int j = j0; j != j1; j += dj )
	for( int i = i0; i != i1; i += di )
	{
		if( cell_type[ix( i, j - 1 )] == CELL_AIR && cell_type[ix( i, j )] == CELL_AIR )
		{
			dq = dj * ( phi[ix( i, j )] - phi[ix( i, j - 1 )]);

			if( dq < 0 )
				continue;

			dp = 0.5f * ( phi[ix( i, j - 1 )] + phi[ix( i, j )] - phi[ix( i - di, j - 1 )] - phi[ix( i - di, j )]);

			if( dp < 0 )
				continue;

			if( dp + dq == 0 )
				alpha = 0.5f;
			else
				alpha = dp / ( dp + dq );

			v( i, j ) = alpha * v( i - di, j ) + ( 1.0f - alpha ) * v( i, j - dj );
		}

		/*if( cell_type[ix( i, j + dj )] == CELL_AIR && cell_type[ix( i, j )] == CELL_AIR )
		{
			dq = dj * ( phi[ix( i, j )] - phi[ix( i, j + dj )]);

			if( dq < 0 )
				continue;

			dp = 0.5f * ( phi[ix( i, j + dj )] + phi[ix( i, j )] - phi[ix( i - di, j + dj )] - phi[ix( i - di, j )]);

			if( dp < 0 )
				continue;

			if( dp + dq == 0 )
				alpha = 0.5f;
			else
				alpha = dp / ( dp + dq );

			v( i, j ) = alpha * v( i - di, j ) + ( 1.0f - alpha ) * v( i, j - dj );
		}*/
	}
}

void FLIPSolver2D::extrapolateVel()
{
	for( int r = 0; r < 4; r++ )
	{
		sweepU( 1				, grid_width, 1					, grid_height - 1	);
		sweepU( 1				, grid_width, grid_height - 2	, 0					);
		sweepU( grid_width - 1	, 0			, 1					, grid_height - 1	);
		sweepU( grid_width - 1	, 0			, grid_height - 2	, 0					);

		for( int i = 0; i < grid_width + 1; i++ )
		{
			u( i, 0 )				= u( i, 1 );
			u( i, grid_height - 1 )	= u( i, grid_height - 2 );
		}
		for( int j = 0; j < grid_height; j++ )
		{
			u( 0			, j ) = u( 1				, j );
			u( grid_width	, j ) = u( grid_width - 1	, j );
		}

		sweepV( 1				, grid_width - 1, 1					, grid_height	);
		sweepV( 1				, grid_width - 1, grid_height - 1	, 0				);
		sweepV( grid_width - 2	, 0				, 1					, grid_height	);
		sweepV( grid_width - 2	, 0				, grid_height - 1	, 0				);

		for( int i = 0; i < grid_width; i++ )
		{
			v( i, 0 )			= v( i, 1 );
			v( i, grid_height )	= v( i, grid_height - 1 );
		}

		for( int j = 0; j < grid_height + 1; j++ )
		{
			v( 0				, j ) = v( 1				, j );
			v( grid_width - 1	, j ) = v( grid_width - 2	, j );
		}
	}
}

void FLIPSolver2D::project( float dt )
{
	// Set the right hand side of the equation system

	rhs.clear();

	for( int j = 0; j < grid_height	; j++ )
	for( int i = 0; i < grid_width	; i++ )
	{
		int ix_ = i + j * grid_width;

		if( cell_type[ix_] == CELL_FLUID )
		{
			rhs[ix_] = ( u( i + 1, j ) - u( i, j ) + v( i, j + 1 ) - v( i, j ));

			if( i > 0 && cell_type[ix( i - 1, j )] == CELL_SOLID )
				rhs[ix_] -= ( u_solid - u( i, j ));
			if( i < grid_width - 1 && cell_type[ix( i + 1, j )] == CELL_SOLID )
				rhs[ix_] -= ( u( i + 1, j ) - u_solid );
			if( j > 0 && cell_type[ix( i, j - 1 )] == CELL_SOLID )
				rhs[ix_] -= ( v_solid - v( i, j ));
			if( j < grid_height - 1 && cell_type[ix( i, j + 1 )] == CELL_SOLID )
				rhs[ix_] -= ( v( i, j + 1 ) - v_solid );
		}
	}

	// Set the coefficients

	coef_diag	.clear();
	coef_plus_i	.clear();
	coef_plus_j	.clear();

	for( int j = 0; j < grid_height; j++ )
	for( int i = 0; i < grid_width ; i++ )
	{
		if( cell_type[ix( i, j )] == CELL_FLUID )
		{
			if( cell_type[ix( i + 1, j )] != CELL_SOLID )
			{
				coef_diag[ix( i, j )] += 1.0f;

				if( cell_type[ix( i + 1, j )] == CELL_FLUID )
					coef_plus_i[ix( i, j )] = -1.0f;
			}

			if( cell_type[ix( i - 1, j )] != CELL_SOLID )
				coef_diag[ix( i, j )] += 1.0f;

			if( cell_type[ix( i, j + 1 )] != CELL_SOLID )
			{
				coef_diag[ix( i, j )] += 1.0f;

				if( cell_type[ix( i, j + 1 )] == CELL_FLUID )
					coef_plus_j[ix( i, j )] = -1.0f;
			}

			if( cell_type[ix( i, j - 1 )] != CELL_SOLID )
				coef_diag[ix( i, j )] += 1.0f;
		}
	}

	// Solve for pressure with PCG algorithm

	CTimer project_timer( TIMER_HIGH_PRECISION );

	project_timer.start();

	solvePressure();

	project_time = project_timer.getTimeElapsedMilliSec();

	// Apply pressure to update velocity

	/*for( int j = 1; j < grid_height - 1 ; j++ )
	for( int i = 2; i < grid_width	- 1	; i++ )
	{
		int ix		= i + j * grid_width;
		int ix_i	= i - 1 + j * grid_width;

		if(( cell_type[ix_i] | cell_type[ix] ) == CELL_FLUID )
			u( i, j ) += ( float )( p[ix] - p[ix_i] );
	}

	for( int j = 2; j < grid_height - 1 ; j++ )
	for( int i = 1; i < grid_width	- 1	; i++ )
	{
		int ix		= i + j * grid_width;
		int ix_j	= i + ( j - 1 ) * grid_width;

		if(( cell_type[ix_j] | cell_type[ix] ) == CELL_FLUID )
			v( i, j ) += ( float )( p[ix] - p[ix_j] );
	}*/

	for( int j = 0; j < grid_height; j++ )
	for( int i = 0; i < grid_width ; i++ )
	{
		int ix_ = i + j * grid_width;

		if(( cell_type[ix_] ) == CELL_FLUID )
		{
			u( i	, j ) += ( float )p[ix_];
			u( i + 1, j ) -= ( float )p[ix_];
			v( i	, j	) += ( float )p[ix_];
			v( i, j + 1 ) -= ( float )p[ix_];
		}
	}

	setBoundary();
}

void FLIPSolver2D::solvePressure()
{
	p.clear();

	double r_max = 0.0f;

	for( unsigned int i = 0; i < rhs.size(); i++ )
	{
		if( !( fabs( rhs[i] ) <= r_max ))
			r_max = fabs( rhs[i] );
	}

	if( r_max == 0.0 )
		return;

    r = rhs;

	double tolerance = PCG_TOLERANCE * r_max;

	calcPrecond	();

/*FILE* f_out = fopen( "output.txt", "a" );
for( int i = 0; i < rhs.size(); i++ )
{
    fprintf( f_out, "rhs[%d] : %.05f, coef_diag[%d] : %.05f, coef_plus_i[%d] : %.05f, coef_plus_j[%d] : %.05f, precond[%d] : %.05f \n", i, rhs[i], i, coef_diag[i], i, coef_plus_i[i], i, coef_plus_j[i], i, precond[i] );
}
fclose( f_out );*/

	applyPrecond();

	s = z;

	double sigma = inner_prod( z, r );

	if( sigma == 0.0 )
		return;

	for( int i = 0; i < PCG_MAX_ITERATIONS; i++ )
	{
		applyA();

		double alpha = sigma / inner_prod( z, s );

		p += ( alpha * s );
		r -= ( alpha * z );

        double error = 0.0f;

		for( int j = 0; j < grid_width * grid_height; j++ )
		{
			double abs_r = fabs( r[j] );

			if( !( abs_r <= error ))
				error = abs_r;
		}

		if( error <= tolerance )
		{
			//printf( "Tolerance %g reached at iteration: %d\n", error, i );
			break;
		}

		applyPrecond();

		double sigma_new = inner_prod( z, r );
		double beta		 = sigma_new / sigma;

		s = z + beta * s;

		sigma = sigma_new;
	}
}

void FLIPSolver2D::calcPrecond()
{
	double tuning_const = 0.99;
	double safety_const = 0.25;

	precond.clear();

	for( int j = 0; j < grid_height	; j++ )
	for( int i = 0; i < grid_width	; i++ )
	{
		int ix = i + j * grid_width;

		if( cell_type[ix] == CELL_FLUID )
		{
			int ix_plus_i = ( i - 1 ) + j * grid_width;
			int ix_plus_j = i + ( j - 1 ) * grid_width;

			precond[ix] = coef_diag[ix];

			if( i > 0 )
			{
				double plus_i = coef_plus_i[ix_plus_i] * precond[ix_plus_i];
				precond[ix] += ( -plus_i * plus_i - tuning_const *
							   ( coef_plus_i[ix_plus_i] * coef_plus_j[ix_plus_i] * precond[ix_plus_i] * precond[ix_plus_i] ));
			}
			if( j > 0 )
			{
				double plus_j = coef_plus_j[ix_plus_j] * precond[ix_plus_j];
				precond[ix] += ( -plus_j * plus_j - tuning_const *
							   ( coef_plus_j[ix_plus_j] * coef_plus_i[ix_plus_j] * precond[ix_plus_j] * precond[ix_plus_j] ));
			}

			if( precond[ix] < safety_const * coef_diag[ix] )
				precond[ix] = coef_diag[ix];

			precond[ix] = 1.0 / sqrt( precond[ix] + PCG_EPSILON );
		}
	}
}

void FLIPSolver2D::applyPrecond()
{
	aux.clear();

	for( int j = 0; j < grid_height	; j++ )
	for( int i = 0; i < grid_width	; i++ )
	{
		int ix = i + j * grid_width;

		if( cell_type[ix] == CELL_FLUID )
		{
			int ix_plus_i = ( i - 1 ) + j * grid_width;
			int ix_plus_j = i + ( j - 1 ) * grid_width;

			double t = r[ix];

			if( i > 0 )
				t -= ( coef_plus_i[ix_plus_i] * precond[ix_plus_i] * aux[ix_plus_i] );
			if( j > 0 )
				t -= ( coef_plus_j[ix_plus_j] * precond[ix_plus_j] * aux[ix_plus_j] );

			aux[ix] = t * precond[ix];
		}
	}

	z.clear();

	for( int j = grid_height; j--; )
	for( int i = grid_width ; i--; )
	{
		int ix = i + j * grid_width;

		if( cell_type[ix] == CELL_FLUID )
		{
			int ix_plus_i = ( i + 1 ) + j * grid_width;
			int ix_plus_j = i + ( j + 1 ) * grid_width;

			double t = aux[ix];

			if( i < grid_width - 1 )
				t -= ( coef_plus_i[ix] * precond[ix] * z[ix_plus_i] );
			if( j < grid_height - 1 )
				t -= ( coef_plus_j[ix] * precond[ix] * z[ix_plus_j] );

			z[ix] = t * precond[ix];
		}
	}
}

void FLIPSolver2D::applyA()
{
	z.clear();

	for( int j = 0; j < grid_height	; j++ )
	for( int i = 0; i < grid_width	; i++ )
	{
		int ix = i + j * grid_width;

		if( cell_type[ix] == CELL_FLUID )
		{
			int ix_plus_i	= ( i + 1 ) + j * grid_width;
			int ix_plus_j	= i + ( j + 1 ) * grid_width;
			int ix_minus_i	= ( i - 1 ) + j * grid_width;
			int ix_minus_j	= i + ( j - 1 ) * grid_width;

			z[ix] = s[ix] * coef_diag[ix];

			if( i > 0 )
				z[ix] += s[ix_minus_i] * coef_plus_i[ix_minus_i];
			if( j > 0 )
				z[ix] += s[ix_minus_j] * coef_plus_j[ix_minus_j];
			if( i < grid_width - 1 )
				z[ix] += s[ix_plus_i] * coef_plus_i[ix];
			if( j < grid_height - 1 )
				z[ix] += s[ix_plus_j] * coef_plus_j[ix];
		}
	}
}

// DEBUG: PRINT COEF. MATRIX
/*
for( int j = 0; j < grid_height	; j++ )
for( int i = 0; i < grid_width	; i++ )
{
	int ix			= i + j * grid_width;
	int ix_plus_i	= ( i + 1 ) + j * grid_width;
	int ix_plus_j	= i + ( j + 1 ) * grid_width;

	for( int k = 0; k < grid_width * grid_height; k++ )
	{
		if( k == ix )
			printf( "%02g ", coef_diag[ix] );
		else if( k == ix_plus_i )
			printf( "%02g ", coef_plus_i[ix] );
		else if( k == ix_plus_j )
			printf( "%02g ", coef_plus_j[ix] );
		else
			printf( "%02g ", 0.0f );
	}

	printf( "\n" );
}
*/
