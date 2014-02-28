#ifndef INCLUDED_CLOTH_HPP
#define INCLUDED_CLOTH_HPP
#pragma once

//-----------------------------------------------------------------------------------------------
#include <vector>
#include "../Engine/Math/FloatVector3.hpp"


//-----------------------------------------------------------------------------------------------
class Cloth
{
	#pragma region Composed Class Definitions 

	struct Particle
	{
		FloatVector3 currentPosition;
		FloatVector3 previousPosition;
		FloatVector3 velocity;
		float mass;
	};

	struct Constraint
	{
		Particle* particle1;
		Particle* particle2;
		float relaxedLength;
		float stiffnessCoefficient;
	};
	#pragma endregion

public:
	Cloth( unsigned int numberOfParticles )
	{
		GenerateParticleGrid( numberOfParticles );
	}

	void Render() const;
	void Update( float deltaSeconds );

private:
	std::vector< Particle > m_particles;
	std::vector< Constraint > m_constraints;
	float m_dragCoefficient;

	void GenerateParticleGrid( unsigned int numberOfParticles );

};

#endif //INCLUDED_CLOTH_HPP
