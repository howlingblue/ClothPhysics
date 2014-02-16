#ifndef INCLUDED_AABB_3D_HPP
#define INCLUDED_AABB_3D_HPP
#pragma once

//-----------------------------------------------------------------------------------------------
#include "Math/FloatVector3.hpp"

//-----------------------------------------------------------------------------------------------
struct AABB3D
{
	FloatVector3 boxMin;
	FloatVector3 boxMax;

	AABB3D();
	AABB3D( float minX, float minY, float minZ, float maxX, float maxY, float maxZ );
	AABB3D( const FloatVector3& minXYZ, const FloatVector3& maxXYZ );
	AABB3D( const FloatVector3& objectCenter, float boxXWidth, float boxYLength, float boxZHeight );

// 	void CalculateCollisionResponseWith( const AABB3D& other, FloatVector3& out_restitution);
 	bool IsCollidingWith( const AABB3D& other ) const;
	void Render() const;
};

//-----------------------------------------------------------------------------------------------
inline AABB3D::AABB3D()
{ }

//-----------------------------------------------------------------------------------------------
inline AABB3D::AABB3D( float minX, float minY, float minZ, float maxX, float maxY, float maxZ )
	: boxMin( minX, minY, minZ )
	, boxMax( maxX, maxY, maxZ )
{ }

//-----------------------------------------------------------------------------------------------
inline AABB3D::AABB3D( const FloatVector3& minXYZ, const FloatVector3& maxXYZ )
	: boxMin( minXYZ )
	, boxMax( maxXYZ )
{ }

//-----------------------------------------------------------------------------------------------
inline AABB3D::AABB3D( const FloatVector3& objectCenter, float boxXWidth, float boxYLength, float boxZHeight )
{
	float halfWidth = boxXWidth * 0.5f;
	float halfLength = boxYLength * 0.5f;
	float halfHeight = boxZHeight * 0.5f;

	boxMin.x = objectCenter.x - halfWidth;
	boxMax.x = objectCenter.x + halfWidth;
	boxMin.y = objectCenter.y - halfLength;
	boxMax.y = objectCenter.y + halfLength;
	boxMin.z = objectCenter.z - halfHeight;
	boxMax.z = objectCenter.z + halfHeight;
}

#endif //INCLUDED_AABB_3D_HPP
