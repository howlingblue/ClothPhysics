#include "../Engine/DebugDrawing.hpp"
#include "Cloth.hpp"
#include "IntegrationMethods.hpp"

static const float STRUCTURAL_STIFFNESS_COEFFICIENT = 12.f;
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

		if( i % particlesPerX < particlesPerX - 1 )
		{
			Particle& particleOneEastOfThis = *m_particles[ i + 1 ];
			Constraint structuralConstraint1( thisParticle, particleOneEastOfThis, STRUCTURAL_STIFFNESS_COEFFICIENT );
			m_constraints.push_back( structuralConstraint1 );
		}

		if( i < m_particles.size() - particlesPerX )
		{
			Particle& particleOneSouthOfThis = *m_particles[ i + particlesPerX ];
			Constraint structuralConstraint2( thisParticle, particleOneSouthOfThis, STRUCTURAL_STIFFNESS_COEFFICIENT );
			m_constraints.push_back( structuralConstraint2 );
		}

		if( i % particlesPerX < particlesPerX - 1 && i < m_particles.size() - particlesPerX )
		{
			Particle& particleOneEastOfThis = *m_particles[ i + 1 ];
			Particle& particleOneSouthOfThis = *m_particles[ i + particlesPerX ];
			Particle& particleSouthEastOfThis = *m_particles[ i + particlesPerX + 1 ];
			
			Constraint shearConstraint1( thisParticle, particleSouthEastOfThis, SHEAR_STIFFNESS_COEFFICIENT );
			m_constraints.push_back( shearConstraint1 );

			Constraint shearConstraint2( particleOneEastOfThis, particleOneSouthOfThis, SHEAR_STIFFNESS_COEFFICIENT );
			m_constraints.push_back( shearConstraint2 );
		}

 		if( i % particlesPerX < particlesPerX - 2 )
		{
			Particle& particleTwoEastOfThis = *m_particles[ i + 2 ];
			Constraint bendingConstraint1( thisParticle, particleTwoEastOfThis, BENDING_STIFFNESS_COEFFICIENT );
			m_constraints.push_back( bendingConstraint1 );
		}


		if( i < m_particles.size() - 2 * particlesPerX )
		{
			Particle& particleTwoSouthOfThis = *m_particles[ i + 2 * particlesPerX ];

			Constraint bendingConstraint2( thisParticle, particleTwoSouthOfThis, BENDING_STIFFNESS_COEFFICIENT );
			m_constraints.push_back( bendingConstraint2 );
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Cloth::Render() const
{
	static const Color YELLOW = Color( 1.f, 1.f, 0.f, 1.f );
	static const Color BLUE = Color( 0.f, 0.f, 1.f, 1.f );
	static const Color GREEN = Color( 0.f, 1.f, 0.f, 1.f );
	static const Color WHITE = Color( 1.f, 1.f, 1.f, 1.f );
	for( unsigned int i = 0; i < m_particles.size(); ++i )
	{
		const Particle& particle = *m_particles[ i ];

		Debug::DrawPoint( particle.currentPosition, 0.5f, WHITE, Debug::Drawing::DRAW_ALWAYS );
	}

	Color constraintColor = WHITE;
	for( unsigned int i = 0; i < m_constraints.size(); ++i )
	{
		const Constraint& constraint = m_constraints[ i ];

		if( constraint.stiffnessCoefficient == STRUCTURAL_STIFFNESS_COEFFICIENT )
			constraintColor = GREEN;
		if( constraint.stiffnessCoefficient == SHEAR_STIFFNESS_COEFFICIENT )
			constraintColor = YELLOW;
		if( constraint.stiffnessCoefficient == BENDING_STIFFNESS_COEFFICIENT )
			constraintColor = BLUE;

		Debug::DrawLine( constraint.particle1->currentPosition, constraintColor, constraint.particle2->currentPosition, constraintColor, Debug::Drawing::DRAW_ALWAYS );
	}
}

//-----------------------------------------------------------------------------------------------
void Cloth::Update( float deltaSeconds )
{
	ClearParticleAccelerations();

	for( unsigned int i = 0; i < m_constraints.size(); ++i )
	{
		ApplyForceToParticlesFromConstraint( m_constraints[ i ] );
	}

	for( unsigned int i = 0; i < m_particles.size(); ++i )
	{
		Particle& particle = *m_particles[ i ];

		if( particle.positionIsLocked )
			continue;

		FloatVector3 forceOnParticle;

		//Gravity
		forceOnParticle.z += -2.4f;

		//Drag
		forceOnParticle += ( -m_dragCoefficient * particle.previousVelocity );

		particle.acceleration += forceOnParticle / particle.mass;

		verletLeapFrogIntegrationMassSpringDamper( *this, particle, deltaSeconds );
	}
}