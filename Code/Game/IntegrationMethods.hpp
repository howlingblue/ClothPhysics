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
	FloatVector3 midpointVelocity;
	float dampeningCoefficient = cloth.getDragCoefficient();
	// Calculate Midpoint Velocity
	midpointVelocity.x = particleToIntegrate.currentVelocity.x + 
		( deltaSeconds * 0.50f ) * 
		( ( ( -dampeningCoefficient * particleToIntegrate.previousVelocity.x ) ) /
		particleToIntegrate.mass );

	midpointVelocity.x = particleToIntegrate.currentVelocity.y + 
		( deltaSeconds * 0.50f ) * 
		( ( ( -dampeningCoefficient * particleToIntegrate.previousVelocity.y ) ) /
		particleToIntegrate.mass );

	midpointVelocity.z = particleToIntegrate.currentVelocity.z + 
		( deltaSeconds * 0.50f ) * 
		( ( ( -dampeningCoefficient * particleToIntegrate.previousVelocity.z ) ) /
		particleToIntegrate.mass );

	// Update Position Based On Midpoint Velocity
	particleToIntegrate.currentPosition.x = particleToIntegrate.previousPosition.x + ( deltaSeconds * midpointVelocity.x );
	particleToIntegrate.currentPosition.y = particleToIntegrate.previousPosition.y + ( deltaSeconds * midpointVelocity.y );
	particleToIntegrate.currentPosition.z = particleToIntegrate.previousPosition.z + ( deltaSeconds * midpointVelocity.z );

	// Update Current Velocity
	particleToIntegrate.currentVelocity.x = midpointVelocity.x + 
		( deltaSeconds * 0.50f ) * 
		( ( ( -dampeningCoefficient * midpointVelocity.x ) ) / 
		particleToIntegrate.mass );

	particleToIntegrate.currentVelocity.y = midpointVelocity.y + 
		( deltaSeconds * 0.50f ) * 
		( ( ( -dampeningCoefficient * midpointVelocity.y ) ) / 
		particleToIntegrate.mass );

	particleToIntegrate.currentVelocity.z = midpointVelocity.z + 
		( deltaSeconds * 0.50f ) * 
		( ( ( -dampeningCoefficient * midpointVelocity.z ) ) / 
		particleToIntegrate.mass );

	// Swap Current With Previous
	std::swap( particleToIntegrate.currentPosition, particleToIntegrate.previousPosition );
	std::swap( particleToIntegrate.currentVelocity, particleToIntegrate.previousVelocity );

}

#endif