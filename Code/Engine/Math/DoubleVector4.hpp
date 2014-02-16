#ifndef INCLUDED_DOUBLE_VECTOR_4_HPP
#define INCLUDED_DOUBLE_VECTOR_4_HPP
#pragma once

//-----------------------------------------------------------------------------------------------
#include "Vector4.hpp"

//-----------------------------------------------------------------------------------------------
typedef Vector4< double > DoubleVector4;

//-----------------------------------------------------------------------------------------------
inline double DoubleVector4::CalculateNorm() const
{
	return sqrt( CalculateSquaredNorm() );
}

//-----------------------------------------------------------------------------------------------
inline void DoubleVector4::Normalize()
{
	float norm = this->CalculateNorm();
	if( norm == 0.f )
		return;

	x /= norm;
	y /= norm;
	z /= norm;

	ONLY_DURING_MATH_DEBUGGING( assert( AreAlmostEqual( this->CalculateNorm(), 1.f ) ) );
}

#endif //INCLUDED_DOUBLE_VECTOR_4_HPP
