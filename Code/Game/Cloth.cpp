#include "../Engine/DebugDrawing.hpp"
#include "Cloth.hpp"

static const int CLOTH_WIDTH = 10;
static const float STIFFNESS_COEFFICIENT = 1.f;

//-----------------------------------------------------------------------------------------------s
void Cloth::GenerateParticleGrid( unsigned int numberOfParticles )
{
	FloatVector3 TOP_LEFT_CORNER( -10.f, 10.f, 0.f );
	for( unsigned int i = 0; i < CLOTH_WIDTH; ++i )
	{
		for( unsigned int j = 0; j < CLOTH_WIDTH; ++j )
		{
			Particle particle;
			particle.currentPosition = FloatVector3( TOP_LEFT_CORNER.x + static_cast< float >( 2 * i ),
													 TOP_LEFT_CORNER.y + static_cast< float >( 2 * j ),
													 0.f );
			particle.previousPosition = particle.currentPosition;
			particle.mass = 0.2f;
			m_particles.push_back( particle );
		}
	}

	for( unsigned int i = 0; i < m_particles.size(); ++i )
	{
		//if we are on the right edge, don't connect us with the left edge
		if( i = CLOTH_WIDTH - 1 )
			continue;

		Particle& thisParticle = m_particles[ i ];
		Particle& particleOneEastOfThis = m_particles[ i + 1 ];
		Particle& particleTwoEastOfThis = m_particles[ i + 2 ];
		Particle& particleOneSouthOfThis = m_particles[ i + CLOTH_WIDTH ];
		Particle& particleTwoSouthOfThis = m_particles[ i + 2 * CLOTH_WIDTH ];
		Particle& particleSouthEastOfThis = m_particles[ i + CLOTH_WIDTH + 1 ];

		Constraint structuralConstraint1;
		structuralConstraint1.particle1 = &thisParticle;
		structuralConstraint1.particle2 = &particleOneEastOfThis;
		FloatVector3 vectorBetweenConstraintEnds = structuralConstraint1.particle1->currentPosition - structuralConstraint1.particle2->currentPosition;
		structuralConstraint1.relaxedLength = vectorBetweenConstraintEnds.CalculateNorm();
		structuralConstraint1.stiffnessCoefficient = STIFFNESS_COEFFICIENT;
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

		Debug::DrawPoint( particle.currentPosition, 0.5f, GREY, Debug::Drawing::DRAW_SKINNY_IF_OCCLUDED );
	}

	for( unsigned int i = 0; i < m_constraints.size(); ++i )
	{
		const Constraint& constraint = m_constraints[ i ];

		Debug::DrawLine( constraint.particle1->currentPosition, WHITE, constraint.particle2->currentPosition, WHITE, Debug::Drawing::DRAW_SKINNY_IF_OCCLUDED );
	}
}

//-----------------------------------------------------------------------------------------------
void Cloth::Update( float )
{

}