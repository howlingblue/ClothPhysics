#include "../Engine/Graphics/Renderer.hpp"
#include "AABB3D.hpp"

//-----------------------------------------------------------------------------------------------
bool AABB3D::IsCollidingWith( const AABB3D& other ) const
{
	if( this->boxMax.x < other.boxMin.x ||
		this->boxMax.y < other.boxMin.y ||
		this->boxMax.z < other.boxMin.z ||
		this->boxMin.x > other.boxMax.x ||
		this->boxMin.y > other.boxMax.y ||
		this->boxMin.z > other.boxMax.z )
	{
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------------------------
void AABB3D::Render() const
{
	Renderer* renderer = Renderer::GetRenderer();

	float boxXWidth  = boxMax.x - boxMin.x;
	float boxYLength = boxMax.y - boxMin.y;
	float boxZHeight = boxMax.z - boxMin.z;

	FloatVector3 m_vertices[ 24 ] = 
	{
		//Near side quad
		FloatVector3( 0.f,			0.f,		0.f ),
		FloatVector3( boxXWidth,	0.f,		0.f ),

		FloatVector3( boxXWidth,	0.f,		0.f ),
		FloatVector3( boxXWidth,	0.f,		boxZHeight ),

		FloatVector3( boxXWidth,	0.f,		boxZHeight ),
		FloatVector3( 0.f,			0.f,		boxZHeight ),

		FloatVector3( 0.f,			0.f,		boxZHeight ),
		FloatVector3( 0.f,			0.f,		0.f ),

		//Connectors between quads
		FloatVector3( 0.f,			0.f,		0.f ),
		FloatVector3( 0.f,			boxYLength,	0.f ),

		FloatVector3( boxXWidth,	0.f,		0.f ),
		FloatVector3( boxXWidth,	boxYLength,	0.f ),

		FloatVector3( 0.f,			0.f,		boxZHeight ),
		FloatVector3( 0.f,			boxYLength,	boxZHeight ),

		FloatVector3( boxXWidth,	0.f,		boxZHeight ),
		FloatVector3( boxXWidth,	boxYLength,	boxZHeight ),

		//Right side quad
		FloatVector3( 0.f,			boxYLength,		0.f ),
		FloatVector3( boxXWidth,	boxYLength,		0.f ),
		
		FloatVector3( boxXWidth,	boxYLength,		0.f ),
		FloatVector3( boxXWidth,	boxYLength,		boxZHeight ),
		
		FloatVector3( boxXWidth,	boxYLength,		boxZHeight ),
		FloatVector3( 0.f,			boxYLength,		boxZHeight ),
		
		FloatVector3( 0.f,			boxYLength,		boxZHeight ),
		FloatVector3( 0.f,			boxYLength,		0.f ),
	};

	renderer->PushMatrix();
	renderer->TranslateWorld( boxMin );

	renderer->EnableArrayType( Renderer::VERTEX_ARRAYS );

	static const int NUMBER_OF_VERTEX_COORDINATES = 3;
	static const int SIZE_OF_ARRAY_STRUCTURE = 0;
	renderer->SetPointerToVertexArray( NUMBER_OF_VERTEX_COORDINATES, Renderer::FLOAT_TYPE, SIZE_OF_ARRAY_STRUCTURE, &m_vertices );

	static const int VERTEX_ARRAY_START = 0;
	static const int NUMBER_OF_POINTS_IN_VERTEX_ARRAY = 24;

	renderer->SetColor( 1.f, 1.f, 1.f, 1.f );
	renderer->SetLineWidth( 5.f );
	renderer->RenderVertexArray( Renderer::LINES, VERTEX_ARRAY_START, NUMBER_OF_POINTS_IN_VERTEX_ARRAY );

	renderer->DisableArrayType( Renderer::VERTEX_ARRAYS );
	renderer->PopMatrix();
}
