#ifndef included_IntegrationMethods
#define included_IntegrationMethods
#pragma once

#include <algorithm>

#include "Cloth.hpp"
#include "../Engine/Math/FloatVector3.hpp"

// Inline Integrator Function Dec
void verletLeapFrogIntegrationMassSpringDamper( const Cloth & cloth, Cloth::Particle & particleToIntegrate, float deltaSeconds );

void verletIntegration( Cloth::Particle& p1, const Cloth::Particle& p2, float deltaSeconds );

// PR: TODO:: Move this to generic math util class
FloatVector3 calculateTriangleNormal( const Cloth::Particle& p1, const Cloth::Particle& p2, const Cloth::Particle& p3 );

// ---------------------- IMPLEMENTATIONS -------------------- //
inline FloatVector3 calculateTriangleNormal( const Cloth::Particle& p1, const Cloth::Particle& p2, const Cloth::Particle& p3 ) {

	// PR:: For some reason there are particles with the same positions
	FloatVector3 v1 = p2.currentPosition - p1.currentPosition;
	FloatVector3 v2 = p3.currentPosition - p1.currentPosition;

	FloatVector3 triangleNormal = CrossProduct( v1, v2 );
	if ( triangleNormal.x == 0.0f && triangleNormal.y == 0.0f && triangleNormal.z == 0.0f ) {
		triangleNormal.z = 1.0f;
		triangleNormal.y = 1.0f;
		triangleNormal.z = 1.0f;
	}
	return triangleNormal;

}



inline void verletIntegration( Cloth::Particle& p1, float deltaSeconds ) {

	FloatVector3 tempPos = p1.currentPosition;
	p1.currentPosition = p1.currentPosition + ( p1.currentPosition - p1.previousPosition ) + ( p1.acceleration * deltaSeconds );
	p1.previousPosition = tempPos;

}

inline void verletLeapFrogIntegrationMassSpringDamper( const Cloth & cloth, Cloth::Particle & particleToIntegrate, float deltaSeconds )
{
	float halfDeltaSeconds = deltaSeconds * 0.5f;
	// Calculate Midpoint Velocity
	FloatVector3 midpointVelocity = particleToIntegrate.currentVelocity + halfDeltaSeconds * particleToIntegrate.acceleration;
	
	// Update Position Based On Midpoint Velocity
	particleToIntegrate.currentPosition = particleToIntegrate.previousPosition + ( deltaSeconds * midpointVelocity );
	
	particleToIntegrate.currentVelocity = midpointVelocity + halfDeltaSeconds * particleToIntegrate.acceleration;

	// Swap Current With Previous
	particleToIntegrate.previousPosition = particleToIntegrate.currentPosition;
}

/*
Vec3 temp = pos;
pos = pos + (pos-old_pos)*(1.0-DAMPING) + acceleration*TIME_STEPSIZE2;
old_pos = temp;
acceleration = Vec3(0,0,0); // acceleration is reset since it HAS been translated into a change i
*/

#endif