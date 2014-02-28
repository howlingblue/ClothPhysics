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
		bool			positionIsLocked;
		FloatVector3	currentPosition;
		FloatVector3	previousPosition;
		FloatVector3	currentVelocity;
		FloatVector3	previousVelocity;
		FloatVector3	acceleration;
		float			mass;

		Particle( FloatVector3 startingPosition, bool lockPosition, float particleMass )
			: currentPosition( startingPosition )
			, previousPosition( startingPosition )
			, positionIsLocked( lockPosition )
			, mass( particleMass )
		{ }

		void UpdatePositionIfUnlocked( const FloatVector3& movementDirection )
		{
			if( !positionIsLocked )
				currentPosition += movementDirection;
		}
	};

	struct Constraint
	{
		Particle* particle1;
		Particle* particle2;
		float relaxedLength;
		float stiffnessCoefficient;

		Constraint( Particle& particleA, Particle& particleB, float constraintStiffness )
		{
			particle1 = &particleA;
			particle2 = &particleB;
			FloatVector3 vectorBetweenConstraintEnds = particle1->currentPosition - particle2->currentPosition;
			relaxedLength = vectorBetweenConstraintEnds.CalculateNorm();
			stiffnessCoefficient = constraintStiffness;
		}
	};
	#pragma endregion

public:
	Cloth( unsigned int particlesPerX, unsigned int particlesPerY, float dragCoefficient )
		: m_dragCoefficient( dragCoefficient )
	{
		GenerateParticleGrid( particlesPerX, particlesPerY );
	}

	void Render() const;
	void Update( float deltaSeconds );

	// Inline Mutators
	void setDragCoefficient( float dragCoefficient ); 
	float getDragCoefficient() const;

private:
	std::vector< Particle* > m_particles;
	std::vector< Constraint > m_constraints;
	float m_dragCoefficient;

	void ApplyForceToParticlesFromConstraint( Constraint& constraint );
	void ClearParticleAccelerations();
	void GenerateParticleGrid( unsigned int particlesPerX, unsigned int particlesPerY );
	void SatisfyConstraint( Constraint& constraint );
};

//-----------------------------------------------------------------------------------------------
inline void Cloth::ApplyForceToParticlesFromConstraint( Constraint& constraint )
{
	Particle* particle1 = constraint.particle1;
	Particle* particle2 = constraint.particle2;

	FloatVector3 vectorFromParticle1To2 = particle2->currentPosition - particle1->currentPosition;
	float currentDistanceBetweenParticles = vectorFromParticle1To2.CalculateNorm();

	FloatVector3 correctionVector = vectorFromParticle1To2 * ( 1.f - constraint.relaxedLength / currentDistanceBetweenParticles );
	FloatVector3 correctionVectorHalf = 0.5f * correctionVector;
	FloatVector3 springForceVector = -STIFFNESS_COEFFICIENT * ( currentDistanceBetweenParticles - constraint.relaxedLength ) * ( vectorFromParticle1To2 / currentDistanceBetweenParticles );

	if( !particle1->positionIsLocked )
		particle1->acceleration -= springForceVector / particle1->mass;

	if( !particle2->positionIsLocked )
		particle2->acceleration += springForceVector / particle2->mass;
}

//-----------------------------------------------------------------------------------------------
inline void Cloth::SatisfyConstraint(  Constraint& constraint )
{
	FloatVector3 vectorFromParticle1To2 = constraint.particle2->currentPosition - constraint.particle1->currentPosition;
	float currentDistanceBetweenParticles = vectorFromParticle1To2.CalculateNorm();

	FloatVector3 correctionVector = vectorFromParticle1To2 * ( 1.f - constraint.relaxedLength / currentDistanceBetweenParticles );
	FloatVector3 correctionVectorHalf = 0.5f * correctionVector;

	if( constraint.particle2->positionIsLocked )
		constraint.particle1->UpdatePositionIfUnlocked( correctionVector );
	else
		constraint.particle1->UpdatePositionIfUnlocked( correctionVectorHalf );

	if( constraint.particle1->positionIsLocked )
		constraint.particle2->UpdatePositionIfUnlocked( -correctionVector );
	else
		constraint.particle2->UpdatePositionIfUnlocked( -correctionVectorHalf );
}

#endif //INCLUDED_CLOTH_HPP
