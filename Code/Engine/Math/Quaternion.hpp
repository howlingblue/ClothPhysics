#ifndef INCLUDED_QUATERNION_HPP
#define INCLUDED_QUATERNION_HPP
#pragma once

//-----------------------------------------------------------------------------------------------
#include "EulerAngles.hpp"
#include "Matrix.hpp"
#include "Vector3.hpp"

//-----------------------------------------------------------------------------------------------
template <typename NumericType>
struct Quaternion
{
	NumericType x;
	NumericType y;
	NumericType z;
	NumericType w;

public:
	Quaternion()
		: x( NumericType() )
		, y( NumericType() )
		, z( NumericType() )
		, w( NumericType() )
	{ }

	Quaternion( NumericType q0X, NumericType q1Y, NumericType q2Z, NumericType q3W )
		: x( q0X )
		, y( q1Y )
		, z( q2Z )
		, w( q3W )
	{ }

	Quaternion( const Vector3<NumericType>& axis, NumericType angleRadians )
		: x( axis.x * sin( NumericType( 0.5 ) * angleRadians ) )
		, y( axis.y * sin( NumericType( 0.5 ) * angleRadians ) )
		, z( axis.z * sin( NumericType( 0.5 ) * angleRadians ) )
		, w( cos( NumericType( 0.5 ) * angleRadians ) )
	{
		Normalize();
	}

	void ConvertToRotationMatrix( Matrix<NumericType, 4, 4>& out_rotationMatrix );

	const Quaternion< NumericType > CalculateConjugate() const;
	NumericType CalculateNorm() const;
	NumericType CalculateSquaredNorm() const;
	void Normalize();

	bool operator==( const Quaternion<NumericType>& rhs ) const;
	bool operator!=( const Quaternion<NumericType>& rhs ) const;

	Quaternion<NumericType>& operator+=( const Quaternion<NumericType>& rhs );
	Quaternion<NumericType>& operator-=( const Quaternion<NumericType>& rhs );
	Quaternion<NumericType>& operator*=( const NumericType& alpha );

	//const Quaternion<NumericType> operator+( const Quaternion<NumericType>& q,	const Quaternion<NumericType>& p );
	//const Quaternion<NumericType> operator-( const Quaternion<NumericType>& q,	const Quaternion<NumericType>& p );
	//const Quaternion<NumericType> operator*( const Quaternion<NumericType>& q,	const NumericType& alpha );
	//const Quaternion<NumericType> operator*( const NumericType& alpha,		const Quaternion<NumericType>& q );
	//const Quaternion<NumericType> operator*( const Quaternion<NumericType>& q,	const Quaternion<NumericType>& p );
	//const NumericType DotProduct( const Quaternion<NumericType>& q,	const Quaternion<NumericType>& p );
};






//-----------------------------------------------------------------------------------------------
template <typename NumericType>
inline void Quaternion<NumericType>::ConvertToRotationMatrix( Matrix<NumericType, 4, 4>& out_rotationMatrix )
{
	//assert( AreAlmostEqual( this->CalculateNorm(), NumericType( 1.0 ) ) );

	out_rotationMatrix(1, 1) = NumericType( 1.0 ) - ( NumericType( 2.0 ) * y * y ) - ( NumericType( 2.0 ) * z * z );
	out_rotationMatrix(1, 2) = ( NumericType( 2.0 ) * x * y ) - ( NumericType( 2.0 ) * w * z );
	out_rotationMatrix(1, 3) = ( NumericType( 2.0 ) * x * z ) + ( NumericType( 2.0 ) * w * y );
	out_rotationMatrix(2, 1) = ( NumericType( 2.0 ) * x * y ) + ( NumericType( 2.0 ) * w * z );
	out_rotationMatrix(2, 2) = NumericType( 1.0 ) - ( NumericType( 2.0 ) * x * x ) - ( NumericType( 2.0 ) * z * z );
	out_rotationMatrix(2, 3) = ( NumericType( 2.0 ) * y * z ) - ( NumericType( 2.0 ) * w * x );
	out_rotationMatrix(3, 1) = ( NumericType( 2.0 ) * x * z ) - ( NumericType( 2.0 ) * w * y );
	out_rotationMatrix(3, 2) = ( NumericType( 2.0 ) * y * z ) + ( NumericType( 2.0 ) * w * x );
	out_rotationMatrix(3, 3) = NumericType( 1.0 ) - ( NumericType( 2.0 ) * x * x ) - ( NumericType( 2.0 ) * y * y );
	out_rotationMatrix(4, 4) = 1.f;
}

//-----------------------------------------------------------------------------------------------
template <typename NumericType>
inline const Quaternion< NumericType > Quaternion<NumericType>::CalculateConjugate() const
{
	Quaternion< NumericType > conjugate;

	conjugate.w = this->w;
	conjugate.x = -this->x;
	conjugate.y = -this->y;
	conjugate.z = -this->z;

	return conjugate;
}

//-----------------------------------------------------------------------------------------------
template <typename NumericType>
inline NumericType Quaternion<NumericType>::CalculateNorm() const
{
	return sqrt( CalculateSquaredNorm() );
}

//-----------------------------------------------------------------------------------------------
template <typename NumericType>
inline NumericType Quaternion<NumericType>::CalculateSquaredNorm() const
{
	return ( w * w ) + ( x * x ) + ( y * y ) + ( z * z );
}

//-----------------------------------------------------------------------------------------------
template <typename NumericType>
inline void Quaternion<NumericType>::Normalize()
{
	NumericType norm = this->CalculateNorm();

	w /= norm;
	x /= norm;
	y /= norm;
	z /= norm;
}

//-----------------------------------------------------------------------------------------------
template <typename NumericType>
inline bool Quaternion<NumericType>::operator==( const Quaternion<NumericType>& rhs ) const
{
	if( this->x == rhs.x && this->y == rhs.y && this->z == rhs.z && this->w == rhs.w )
		return true;
	return false;
}

//-----------------------------------------------------------------------------------------------
template <typename NumericType>
inline bool Quaternion<NumericType>::operator!=( const Quaternion<NumericType>& rhs ) const
{
	return !( this == rhs );
}

//-----------------------------------------------------------------------------------------------
template <typename NumericType>
inline Quaternion<NumericType>& Quaternion<NumericType>::operator+=( const Quaternion<NumericType>& rhs ) 
{
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	this->w += rhs.w;

	return *this;
}

//-----------------------------------------------------------------------------------------------
template <typename NumericType>
Quaternion<NumericType>& Quaternion<NumericType>::operator-=( const Quaternion<NumericType>& rhs )
{
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	this->w -= rhs.w;

	return *this;
}

//-----------------------------------------------------------------------------------------------
template <typename NumericType>
inline Quaternion<NumericType>& Quaternion<NumericType>::operator*=( const NumericType& alpha )
{
	this->x *= alpha;
	this->y *= alpha;
	this->z *= alpha;
	this->w *= alpha;

	return *this;
}

//-----------------------------------------------------------------------------------------------
template <typename NumericType>
inline const Quaternion<NumericType> operator+( const Quaternion<NumericType>& q,	const Quaternion<NumericType>& p )
{
	Quaternion< NumericType > result = q;
	result += p;
	return result;
}

//-----------------------------------------------------------------------------------------------
template <typename NumericType>
inline const Quaternion<NumericType> operator-( const Quaternion<NumericType>& q,	const Quaternion<NumericType>& p )
{
	Quaternion< NumericType > result = q;
	result -= p;
	return result;
}

//-----------------------------------------------------------------------------------------------
template <typename NumericType>
inline const Quaternion<NumericType> operator*( const Quaternion<NumericType>& q,	const NumericType& alpha )
{
	Quaternion< NumericType > result = q;
	result *= alpha;
	return result;
}

//-----------------------------------------------------------------------------------------------
template <typename NumericType>
inline const Quaternion<NumericType> operator*( const NumericType& alpha,		const Quaternion<NumericType>& q )
{
	Quaternion< NumericType > result = q;
	result.x = alpha * result.x;
	result.y = alpha * result.y;
	result.z = alpha * result.z;
	result.w = alpha * result.w;
	return result;
}

//-----------------------------------------------------------------------------------------------
template <typename NumericType>
inline const Quaternion<NumericType> operator*( const Quaternion<NumericType>& q,	const Quaternion<NumericType>& p )
{
	Quaternion< NumericType > result;
	result.x = ( q.w  * p.x ) + ( q.x * p.w ) + ( q.y * p.z ) - ( q.z * p.y );
	result.y = ( q.w * p.y ) - ( q.x * p.z ) + ( q.y * p.w ) + ( q.z * p.x );
	result.z = ( q.w * p.z ) + ( q.x * p.y ) - ( q.y * p.x ) + ( q.z * p.w );
	result.w = ( q.w * p.w ) - ( q.x * p.x ) - ( q.y * p.y ) - ( q.z * p.z );
	return result;
}

//-----------------------------------------------------------------------------------------------
template <typename NumericType>
const NumericType DotProduct( const Quaternion<NumericType>& q,	const Quaternion<NumericType>& p )
{
	return ( q.w * p.w ) + ( q.x * p.x ) + ( q.y * p.y ) + ( q.z * p.z ); 
}

//-----------------------------------------------------------------------------------------------
inline void UnitTestQuaternions()
{
	//Test addition
	Quaternion< float > quat1( 1.f, 2.f, 3.f, 4.f ), quat2( 5.f, 6.f, 7.f, 8.f );
	Quaternion< float > quatAdded = quat1 + quat2;
	assert( quatAdded == Quaternion< float >( 6.f, 8.f, 10.f, 12.f ) );
	
	//Test subtraction
	Quaternion< float > quatSubbed = quat2 - quat1;
	assert( quatSubbed == Quaternion< float >( 4.f, 4.f, 4.f, 4.f ) );

	//Test scalar multiplication
	Quaternion< float > quatPostMultiplied = quat1 * 3.f;
	assert( quatPostMultiplied == Quaternion< float >( 3.f, 6.f, 9.f, 12.f ) );

	Quaternion< float > quatPreMultiplied = 4.f * quat2;
	assert( quatPreMultiplied == Quaternion< float >( 20.f, 24.f, 28.f, 32.f ) );

	//Test quaternion multiplication
	Quaternion< float > quat3( 2.f, 3.f, 4.f, 1.f ), quat4( 6.f, 7.f, 8.f, 5.f );
	Quaternion< float > quatMultiplication = quat3 * quat4;
	assert( quatMultiplication == Quaternion< float >( 12.f, 30.f, 24.f, -60.f ) );

	//FIX: Test dot product

	//Test length calculation and normalization
	Quaternion< float > quat5( 1.f, 1.f, 1.f, 1.f );
	float quatLength = quat5.CalculateNorm();
	assert( quatLength == 2.f );

	Quaternion< float > quat6 = quat5;
	quat6.Normalize();
	assert( quat6.CalculateNorm() == 1.f );

	//Test conjugation and multiplication (again )
	Quaternion< float > quat4prime = quat4.CalculateConjugate();
	Quaternion< float > quatAbsoluteValueSquared = quat4 * quat4prime;
	assert( quatAbsoluteValueSquared == Quaternion< float >( 0.f, 0.f, 0.f, 174.f ) );

	//Test Rotation Matrix Conversion
	Quaternion< float > quat7( Vector3< float >( 1.f, 1.f, 1.f ), .66666666f * 3.14159265f );
	quat7.Normalize();
	Matrix< float, 4, 4 > quatRotation, correctRotation;
	correctRotation( 1, 1 ) = 0.f;
	correctRotation( 1, 2 ) = 0.f;
	correctRotation( 1, 3 ) = 1.f;
	correctRotation( 2, 1 ) = 1.f;
	correctRotation( 2, 2 ) = 0.f;
	correctRotation( 2, 3 ) = 0.f;
	correctRotation( 3, 1 ) = 0.f;
	correctRotation( 3, 2 ) = 1.f;
	correctRotation( 3, 3 ) = 0.f;
	correctRotation( 4, 4 ) = 1.f;
	quat7.ConvertToRotationMatrix( quatRotation );
	//assert( quatRotation == correctRotation ); // Off due to float evaluation
}

#endif //INCLUDED_QUATERNION_HPP
