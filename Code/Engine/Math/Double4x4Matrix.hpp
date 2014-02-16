#ifndef INCLUDED_DOUBLE_4X4_MATRIX_HPP
#define INCLUDED_DOUBLE_4X4_MATRIX_HPP
#pragma once

//-----------------------------------------------------------------------------------------------
#include "Matrix.hpp"

//-----------------------------------------------------------------------------------------------
typedef Matrix< double, 4, 4 > Double4x4Matrix;

//-----------------------------------------------------------------------------------------------
static const double D4X4_IDENTITY_ARRAY[ 16 ] = { 1.0, 0.0, 0.0, 0.0, 
												 0.0, 1.0, 0.0, 0.0, 
												 0.0, 0.0, 1.0, 0.0,
												 0.0, 0.0, 0.0, 1.0 };

static const Double4x4Matrix D4X4_IDENTITY_MATRIX = Double4x4Matrix( D4X4_IDENTITY_ARRAY );

#endif //INCLUDED_DOUBLE_4X4_MATRIX_HPP
