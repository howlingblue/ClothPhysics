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
	//Since this cloth is made of one material, all particles can use the same constraint
	struct Constraint
	{
		float relaxedLength;
		float dragCoefficient;
		float stiffnessCoefficient;
	};

	struct Particle
	{
		std::vector< Particle* > boundParticles;
		FloatVector3 position;
		FloatVector3 velocity;
		float mass;
	};
	#pragma endregion

public:
	Cloth() { }

	void Render() const;
	void Update( float deltaSeconds );

private:
	float m_length;
	float m_width;
	unsigned int numberOfParticles;

};

#endif //INCLUDED_CLOTH_HPP
