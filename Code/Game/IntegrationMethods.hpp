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

#endif