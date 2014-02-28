#ifndef INCLUDED_CLOTH_HPP
#define INCLUDED_CLOTH_HPP
#pragma once

//-----------------------------------------------------------------------------------------------
#include <cassert>
#include <vector>
#include "../Engine/Math/FloatVector3.hpp"


//-----------------------------------------------------------------------------------------------
class Cloth
{
public:
	#pragma region Composed Class Definitions 
	struct Particle
	{
		FloatVector3	currentPosition;
		FloatVector3	previousPosition;
		FloatVector3	currentVelocity;
		FloatVector3	previousVelocity;
		FloatVector3	acceleration;
		float			mass;
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
	Cloth( unsigned int numberOfParticles, float dragCoefficient )
	{
		GenerateParticleGrid( numberOfParticles );
	}

	void Render() const;
	void Update( float deltaSeconds );

	// Inline Mutators
	void setDragCoefficient( float dragCoefficient ); 
	float getDragCoefficient() const;

private:
	std::vector< Particle* > m_particles;
	std::vector< Constraint* > m_constraints;
	float m_dragCoefficient;

	void GenerateParticleGrid( unsigned int numberOfParticles );

};

// Inline Mutators
inline void Cloth::setDragCoefficient( float dragCoefficient ) {
	assert( dragCoefficient >= 0.0f );
	m_dragCoefficient = dragCoefficient;
}


inline float Cloth::getDragCoefficient() const {
	return m_dragCoefficient;
}

#endif //INCLUDED_CLOTH_HPP
