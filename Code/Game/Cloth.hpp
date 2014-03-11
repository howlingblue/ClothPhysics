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
	static const size_t DEFAULT_NUMBER_OF_CONSTRAINT_SATISFACTION_LOOPS = 8;

public:
	#pragma region Composed Class Definitions 
	struct Particle
	{
		bool			positionIsLocked;
		FloatVector3	currentPosition;
		FloatVector3	previousPosition;
		FloatVector3	currentVelocity;
		FloatVector3	acceleration;
		FloatVector3	normal;
		float			mass;
		float			inverseMass;

		Particle( FloatVector3 startingPosition, bool lockPosition, float particleMass )
			: currentPosition( startingPosition )
			, previousPosition( startingPosition )
			, positionIsLocked( lockPosition )
			, mass( particleMass )
			, inverseMass( 1.0f/particleMass ) // PR: Potentially dangerous to do this here if mass is invalidly entered (EX: 0.0f )
		{ }

		inline void UpdatePositionIfUnlocked( const FloatVector3& movementDirection )
		{
			if( !positionIsLocked ) {
				currentPosition += movementDirection;
			}
		}

		inline void addExternalForceToParticle( const FloatVector3& forceVector ) {
			acceleration += ( inverseMass * forceVector ); 
		}

	};

	struct Constraint
	{
		Particle* particle1;
		Particle* particle2;
		float relaxedLength;

		Constraint( Particle& particleA, Particle& particleB )
		{
			particle1 = &particleA;
			particle2 = &particleB;
			FloatVector3 vectorBetweenConstraintEnds = particle1->currentPosition - particle2->currentPosition;
			relaxedLength = vectorBetweenConstraintEnds.CalculateNorm();
		}
	};
	#pragma endregion

public:
	Cloth( unsigned int particlesPerX, unsigned int particlesPerY, float dragCoefficient )
		: m_dragCoefficient( dragCoefficient )
		, m_particlesPerX( particlesPerX )
		, m_particlesPerY( particlesPerY )
		, m_numberOfConstraintSatisfactionLoops( DEFAULT_NUMBER_OF_CONSTRAINT_SATISFACTION_LOOPS )
	{
		GenerateParticleGrid( particlesPerX, particlesPerY );
	}

	void Render( bool drawInDebug ) const;
	void Update( float deltaSeconds, bool useConstraintSatisfaction );

	// Inline Mutators
	void setDragCoefficient( float dragCoefficient ); 
	float getDragCoefficient() const;
	void SetWindForce( const FloatVector3& windForce ) { m_windForce = windForce; }

private:

	std::vector< Particle* > m_particles;
	std::vector< Constraint > m_bendingConstraints;
	std::vector< Constraint > m_shearConstraints;
	std::vector< Constraint > m_structuralConstraints;
	float m_dragCoefficient;
	unsigned int m_particlesPerX, m_particlesPerY;
	FloatVector3 m_windForce;
	// PR: Added this to dictate how many times we for loop
	size_t		 m_numberOfConstraintSatisfactionLoops;

	Particle &   GetParticleAtPosition( size_t rowNum, size_t colNum );
	unsigned int GetIndexOfParticleEastOf( unsigned int particleIndex ) { return particleIndex + 1; }
	unsigned int GetIndexOfParticleSouthOf( unsigned int particleIndex ) { return particleIndex + m_particlesPerX; }
	unsigned int GetIndexOfParticleSoutheastOf( unsigned int particleIndex ) { return particleIndex + m_particlesPerX + 1; }

	bool indexIsNotOnRightEdge( unsigned int particleIndex ) { return particleIndex % m_particlesPerX < m_particlesPerX - 1; }
	bool indexIsNotOnBottomEdge( unsigned int particleIndex ) { return particleIndex < m_particles.size() - m_particlesPerX; }

	void ClearParticleAccelerations();
	void ClearParticleNormals();

	void ApplyForceToParticlesFromConstraint( Constraint& constraint, float stiffnessCoefficient );
	void CalculateAndAddNormalsToParticles( Particle* particle1, Particle* particle2, Particle* particle3 );
	void GenerateClothNormals();
	void GenerateParticleGrid( unsigned int particlesPerX, unsigned int particlesPerY );
	void GenerateVertexAndIndexArray( VertexColorNormalTextureData* out_nullVertexArray, unsigned int& out_numberOfVertices,
									  unsigned short* out_nullIndexArray, unsigned int& out_numberOfIndices );
	void SatisfyConstraint( Constraint& constraint );

	void AddWindForce( const FloatVector3& directionOfWindForce );
	void AddWindForcesForTriangle( Particle& p1, Particle& p2, Particle& p3, const FloatVector3& direction );

	void RenderDebugParticlesAndConstraints() const;
};

//-----------------------------------------------------------------------------------------------
inline void Cloth::ApplyForceToParticlesFromConstraint( Constraint& constraint, float stiffnessCoefficient )
{
	
	Particle* particle1 = constraint.particle1;
	Particle* particle2 = constraint.particle2;

	FloatVector3 vectorFromParticle1To2 = particle2->currentPosition - particle1->currentPosition;
	// PR: CalculateNorm is Length of vector?
	float currentDistanceBetweenParticles = vectorFromParticle1To2.CalculateNorm();

	FloatVector3 correctionVector = vectorFromParticle1To2 * ( 1.f - constraint.relaxedLength / currentDistanceBetweenParticles );
	FloatVector3 correctionVectorHalf = 0.5f * correctionVector;
	
	//PR: OLD method with springs
	FloatVector3 springForceVector = -stiffnessCoefficient * ( currentDistanceBetweenParticles - constraint.relaxedLength ) * ( vectorFromParticle1To2 / currentDistanceBetweenParticles );

	if( !particle1->positionIsLocked ) {
		particle1->acceleration -= springForceVector / particle1->mass;
	}

	if( !particle2->positionIsLocked ) {
		particle2->acceleration += springForceVector / particle2->mass;
	}
	
	
}


//-----------------------------------------------------------------------------------------------
inline void Cloth::CalculateAndAddNormalsToParticles( Particle* particle1, Particle* particle2, Particle* particle3 )
{
	FloatVector3 vectorFromParticle1To2 = particle2->currentPosition - particle1->currentPosition;
	FloatVector3 vectorFromParticle1To3 = particle3->currentPosition - particle1->currentPosition;

	FloatVector3 normalForTriangle = CrossProduct( vectorFromParticle1To2, vectorFromParticle1To3 );

	particle1->normal += normalForTriangle;
	particle2->normal += normalForTriangle;
	particle3->normal += normalForTriangle;
}

//-----------------------------------------------------------------------------------------------
inline void Cloth::SatisfyConstraint( Constraint& constraint )
{
	Particle* particle1 = constraint.particle1;
	Particle* particle2 = constraint.particle2;

	FloatVector3 vectorFromParticle1To2 = particle2->currentPosition - particle1->currentPosition;
	// PR: CalculateNorm is Length of vector?
	float currentDistanceBetweenParticles = vectorFromParticle1To2.CalculateNorm();

	FloatVector3 correctionVector = vectorFromParticle1To2 * ( 1.f - constraint.relaxedLength / currentDistanceBetweenParticles );
	FloatVector3 correctionVectorHalf = 0.5f * correctionVector;
	
	if ( !particle1->positionIsLocked ) {
		particle1->currentPosition += correctionVectorHalf;
	}

	if ( !particle2->positionIsLocked ) {
		FloatVector3 negatedHalfCorrectionVector = correctionVectorHalf * -1.0f;
		particle2->currentPosition += negatedHalfCorrectionVector;
	}
}


// PR : For Row major convenience
inline Cloth::Particle& Cloth::GetParticleAtPosition( size_t colNum, size_t rowNum ) 
{
	size_t offset = ( rowNum * m_particlesPerX ) + colNum; 
	Particle & particle = *(m_particles[offset]);
	return particle;
}


#endif //INCLUDED_CLOTH_HPP
