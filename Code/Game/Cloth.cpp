#include "../Engine/DebugDrawing.hpp"
#include "Cloth.hpp"

//-----------------------------------------------------------------------------------------------s
void Cloth::GenerateParticleGrid( unsigned int numberOfParticles )
{
	FloatVector3 TOP_LEFT_CORNER( -25.f, 25.f, 0.f );
	for( unsigned int i = 0; i < 50; ++i )
	{
		for( unsigned int j = 0; j < 50; ++j )
		{
			//m_particles[ i + j ] = ;
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Cloth::Render() const
{
	for( unsigned int i = 0; i < m_particles.size(); ++i )
	{
		const Particle& particle = *m_particles[ i ];

		Debug::DrawPoint( particle.currentPosition, 0.5f, Color( 1.f, 1.f, 1.f, 1.f ), Debug::Drawing::DRAW_SKINNY_IF_OCCLUDED );
	}
}

//-----------------------------------------------------------------------------------------------
void Cloth::Update( float )
{

}