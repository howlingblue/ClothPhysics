#ifndef INCLUDED_PERLIN_NOISE_HPP
#define INCLUDED_PERLIN_NOISE_HPP
#pragma once

//-----------------------------------------------------------------------------------------------
#include "../Engine/Math/EngineMath.hpp"
#include "../Engine/Math/FloatVector2.hpp"

//-----------------------------------------------------------------------------------------------
inline double GenerateNoiseFromSeeds( int x, int y )
{
	int seed = x + y * 57;
	seed = (seed << 13 ) ^ seed;
	return ( 1.0 - ( ( seed * ( seed * seed * 15731 + 789221 ) + 1376312589 ) & 0x7fffffff ) / 1073741824.0 );
}

//-----------------------------------------------------------------------------------------------
inline float GeneratePseudoRandomAngleInRadians( int x, int y )
{
	float pseudoRandomNormalizedNumber = static_cast< float >( GenerateNoiseFromSeeds( x, y ) );

	//convert the pseudoRandomNumber so that it is between 0 and 360
	return ( pseudoRandomNormalizedNumber * PI ) + PI;
}

//-----------------------------------------------------------------------------------------------
inline FloatVector2 GeneratePseudoRandomUnitVectorAtPosition( int x, int y )
{
	float pseudoRandomAngle = GeneratePseudoRandomAngleInRadians( x , y );
	return FloatVector2( cos( pseudoRandomAngle ), sin( pseudoRandomAngle ) );
}

//-----------------------------------------------------------------------------------------------
inline float SmoothStep( float value )
{
	//3t^2 - 2t^3
	return ( 3 * value * value ) - ( 2 * value * value * value );
}

//-----------------------------------------------------------------------------------------------
inline float SmootherStep( float value )
{
	//6t^5 - 15t^4 + 10t^3
	float valueSquared = value * value;
	float valueCubed = value * value * value;
	return ( 6 * valueSquared * valueCubed ) - ( 15 * valueSquared * valueSquared ) + ( 10 * valueCubed );
}

//-----------------------------------------------------------------------------------------------
float Generate2DNoise( float x, float y, float gridSize );

//-----------------------------------------------------------------------------------------------
float PerlinNoise2D( float x, float y, float startingBlockSize, float persistence, unsigned int octaves );

#endif //INCLUDED_PERLIN_NOISE_HPP
