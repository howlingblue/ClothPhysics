#ifndef INCLUDED_AABB_HPP
#define INCLUDED_AABB_HPP
#pragma once

//-----------------------------------------------------------------------------------------------
#include "Math/FloatVector2.hpp"

//-----------------------------------------------------------------------------------------------
struct AABB
{
	FloatVector2 min, max;

	AABB();
	AABB( float minX, float minY, float maxX, float maxY );
	AABB( const FloatVector2& minXY, const FloatVector2& maxXY );

	void CalculateCollisionResponseWith( const AABB& other, FloatVector2& out_restitution);
	bool IsCollidingWith( const AABB& other ) const;
};

#endif //INCLUDED_AABB_HPP
