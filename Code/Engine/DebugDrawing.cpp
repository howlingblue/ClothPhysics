#include "Math/EngineMath.hpp"
#include "DebugDrawing.hpp"

//-----------------------------------------------------------------------------------------------
Debug::ShapeManager Debug::g_shapeManager;

//-----------------------------------------------------------------------------------------------
void Debug::ShapeManager::Initialize()
{
	Renderer* renderer = Renderer::GetRenderer();

	m_shapeRenderingMaterial.SetShaderProgram( ShaderProgram::CreateOrGetShaderProgram( "Data/Shaders/BasicNoTexture.vertex.330.glsl", "Data/Shaders/BasicNoTexture.fragment.330.glsl" ) );
	m_shapeRenderingMaterial.SetModelMatrixUniform( "u_modelMatrix" );
	m_shapeRenderingMaterial.SetViewMatrixUniform( "u_viewMatrix" );
	m_shapeRenderingMaterial.SetProjectionMatrixUniform( "u_projectionMatrix" );

	m_vertexAttributeID = m_shapeRenderingMaterial.GetShaderProgram()->GetAttributeIDFromName( "i_vertexWorldPosition" );
	renderer->BindVertexArraysToAttributeLocation( m_vertexAttributeID );

	m_colorAttributeID = m_shapeRenderingMaterial.GetShaderProgram()->GetAttributeIDFromName( "i_vertexColor" );
	renderer->BindVertexArraysToAttributeLocation( m_colorAttributeID );
}

//-----------------------------------------------------------------------------------------------
void Debug::ShapeManager::Render()
{
	Renderer* renderer = Renderer::GetRenderer();

	m_shapeRenderingMaterial.Apply( renderer );

	RenderShapesDrawnSkinnyWhenOccluded( renderer );
	RenderShapesDrawnOnlyWhenVisible( renderer );
	RenderShapesDrawnAlways( renderer );

	m_shapeRenderingMaterial.Remove( renderer );

	CleanupDeadShapes();
}

//-----------------------------------------------------------------------------------------------
void Debug::ShapeManager::Update( float deltaSeconds )
{
	UpdateLifetimesInShapeDataArray( deltaSeconds, m_linesDrawnWhenVisible );
	UpdateLifetimesInShapeDataArray( deltaSeconds, m_linesSkinnyOccluded );
	UpdateLifetimesInShapeDataArray( deltaSeconds, m_linesAlwaysDrawn );

	UpdateLifetimesInShapeDataArray( deltaSeconds, m_triStripsDrawnWhenVisible );
	UpdateLifetimesInShapeDataArray( deltaSeconds, m_triStripsSkinnyOccluded );
	UpdateLifetimesInShapeDataArray( deltaSeconds, m_triStripsAlwaysDrawn );
}

//-----------------------------------------------------------------------------------------------
void Debug::ShapeManager::CleanupDeadShapes()
{
	RemoveDeadShapesFromDataArray( m_linesDrawnWhenVisible );
	RemoveDeadShapesFromDataArray( m_linesSkinnyOccluded );
	RemoveDeadShapesFromDataArray( m_linesAlwaysDrawn );

	RemoveDeadShapesFromDataArray( m_triStripsDrawnWhenVisible );
	RemoveDeadShapesFromDataArray( m_triStripsSkinnyOccluded );
	RemoveDeadShapesFromDataArray( m_triStripsAlwaysDrawn );
}

//-----------------------------------------------------------------------------------------------
void Debug::ShapeManager::RenderShapesDrawnSkinnyWhenOccluded( const Renderer* renderer ) const
{
	renderer->DisableFeature( Renderer::DEPTH_TESTING );
	renderer->SetLineWidth( 2.f );

	static const int SIZE_OF_ARRAY_STRUCTURE = sizeof( ShapeVertexData );
	static const int NUMBER_OF_VERTEX_COORDINATES = 3;
	static const int NUMBER_OF_COLOR_COORDINATES = 4;
	static const int VERTEX_ARRAY_START = 0;

	if( m_linesSkinnyOccluded.size() > 0 )
	{
		renderer->SetPointerToGenericArray( m_vertexAttributeID, NUMBER_OF_VERTEX_COORDINATES, Renderer::FLOAT_TYPE, false, SIZE_OF_ARRAY_STRUCTURE, &m_linesSkinnyOccluded[0].x );
		renderer->SetPointerToGenericArray( m_colorAttributeID, NUMBER_OF_COLOR_COORDINATES, Renderer::FLOAT_TYPE, false, SIZE_OF_ARRAY_STRUCTURE, &m_linesSkinnyOccluded[0].red );
		renderer->RenderVertexArray( Renderer::LINES, VERTEX_ARRAY_START, m_linesSkinnyOccluded.size() );
	}

	if( m_triStripsSkinnyOccluded.size() > 0 )
	{
		renderer->SetPointerToGenericArray( m_vertexAttributeID, NUMBER_OF_VERTEX_COORDINATES, Renderer::FLOAT_TYPE, false, SIZE_OF_ARRAY_STRUCTURE, &m_triStripsSkinnyOccluded[0].x );
		renderer->SetPointerToGenericArray( m_colorAttributeID, NUMBER_OF_COLOR_COORDINATES, Renderer::FLOAT_TYPE, false, SIZE_OF_ARRAY_STRUCTURE, &m_triStripsSkinnyOccluded[0].red );
		renderer->RenderVertexArray( Renderer::TRIANGLE_STRIP, VERTEX_ARRAY_START, m_triStripsSkinnyOccluded.size() );
	}

	renderer->EnableFeature( Renderer::DEPTH_TESTING );
}

//-----------------------------------------------------------------------------------------------
void Debug::ShapeManager::RenderShapesDrawnOnlyWhenVisible( const Renderer* renderer ) const
{
	renderer->SetLineWidth( 5.f );

	static const int SIZE_OF_ARRAY_STRUCTURE = sizeof( ShapeVertexData );
	static const int NUMBER_OF_VERTEX_COORDINATES = 3;
	static const int NUMBER_OF_COLOR_COORDINATES = 4;
	static const int VERTEX_ARRAY_START = 0;

	if( m_linesDrawnWhenVisible.size() > 0 )
	{
		renderer->SetPointerToGenericArray( m_vertexAttributeID, NUMBER_OF_VERTEX_COORDINATES, Renderer::FLOAT_TYPE, false, SIZE_OF_ARRAY_STRUCTURE, &m_linesDrawnWhenVisible[0].x );
		renderer->SetPointerToGenericArray( m_colorAttributeID, NUMBER_OF_COLOR_COORDINATES, Renderer::FLOAT_TYPE, false, SIZE_OF_ARRAY_STRUCTURE, &m_linesDrawnWhenVisible[0].red );
		renderer->RenderVertexArray( Renderer::LINES, VERTEX_ARRAY_START, m_linesDrawnWhenVisible.size() );
	}

	if( m_triStripsDrawnWhenVisible.size() > 0 )
	{
		renderer->SetPointerToGenericArray( m_vertexAttributeID, NUMBER_OF_VERTEX_COORDINATES, Renderer::FLOAT_TYPE, false, SIZE_OF_ARRAY_STRUCTURE, &m_triStripsDrawnWhenVisible[0].x );
		renderer->SetPointerToGenericArray( m_colorAttributeID, NUMBER_OF_COLOR_COORDINATES, Renderer::FLOAT_TYPE, false, SIZE_OF_ARRAY_STRUCTURE, &m_triStripsDrawnWhenVisible[0].red );
		renderer->RenderVertexArray( Renderer::TRIANGLE_STRIP, VERTEX_ARRAY_START, m_triStripsDrawnWhenVisible.size() );
	}
}

//-----------------------------------------------------------------------------------------------
void Debug::ShapeManager::RenderShapesDrawnAlways( const Renderer* renderer ) const
{
	renderer->DisableFeature( Renderer::DEPTH_TESTING );
	renderer->SetLineWidth( 5.f );

	static const int SIZE_OF_ARRAY_STRUCTURE = sizeof( ShapeVertexData );
	static const int NUMBER_OF_VERTEX_COORDINATES = 3;
	static const int NUMBER_OF_COLOR_COORDINATES = 4;
	static const int VERTEX_ARRAY_START = 0;

	if( m_linesAlwaysDrawn.size() > 0 )
	{
		renderer->SetPointerToGenericArray( m_vertexAttributeID, NUMBER_OF_VERTEX_COORDINATES, Renderer::FLOAT_TYPE, false, SIZE_OF_ARRAY_STRUCTURE, &m_linesAlwaysDrawn[0].x );
		renderer->SetPointerToGenericArray( m_colorAttributeID, NUMBER_OF_COLOR_COORDINATES, Renderer::FLOAT_TYPE, false, SIZE_OF_ARRAY_STRUCTURE, &m_linesAlwaysDrawn[0].red );
		renderer->RenderVertexArray( Renderer::LINES, VERTEX_ARRAY_START, m_linesAlwaysDrawn.size() );
	}

	if( m_triStripsAlwaysDrawn.size() > 0 )
	{
		renderer->SetPointerToGenericArray( m_vertexAttributeID, NUMBER_OF_VERTEX_COORDINATES, Renderer::FLOAT_TYPE, false, SIZE_OF_ARRAY_STRUCTURE, &m_triStripsAlwaysDrawn[0].x );
		renderer->SetPointerToGenericArray( m_colorAttributeID, NUMBER_OF_COLOR_COORDINATES, Renderer::FLOAT_TYPE, false, SIZE_OF_ARRAY_STRUCTURE, &m_triStripsAlwaysDrawn[0].red );
		renderer->RenderVertexArray( Renderer::TRIANGLE_STRIP, VERTEX_ARRAY_START, m_triStripsAlwaysDrawn.size() );
	}

	renderer->EnableFeature( Renderer::DEPTH_TESTING );
}

//-----------------------------------------------------------------------------------------------
void Debug::ShapeManager::RemoveDeadShapesFromDataArray( std::vector< ShapeVertexData >& vertexDataArray )
{
	for( unsigned int i = 0; i < vertexDataArray.size(); ++i )
	{
		if( vertexDataArray[ i ].lifetimeRemainingSeconds <= 0.f )
		{
			vertexDataArray.erase( vertexDataArray.begin() + i );
			--i;
			continue;
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Debug::ShapeManager::UpdateLifetimesInShapeDataArray( float deltaSeconds, std::vector< ShapeVertexData >& vertexDataArray )
{
	for( unsigned int i = 0; i < vertexDataArray.size(); ++i )
	{
		vertexDataArray[ i ].lifetimeRemainingSeconds -= deltaSeconds;
	}
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
void Debug::DrawSphereForTime( const FloatVector3& center, float radius, const Color& color, DrawingVisibility visibility, float lifetimeSeconds )
{
	std::vector< ShapeVertexData >* arrayToAddPoint = nullptr;

	switch( visibility )
	{
	case DRAW_ALWAYS:
		arrayToAddPoint = &g_shapeManager.m_triStripsAlwaysDrawn;
		break;
	case DRAW_ONLY_IF_VISIBLE:
		arrayToAddPoint = &g_shapeManager.m_triStripsDrawnWhenVisible;
		break;
	case DRAW_SKINNY_IF_OCCLUDED:
		arrayToAddPoint = &g_shapeManager.m_triStripsSkinnyOccluded;
		//Add a second set of vertices that will be drawn larger when visible
		DrawSphereForTime( center, radius, Color( color.r, color.g, color.b, 0.5 * color.a ), DRAW_ONLY_IF_VISIBLE, lifetimeSeconds );
		break;
	default:
		assert( false );
	}

 	std::vector< FloatVector3 > icosahedralSpherePoints;
 	GenerateUnitIcosahedralSphereAboutOrigin( icosahedralSpherePoints, 2 );
 
 	for( unsigned int i = 0; i < icosahedralSpherePoints.size(); ++i )
 	{
		arrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, icosahedralSpherePoints[ i ] * radius + center, color ) );
 	}
}
