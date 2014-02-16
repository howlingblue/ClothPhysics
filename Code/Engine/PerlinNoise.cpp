#include "PerlinNoise.hpp"

//-----------------------------------------------------------------------------------------------
float Generate2DNoise( float x, float y, float gridSize )
{
	float INVERSE_GRID_SIZE = 1.0f / gridSize;

	//Find location in grid space
	float xWithRespectToGridSize = x * INVERSE_GRID_SIZE;
	float yWithRespectToGridSize = y * INVERSE_GRID_SIZE;
	int	  gridX = static_cast< int >( floor( xWithRespectToGridSize ) );
	int   gridY = static_cast< int >( floor( yWithRespectToGridSize ) );
	float xInGrid = xWithRespectToGridSize - gridX;
	float yInGrid = yWithRespectToGridSize - gridY;

	//Smooth values
	FloatVector2 gridPosition( SmoothStep( xInGrid ), SmoothStep( yInGrid ) );

	//Create Random Gradients
	FloatVector2 gradient00 = GeneratePseudoRandomUnitVectorAtPosition( gridX,		gridY );
	FloatVector2 gradientX0 = GeneratePseudoRandomUnitVectorAtPosition( gridX + 1,	gridY );
	FloatVector2 gradient0Y = GeneratePseudoRandomUnitVectorAtPosition( gridX,		gridY + 1 );
	FloatVector2 gradientXY = GeneratePseudoRandomUnitVectorAtPosition( gridX + 1,	gridY + 1 );

	FloatVector2 vectorToCorner00( gridPosition.x,		 gridPosition.y );
	FloatVector2 vectorToCornerX0( gridPosition.x - 1.f, gridPosition.y );
	FloatVector2 vectorToCorner0Y( gridPosition.x,		 gridPosition.y - 1.f );
	FloatVector2 vectorToCornerXY( gridPosition.x - 1.f, gridPosition.y - 1.f );

	//Dot Product
	float influence00 = DotProduct( gradient00, vectorToCorner00 );
	float influenceX0 = DotProduct( gradientX0, vectorToCornerX0 );
	float influence0Y = DotProduct( gradient0Y, vectorToCorner0Y );
	float influenceXY = DotProduct( gradientXY, vectorToCornerXY );

	//Weighted Average
	float influenceLowerSide = influence00 + gridPosition.x * ( influenceX0 - influence00 );
	float influenceUpperSide = influence0Y + gridPosition.x * ( influenceXY - influence0Y );
	float influenceCenter = influenceLowerSide + gridPosition.y * ( influenceUpperSide - influenceLowerSide );
	assert( influenceCenter <= 1.f);
	assert( influenceCenter >= -1.f);
	return influenceCenter;
}

//-----------------------------------------------------------------------------------------------
float PerlinNoise2D( float x, float y, float startingBlockSize, float persistence, unsigned int octaves )
{
	float total = 0.f;
	
	float amplitude = 1.f;
	for( unsigned int tone = 0; tone < octaves; ++tone )
	{
		float frequency = 1.f / ( 1 << tone );
		
		total += Generate2DNoise( x, y, startingBlockSize * frequency ) * amplitude;

		amplitude *= persistence;
	}
	assert( total <= 1.f);
	assert( total >= -1.f);
	return total;
}
