#ifndef INCLUDED_FLOAT_4X4_MATRIX_HPP
#define INCLUDED_FLOAT_4X4_MATRIX_HPP
#pragma once

//-----------------------------------------------------------------------------------------------
#include "Matrix.hpp"

//-----------------------------------------------------------------------------------------------
typedef Matrix< float, 4, 4 > Float4x4Matrix;

//-----------------------------------------------------------------------------------------------
static const float F4X4_IDENTITY_ARRAY[ 16 ] = { 1.f, 0.f, 0.f, 0.f, 
												0.f, 1.f, 0.f, 0.f, 
												0.f, 0.f, 1.f, 0.f,
												0.f, 0.f, 0.f, 1.f };

static const Float4x4Matrix F4X4_IDENTITY_MATRIX = Float4x4Matrix( F4X4_IDENTITY_ARRAY );

#endif //INCLUDED_FLOAT_4X4_MATRIX_HPP
