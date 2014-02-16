#ifndef INCLUDED_COLOR_HPP
#define INCLUDED_COLOR_HPP
#pragma once

//-----------------------------------------------------------------------------------------------
#include "EngineDefines.hpp"

//-----------------------------------------------------------------------------------------------
struct Color
{
	float r, g, b, a;

	Color()
		: r( 0.f )
		, g( 0.f )
		, b( 0.f )
		, a( 0.f )
	{ }

	Color( float red, float green, float blue, float alpha )
		: r( red )
		, g( green )
		, b( blue )
		, a( alpha )
	{ }
};

#endif //INCLUDED_COLOR_HPP
