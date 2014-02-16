#include <math.h>
#include "AABB.hpp"

//-----------------------------------------------------------------------------------------------
AABB::AABB()
	: min( 0.f, 0.f )
	, max( 0.f, 0.f )
{ }

//-----------------------------------------------------------------------------------------------
AABB::AABB( float minX, float minY, float maxX, float maxY )
	: min( minX, minY )
	, max( maxX, maxY )
{ }

//-----------------------------------------------------------------------------------------------
AABB::AABB( const FloatVector2& minXY, const FloatVector2& maxXY )
	: min( minXY )
	, max( maxXY )
{ }

//-----------------------------------------------------------------------------------------------
void AABB::CalculateCollisionResponseWith( const AABB& other, FloatVector2& out_restitution)
{
	float leftPenetration  = other.min.x - this->max.x;
	float rightPenetration = other.max.x - this->min.x;
	float upperPenetration = other.min.y - this->max.y;
	float lowerPenetration = other.max.y - this->min.y;

	if( abs( leftPenetration ) < rightPenetration )
		out_restitution.x = leftPenetration;
	else
		out_restitution.x = rightPenetration;

	if( abs( upperPenetration ) < lowerPenetration )
		out_restitution.y = upperPenetration;
	else
		out_restitution.y = lowerPenetration;

	if( abs( out_restitution.x ) < abs( out_restitution.y ) )
		out_restitution.y = 0.f;
	else
		out_restitution.x = 0.f;
}

//-----------------------------------------------------------------------------------------------
//This shorter collision detector for AABBs from: http://www.opentk.com/node/869
bool AABB::IsCollidingWith( const AABB& other ) const
{
	if( this->max.x < other.min.x ||
		this->max.y < other.min.y ||
		this->min.x > other.max.x ||
		this->min.y > other.max.y )
	{
		return false;
	}
	return true;
}
