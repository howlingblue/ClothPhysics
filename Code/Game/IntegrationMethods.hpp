#ifndef included_IntegrationMethods
#define included_IntegrationMethods
#pragma once

#include <algorithm>

#include "Cloth.hpp"
#include "../Engine/Math/FloatVector3.hpp"

// Inline Integrator Function Dec
void verletLeapFrogIntegrationMassSpringDamper( const Cloth & cloth, 
	Cloth::Particle & particleToIntegrate, 
	float deltaSeconds );

// PR: TODO:: Move this to generic math util class
FloatVector3 calculateTriangleNormal( const Cloth::Particle& p1, const Cloth::Particle& p2, const Cloth::Particle& p3 );

// ---------------------- IMPLEMENTATIONS -------------------- //
inline void verletLeapFrogIntegrationMassSpringDamper( const Cloth & cloth, 
	Cloth::Particle & particleToIntegrate,
	float deltaSeconds )
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

/*
Vec3 calcTriangleNormal(Particle *p1,Particle *p2,Particle *p3)
{
Vec3 pos1 = p1->getPos();
Vec3 pos2 = p2->getPos();
Vec3 pos3 = p3->getPos();

Vec3 v1 = pos2-pos1;
Vec3 v2 = pos3-pos1;

return v1.cross(v2);
}

*/


#endif