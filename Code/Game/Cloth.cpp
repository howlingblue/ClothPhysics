#include "../Engine/DebugDrawing.hpp"
#include "../Engine/Graphics/Renderer.hpp"
#include "../Engine/Graphics/VertexDataContainers.hpp"
#include "Cloth.hpp"
#include "IntegrationMethods.hpp"

static const float STRUCTURAL_STIFFNESS_COEFFICIENT = 8.f;
static const float SHEAR_STIFFNESS_COEFFICIENT = 6.f;
static const float BENDING_STIFFNESS_COEFFICIENT = 7.f;

//-----------------------------------------------------------------------------------------------
void Cloth::ClearParticleAccelerations()
{
	for( unsigned int i = 0; i < m_particles.size(); ++i )
	{
		m_particles[ i ]->acceleration.x = 0.f;
		m_particles[ i ]->acceleration.y = 0.f;
		m_particles[ i ]->acceleration.z = 0.f;
	}
}

//-----------------------------------------------------------------------------------------------
void Cloth::ClearParticleNormals()
{
	for( unsigned int i = 0; i < m_particles.size(); ++i )
	{
		m_particles[ i ]->acceleration.x = 0.f;
		m_particles[ i ]->acceleration.y = 0.f;
		m_particles[ i ]->acceleration.z = 0.f;
	}
}

//-----------------------------------------------------------------------------------------------
void Cloth::GenerateClothNormals()
{
	ClearParticleNormals();

	for( unsigned int i = 0; i < m_particles.size() - m_particlesPerX; ++i )
	{
		int numberOfNeighboringTriangles = 0;
	}
}

//-----------------------------------------------------------------------------------------------
void Cloth::GenerateParticleGrid( unsigned int particlesPerX, unsigned int particlesPerY )
{
	FloatVector3 TOP_LEFT_CORNER( -5.f, 5.f, 0.f );
	for( unsigned int i = 0; i < particlesPerX; ++i )
	{
		for( unsigned int j = 0; j < particlesPerY; ++j )
		{
			m_particles.push_back( new Particle( FloatVector3( TOP_LEFT_CORNER.x + static_cast< float >( 2 * i ), TOP_LEFT_CORNER.y + static_cast< float >( 2 * j ), 0.f ), false, 0.2f ) );
		}
	}

	m_particles[ 0 ]->positionIsLocked = true;
	m_particles[ particlesPerX - 1 ]->positionIsLocked = true;
	m_particles[ m_particles.size() - particlesPerX ]->positionIsLocked = true;
	m_particles[ m_particles.size() - 1 ]->positionIsLocked = true;

	for( unsigned int i = 0; i < m_particles.size(); ++i )
	{
		Particle& thisParticle = *m_particles[ i ];

		if( indexIsNotOnRightEdge( i ) )
		{
			Particle& particleOneEastOfThis = *m_particles[ i + 1 ];
			m_structuralConstraints.push_back( Constraint( thisParticle, particleOneEastOfThis ) );
		}

		if( indexIsNotOnBottomEdge( i ) )
		{
			Particle& particleOneSouthOfThis = *m_particles[ i + particlesPerX ];
			m_structuralConstraints.push_back( Constraint( thisParticle, particleOneSouthOfThis ) );
		}

		if( indexIsNotOnRightEdge( i ) && indexIsNotOnBottomEdge( i ) )
		{
			Particle& particleOneEastOfThis = *m_particles[ i + 1 ];
			Particle& particleOneSouthOfThis = *m_particles[ i + particlesPerX ];
			Particle& particleSouthEastOfThis = *m_particles[ i + particlesPerX + 1 ];
			
			m_shearConstraints.push_back( Constraint( thisParticle, particleSouthEastOfThis ) );

			m_shearConstraints.push_back( Constraint( particleOneEastOfThis, particleOneSouthOfThis ) );
		}

 		if( i % particlesPerX < particlesPerX - 2 )
		{
			Particle& particleTwoEastOfThis = *m_particles[ i + 2 ];

			m_bendingConstraints.push_back( Constraint( thisParticle, particleTwoEastOfThis ) );
		}


		if( i < m_particles.size() - 2 * particlesPerX )
		{
			Particle& particleTwoSouthOfThis = *m_particles[ i + 2 * particlesPerX ];

			m_bendingConstraints.push_back( Constraint( thisParticle, particleTwoSouthOfThis ) );
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Cloth::GenerateVertexAndIndexArray( VertexColorNormalTextureData* out_nullVertexArray, unsigned int& out_numberOfVertices,
										 unsigned short* out_nullIndexArray, unsigned int& out_numberOfIndices )
{
	static const unsigned int VERTICES_PER_CLOTH_SQUARE = 6;
	static const Color WHITE = Color( 1.f, 1.f, 1.f, 1.f );

	out_numberOfVertices = VERTICES_PER_CLOTH_SQUARE * 10;
	out_numberOfIndices = out_numberOfVertices + m_particlesPerY;

	out_nullVertexArray = new VertexColorNormalTextureData[ out_numberOfVertices ];

	//Bind every particle except the last line
	for( unsigned int i = 0; i < 10 * VERTICES_PER_CLOTH_SQUARE; ++i )
	{
		if( i % m_particlesPerX < m_particlesPerX - 1 )
		{
			Particle& thisParticle = *m_particles[ i ];
			Particle& particleOneEastOfThis = *m_particles[ GetIndexOfParticleEastOf( i ) ];
			Particle& particleOneSouthOfThis = *m_particles[ GetIndexOfParticleSouthOf( i ) ];
			Particle& particleSouthEastOfThis = *m_particles[ GetIndexOfParticleSoutheastOf( i ) ];

			out_nullVertexArray[ VERTICES_PER_CLOTH_SQUARE * i + 0 ] = VertexColorNormalTextureData( particleOneSouthOfThis.currentPosition, WHITE, particleOneSouthOfThis.normal, FloatVector2( 0.f, 0.f ) );
			out_nullVertexArray[ VERTICES_PER_CLOTH_SQUARE * i + 1 ] = VertexColorNormalTextureData( particleSouthEastOfThis.currentPosition, WHITE, particleSouthEastOfThis.normal, FloatVector2( 0.f, 0.f ) );
			out_nullVertexArray[ VERTICES_PER_CLOTH_SQUARE * i + 2 ] = VertexColorNormalTextureData( thisParticle.currentPosition, WHITE, thisParticle.normal, FloatVector2( 0.f, 0.f ) );
			out_nullVertexArray[ VERTICES_PER_CLOTH_SQUARE * i + 3 ] = VertexColorNormalTextureData( particleOneEastOfThis.currentPosition, WHITE, particleOneEastOfThis.normal, FloatVector2( 0.f, 0.f ) );
			out_nullVertexArray[ VERTICES_PER_CLOTH_SQUARE * i + 4 ] = out_nullVertexArray[ VERTICES_PER_CLOTH_SQUARE * i + 3 ];
			out_nullVertexArray[ VERTICES_PER_CLOTH_SQUARE * i + 5 ] = out_nullVertexArray[ VERTICES_PER_CLOTH_SQUARE * i + 2 ];

		}
	}

	out_nullIndexArray = new unsigned short[ out_numberOfIndices ];
}

//-----------------------------------------------------------------------------------------------
void Cloth::Render( bool drawInDebug ) const
{
	if( drawInDebug )
		RenderDebugParticlesAndConstraints();
}

//-----------------------------------------------------------------------------------------------
void Cloth::Update( float deltaSeconds, bool useConstraintSatisfaction )
{
	ClearParticleAccelerations();

	GenerateClothNormals();

	if( useConstraintSatisfaction )
	{
		// PR: Added loop to control how many times we want to satisfy the constraints
		for ( unsigned int i = 0; i < m_numberOfConstraintSatisfactionLoops; ++i )
		{
			for( unsigned int j = 0; j < m_structuralConstraints.size(); ++j )
			{
				SatisfyConstraint( m_structuralConstraints[ j ] );
			}

			for( unsigned int j = 0; j < m_shearConstraints.size(); ++j )
			{
				SatisfyConstraint( m_shearConstraints[ j ] );
			}

			for( unsigned int j = 0; j < m_bendingConstraints.size(); ++j )
			{
				SatisfyConstraint( m_bendingConstraints[ j ] );
			}
		}
	}
	else
	{
		for( unsigned int j = 0; j < m_structuralConstraints.size(); ++j )
		{
			ApplyForceToParticlesFromConstraint( m_structuralConstraints[ j ], STRUCTURAL_STIFFNESS_COEFFICIENT );
		}

		for( unsigned int j = 0; j < m_shearConstraints.size(); ++j )
		{
			ApplyForceToParticlesFromConstraint( m_shearConstraints[ j ], SHEAR_STIFFNESS_COEFFICIENT );
		}

		for( unsigned int j = 0; j < m_bendingConstraints.size(); ++j )
		{
			ApplyForceToParticlesFromConstraint( m_bendingConstraints[ j ], BENDING_STIFFNESS_COEFFICIENT );
		}
	}
	

	AddWindForce( m_windForce );

	for( unsigned int i = 0; i < m_particles.size(); ++i )
	{
		Particle& particle = *m_particles[ i ];

		if( particle.positionIsLocked )
			continue;

		FloatVector3 forceOnParticle;

		//Gravity
		forceOnParticle.z += -2.4f;

		//Drag
		forceOnParticle += ( -m_dragCoefficient * particle.currentVelocity );

		particle.acceleration += forceOnParticle / particle.mass;

		verletIntegration( particle, deltaSeconds );

	}

}


void Cloth::AddWindForce( const FloatVector3& directionOfWindForce ) {
	// PR :: Ensure we don't overstep with (-1)
	for ( size_t x = 0; x < ( m_particlesPerX - 1 ); ++x ) {
		for ( size_t y = 0; y < ( m_particlesPerY - 1 ); ++y ) {
			Particle & p1 = GetParticleAtPosition( x+1, y );
			Particle & p2 = GetParticleAtPosition( x, y );
			Particle & p3 = GetParticleAtPosition( x, y+1 );
			AddWindForcesForTriangle( p1, p2, p3, directionOfWindForce );
			Particle & pOne = GetParticleAtPosition( x+1, y+1 );
			Particle & pTwo = GetParticleAtPosition( x+1, y );
			Particle & pThree = GetParticleAtPosition( x, y+1 );
			AddWindForcesForTriangle( p1, p2, p3, directionOfWindForce );
		} // end inner for
	} // end outer for
}


void Cloth::AddWindForcesForTriangle( Particle& p1, Particle& p2, Particle& p3, const FloatVector3& direction ) {

	FloatVector3 normalOfTriangle = calculateTriangleNormal( p1, p2, p3 );
	FloatVector3 d = normalOfTriangle;
	d.Normalize();
	FloatVector3 force = normalOfTriangle * ( DotProduct( d, direction ) );
	p1.addExternalForceToParticle( force );
	p2.addExternalForceToParticle( force );
	p3.addExternalForceToParticle( force );

}


void Cloth::RenderDebugParticlesAndConstraints() const
{
	static const Color YELLOW = Color( 1.f, 1.f, 0.f, 1.f );
	static const Color BLUE = Color( 0.f, 0.f, 1.f, 1.f );
	static const Color GREEN = Color( 0.f, 1.f, 0.f, 1.f );
	static const Color WHITE = Color( 1.f, 1.f, 1.f, 1.f );

	for( unsigned int i = 0; i < m_particles.size(); ++i )
	{
		const Particle& particle = *m_particles[ i ];

		Debug::DrawPoint( particle.currentPosition, 0.5f, WHITE, Debug::DRAW_ALWAYS );
	}

	for( unsigned int i = 0; i < m_structuralConstraints.size(); ++i )
	{
		const Constraint& constraint = m_structuralConstraints[ i ];

		Debug::DrawLine( constraint.particle1->currentPosition, GREEN, constraint.particle2->currentPosition, GREEN, Debug::DRAW_ALWAYS );
	}

	for( unsigned int i = 0; i < m_shearConstraints.size(); ++i )
	{
		const Constraint& constraint = m_shearConstraints[ i ];

		Debug::DrawLine( constraint.particle1->currentPosition, YELLOW, constraint.particle2->currentPosition, YELLOW, Debug::DRAW_ALWAYS );
	}

	for( unsigned int i = 0; i < m_bendingConstraints.size(); ++i )
	{
		const Constraint& constraint = m_bendingConstraints[ i ];

		Debug::DrawLine( constraint.particle1->currentPosition, BLUE, constraint.particle2->currentPosition, BLUE, Debug::DRAW_ALWAYS );
	}
}

