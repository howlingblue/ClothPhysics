#include "Math/EngineMath.hpp"
#include "DebugDrawing.hpp"

//-----------------------------------------------------------------------------------------------
std::vector< Debug::Drawing > Debug::g_activeDebugDrawings;
Material Debug::g_debugDrawingMaterial;

void Debug::InitializeDrawingSystem()
{
	g_debugDrawingMaterial.SetShaderProgram( ShaderProgram::CreateOrGetShaderProgram( "Data/Shaders/BasicNoTexture.vertex.330.glsl", "Data/Shaders/BasicNoTexture.fragment.330.glsl" ) );
	g_debugDrawingMaterial.SetModelMatrixUniform( "u_modelMatrix" );
	g_debugDrawingMaterial.SetViewMatrixUniform( "u_viewMatrix" );
	g_debugDrawingMaterial.SetProjectionMatrixUniform( "u_projectionMatrix" );
}

void Debug::RenderDrawings()
{
	Renderer* renderer = Renderer::GetRenderer();

	g_debugDrawingMaterial.Apply( renderer );

	for( unsigned int i = 0; i < Debug::g_activeDebugDrawings.size(); ++i )
	{

		switch( g_activeDebugDrawings[ i ].GetVisibility() )
		{
		case Drawing::DRAW_ALWAYS:
			renderer->DisableFeature( Renderer::DEPTH_TESTING );

			renderer->SetLineWidth( 5.f );
			Debug::g_activeDebugDrawings[ i ].Render( 1.f );

			renderer->EnableFeature( Renderer::DEPTH_TESTING );
			break;


		case Debug::Drawing::DRAW_SKINNY_IF_OCCLUDED:
			renderer->DisableFeature( Renderer::DEPTH_TESTING );

			renderer->SetLineWidth( 2.f );
			Debug::g_activeDebugDrawings[ i ].Render( 0.3f );

			renderer->EnableFeature( Renderer::DEPTH_TESTING );


		case Debug::Drawing::DRAW_ONLY_IF_VISIBLE:
		default:
			renderer->SetLineWidth( 5.f );
			Debug::g_activeDebugDrawings[ i ].Render( 1.f );
		}
	}

	g_debugDrawingMaterial.Remove( renderer );
}

void Debug::UpdateDrawings( float deltaSeconds )
{
	for( unsigned int i = 0; i < Debug::g_activeDebugDrawings.size(); ++i )
	{
		if( Debug::g_activeDebugDrawings[ i ].IsDead() )
		{
			Debug::g_activeDebugDrawings.erase( Debug::g_activeDebugDrawings.begin() + i );
			--i;
			continue;
		}

		Debug::g_activeDebugDrawings[ i ].Update( deltaSeconds );
		if( Debug::g_activeDebugDrawings[ i ].GetLifetimeRemaining() < 0.f )
			g_activeDebugDrawings[ i ].SetDead();
	}
}


//-----------------------------------------------------------------------------------------------
void Debug::Drawing::Render( float opacity ) const
{
	Renderer* renderer = Renderer::GetRenderer();

	static const int NUMBER_OF_VERTEX_COORDINATES = 3;
	static const int SIZE_OF_ARRAY_STRUCTURE = sizeof( DrawingVertexData );
	int vertexAttributeID = renderer->GetActiveShaderProgram()->GetAttributeIDFromName( "i_vertexWorldPosition" );
	renderer->BindVertexArraysToAttributeLocation( vertexAttributeID );
	renderer->SetPointerToGenericArray( vertexAttributeID, NUMBER_OF_VERTEX_COORDINATES, Renderer::FLOAT_TYPE, false, SIZE_OF_ARRAY_STRUCTURE, &m_drawingVertices[0] );


	if( opacity < 1.f )
		renderer->SetColor( m_drawingVertices[ 0 ].red, m_drawingVertices[ 0 ].green, m_drawingVertices[ 0 ].blue, std::min( opacity, m_drawingVertices[ 0 ].alpha ) );
	else
	{
		static const int NUMBER_OF_COLOR_COORDINATES = 4;
		int colorAttributeID = renderer->GetActiveShaderProgram()->GetAttributeIDFromName( "i_vertexColor" );
		renderer->BindVertexArraysToAttributeLocation( colorAttributeID );
		renderer->SetPointerToGenericArray( colorAttributeID, NUMBER_OF_COLOR_COORDINATES, Renderer::FLOAT_TYPE, false, SIZE_OF_ARRAY_STRUCTURE, &m_drawingVertices[0].red );
	}

	static const int VERTEX_ARRAY_START = 0;
	renderer->RenderVertexArray( m_shape, VERTEX_ARRAY_START, m_numberOfVertices );
}

void GenerateUnitIcosahedronAboutOrigin( std::vector< FloatVector3 >& out_triangleArray )
{
	out_triangleArray.clear();
	out_triangleArray.reserve( 60 );

	//These twelve points derive an icosahedron about the origin (Thanks Wikipedia!)
	FloatVector3 icoVertex0( -1.f,  GOLDEN_RATIO, 0.f );
	FloatVector3 icoVertex1(  1.f,  GOLDEN_RATIO, 0.f );
	FloatVector3 icoVertex2( -1.f, -GOLDEN_RATIO, 0.f );
	FloatVector3 icoVertex3(  1.f, -GOLDEN_RATIO, 0.f );

	FloatVector3 icoVertex4( 0.f, -1.f,  GOLDEN_RATIO );
	FloatVector3 icoVertex5( 0.f,  1.f,  GOLDEN_RATIO );
	FloatVector3 icoVertex6( 0.f, -1.f, -GOLDEN_RATIO );
	FloatVector3 icoVertex7( 0.f,  1.f, -GOLDEN_RATIO );

	FloatVector3 icoVertex8 (  GOLDEN_RATIO, 0.f, -1.f );
	FloatVector3 icoVertex9 (  GOLDEN_RATIO, 0.f,  1.f );
	FloatVector3 icoVertex10( -GOLDEN_RATIO, 0.f, -1.f );
	FloatVector3 icoVertex11( -GOLDEN_RATIO, 0.f,  1.f );

	//This mapping thanks to Andreas Kahler (http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html)
	out_triangleArray.push_back( icoVertex0 );
	out_triangleArray.push_back( icoVertex11 );
	out_triangleArray.push_back( icoVertex5 );

	out_triangleArray.push_back( icoVertex0 );
	out_triangleArray.push_back( icoVertex5 );
	out_triangleArray.push_back( icoVertex1 );

	out_triangleArray.push_back( icoVertex0 );
	out_triangleArray.push_back( icoVertex1 );
	out_triangleArray.push_back( icoVertex7 );

	out_triangleArray.push_back( icoVertex0 );
	out_triangleArray.push_back( icoVertex7 );
	out_triangleArray.push_back( icoVertex10 );

	out_triangleArray.push_back( icoVertex0 );
	out_triangleArray.push_back( icoVertex10 );
	out_triangleArray.push_back( icoVertex11 );

	out_triangleArray.push_back( icoVertex1 );
	out_triangleArray.push_back( icoVertex5 );
	out_triangleArray.push_back( icoVertex9 );

	out_triangleArray.push_back( icoVertex5 );
	out_triangleArray.push_back( icoVertex11 );
	out_triangleArray.push_back( icoVertex4 );

	out_triangleArray.push_back( icoVertex11 );
	out_triangleArray.push_back( icoVertex10 );
	out_triangleArray.push_back( icoVertex2 );

	out_triangleArray.push_back( icoVertex10 );
	out_triangleArray.push_back( icoVertex7 );
	out_triangleArray.push_back( icoVertex6 );

	out_triangleArray.push_back( icoVertex7 );
	out_triangleArray.push_back( icoVertex1 );
	out_triangleArray.push_back( icoVertex8 );

	out_triangleArray.push_back( icoVertex3 );
	out_triangleArray.push_back( icoVertex9 );
	out_triangleArray.push_back( icoVertex4 );

	out_triangleArray.push_back( icoVertex3 );
	out_triangleArray.push_back( icoVertex4 );
	out_triangleArray.push_back( icoVertex2 );

	out_triangleArray.push_back( icoVertex3 );
	out_triangleArray.push_back( icoVertex2 );
	out_triangleArray.push_back( icoVertex6 );

	out_triangleArray.push_back( icoVertex3 );
	out_triangleArray.push_back( icoVertex6 );
	out_triangleArray.push_back( icoVertex8 );

	out_triangleArray.push_back( icoVertex3 );
	out_triangleArray.push_back( icoVertex8 );
	out_triangleArray.push_back( icoVertex9 );

	out_triangleArray.push_back( icoVertex4 );
	out_triangleArray.push_back( icoVertex9 );
	out_triangleArray.push_back( icoVertex5 );

	out_triangleArray.push_back( icoVertex2 );
	out_triangleArray.push_back( icoVertex4 );
	out_triangleArray.push_back( icoVertex11 );

	out_triangleArray.push_back( icoVertex6 );
	out_triangleArray.push_back( icoVertex2 );
	out_triangleArray.push_back( icoVertex10 );

	out_triangleArray.push_back( icoVertex8 );
	out_triangleArray.push_back( icoVertex6 );
	out_triangleArray.push_back( icoVertex7 );

	out_triangleArray.push_back( icoVertex9 );
	out_triangleArray.push_back( icoVertex8 );
	out_triangleArray.push_back( icoVertex1 );

	//Normalize the points
	for( unsigned int i = 0; i < out_triangleArray.size(); ++i )
	{
		out_triangleArray[ i ].Normalize();
	}
}

//-----------------------------------------------------------------------------------------------
void GenerateUnitIcosahedralSphereAboutOrigin( std::vector< FloatVector3 >& out_triangleArray, unsigned int numberOfRecursions )
{
	GenerateUnitIcosahedronAboutOrigin( out_triangleArray );

	std::vector< FloatVector3 > workingArray;
	for( unsigned int recursion = 0; recursion < numberOfRecursions; ++recursion )
	{
		workingArray.reserve( 4 * out_triangleArray.size() );

		for( unsigned int pointIndex = 0; pointIndex < out_triangleArray.size(); pointIndex += 3 )
		{
			const FloatVector3& triPoint1 = out_triangleArray[ pointIndex ];
			const FloatVector3& triPoint2 = out_triangleArray[ pointIndex + 1 ];
			const FloatVector3& triPoint3 = out_triangleArray[ pointIndex + 2 ];

			FloatVector3 triSide1Middle = ( triPoint1 + triPoint2 ) * 0.5f;
			FloatVector3 triSide2Middle = ( triPoint2 + triPoint3 ) * 0.5f;
			FloatVector3 triSide3Middle = ( triPoint3 + triPoint1 ) * 0.5f;

			//Normalize points so that they remain on the unit sphere
			triSide1Middle.Normalize();
			triSide2Middle.Normalize();
			triSide3Middle.Normalize();

			workingArray.push_back( triPoint1 );
			workingArray.push_back( triSide1Middle );
			workingArray.push_back( triSide3Middle );

			workingArray.push_back( triPoint2 );
			workingArray.push_back( triSide2Middle );
			workingArray.push_back( triSide1Middle );

			workingArray.push_back( triPoint3 );
			workingArray.push_back( triSide3Middle );
			workingArray.push_back( triSide2Middle );

			workingArray.push_back( triSide1Middle );
			workingArray.push_back( triSide2Middle );
			workingArray.push_back( triSide3Middle );
		}

		out_triangleArray.clear();
		workingArray.swap( out_triangleArray );
	}
}

//-----------------------------------------------------------------------------------------------
void Debug::DrawSphereForTime( const FloatVector3& center, float radius, const Color& color, Debug::Drawing::Visibility visibility, float timeSeconds )
{
	std::vector< FloatVector3 > icosahedralSpherePoints;
	GenerateUnitIcosahedralSphereAboutOrigin( icosahedralSpherePoints, 2 );

	Debug::DrawingVertexData* finalTriangleArray = new Debug::DrawingVertexData[ icosahedralSpherePoints.size() ];
	for( unsigned int i = 0; i < icosahedralSpherePoints.size(); ++i )
	{
		finalTriangleArray[ i ] = DrawingVertexData( icosahedralSpherePoints[ i ] * radius + center, color );
	}

	g_activeDebugDrawings.push_back( Debug::Drawing( timeSeconds, icosahedralSpherePoints.size(), finalTriangleArray, Renderer::TRIANGLES, visibility ) );
}
