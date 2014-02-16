#ifndef INCLUDED_INTERPOLATION_HPP
#define INCLUDED_INTERPOLATION_HPP
#pragma once

//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
inline float InverseSquareInterpolate( float value )
{
	//1 - (1 - t)^2
	return 1 - ( ( 1 - value ) * ( 1 - value ) );
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

#endif //INCLUDED_INTERPOLATION_HPP
