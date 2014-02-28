#include "../Engine/DebugDrawing.hpp"
#include "Cloth.hpp"
#include "IntegrationMethods.hpp"

static const float STIFFNESS_COEFFICIENT = 1.f;

//-----------------------------------------------------------------------------------------------s
void Cloth::GenerateParticleGrid( unsigned int particlesPerX, unsigned int particlesPerY )
{
	FloatVector3 TOP_LEFT_CORNER( -5.f, 5.f, 0.f );
	for( unsigned int i = 0; i < particlesPerX; ++i )
	{
		for( unsigned int j = 0; j < particlesPerY; ++j )
		{
			Particle particle;
			particle.positionIsLocked = false;
			particle.currentPosition = FloatVector3( TOP_LEFT_CORNER.x + static_cast< float >( 2 * i ),
													 TOP_LEFT_CORNER.y + static_cast< float >( 2 * j ),
													 0.f );
			particle.previousPosition = particle.currentPosition;
			particle.mass = 0.2f;
			m_particles.push_back( particle );
		}
	}

	m_particles[ 0 ].positionIsLocked = true;
	m_particles[ m_particles.size() - 1 ].positionIsLocked = true;

	for( unsigned int i = 0; i < m_particles.size(); ++i )
	{
		Particle& thisParticle = m_particles[ i ];

		if( i % particlesPerX < particlesPerX - 1 )
		{
			Particle& particleOneEastOfThis = m_particles[ i + 1 ];
			Constraint structuralConstraint1( thisParticle, particleOneEastOfThis, STIFFNESS_COEFFICIENT );
			m_constraints.push_back( structuralConstraint1 );
		}

		if( i < m_particles.size() - particlesPerX )
		{
			Particle& particleOneSouthOfThis = m_particles[ i + particlesPerX ];
			Constraint structuralConstraint2( thisParticle, particleOneSouthOfThis, STIFFNESS_COEFFICIENT );
			m_constraints.push_back( structuralConstraint2 );
		}

		if( i % particlesPerX < particlesPerX - 1 && i < m_particles.size() - particlesPerX )
		{
			Particle& particleOneEastOfThis = m_particles[ i + 1 ];
			Particle& particleOneSouthOfThis = m_particles[ i + particlesPerX ];
			Particle& particleSouthEastOfThis = m_particles[ i + particlesPerX + 1 ];
			
			Constraint shearConstraint1( thisParticle, particleSouthEastOfThis, STIFFNESS_COEFFICIENT );
			m_constraints.push_back( shearConstraint1 );

			Constraint shearConstraint2( particleOneEastOfThis, particleOneSouthOfThis, STIFFNESS_COEFFICIENT );
			m_constraints.push_back( shearConstraint2 );
		}

 		if( i % particlesPerX < particlesPerX - 2 )
		{
			Particle& particleTwoEastOfThis = m_particles[ i + 2 ];
			Constraint bendingConstraint1( thisParticle, particleTwoEastOfThis, STIFFNESS_COEFFICIENT );
			m_constraints.push_back( bendingConstraint1 );
		}


		if( i < m_particles.size() - 2 * particlesPerX )
		{
			Particle& particleTwoSouthOfThis = m_particles[ i + 2 * particlesPerX ];

			Constraint bendingConstraint2( thisParticle, particleTwoSouthOfThis, STIFFNESS_COEFFICIENT );
			m_constraints.push_back( bendingConstraint2 );
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Cloth::Render() const
{
	static const Color WHITE = Color( 1.f, 1.f, 1.f, 1.f );
	static const Color GREY = Color( 0.5f, 0.5f, 0.5f, 1.f );
	for( unsigned int i = 0; i < m_particles.size(); ++i )
	{
		const Particle& particle = m_particles[ i ];

		Debug::DrawPoint( particle.currentPosition, 0.5f, GREY, Debug::Drawing::DRAW_ALWAYS );
	}

	for( unsigned int i = 0; i < m_constraints.size(); ++i )
	{
		const Constraint& constraint = m_constraints[ i ];

		Debug::DrawLine( constraint.particle1->currentPosition, WHITE, constraint.particle2->currentPosition, WHITE, Debug::Drawing::DRAW_ALWAYS );
	}
}

//-----------------------------------------------------------------------------------------------
void Cloth::Update( float deltaSeconds )
{
	for( unsigned int i = 0; i < m_particles.size(); ++i )
	{
		Particle& particle = m_particles[ i ];

		if( particle.positionIsLocked )
			continue;

		particle.acceleration.z = -.98f;

		verletLeapFrogIntegrationMassSpringDamper( *this, particle, deltaSeconds );
	}
}