#ifndef INCLUDED_DEBUG_DRAWING_HPP
#define INCLUDED_DEBUG_DRAWING_HPP
#pragma once

//-----------------------------------------------------------------------------------------------
#include <vector>
#include "Graphics/Material.hpp"
#include "Graphics/Mesh.hpp"
#include "Graphics/Renderer.hpp"
#include "Math/FloatVector3.hpp"
#include "Color.hpp"

//-----------------------------------------------------------------------------------------------
namespace Debug
{
	enum DrawingVisibility
	{
		DRAW_ONLY_IF_VISIBLE = 0,
		DRAW_SKINNY_IF_OCCLUDED = 1,
		DRAW_ALWAYS = 2
	};

	struct ShapeVertexData
	{
		float x, y, z;
		float red, green, blue, alpha;
		float lifetimeRemainingSeconds;

		ShapeVertexData() { }

		ShapeVertexData( float lifetimeSeconds, float X, float Y, float Z, const Color& color ) 
			: lifetimeRemainingSeconds( lifetimeSeconds )
			, x( X )
			, y( Y )
			, z( Z )
			, red  ( color.r )
			, green( color.g )
			, blue ( color.b )
			, alpha( color.a )
		{ }

		ShapeVertexData( float lifetimeSeconds, const FloatVector3& position, const Color& color ) 
			: lifetimeRemainingSeconds( lifetimeSeconds )
			, x( position.x )
			, y( position.y )
			, z( position.z )
			, red  ( color.r )
			, green( color.g )
			, blue ( color.b )
			, alpha( color.a )
		{ }
	};

	class ShapeManager
	{
	public:
		ShapeManager() {}

		//Lifetime functions
		void Initialize();
		void Render();
		void Update( float deltaSeconds );

		//"Call anywhere" functions
		friend void DrawPointForTime( const FloatVector3& location, float size, const Color& color, DrawingVisibility visibility, float lifetimeSeconds );
		friend void DrawLineForTime( const FloatVector3& startLocation, const Color& startColor, const FloatVector3& endLocation, const Color& endColor, DrawingVisibility visibility, float lifetimeSeconds );
		friend void DrawAxesForTime( const FloatVector3& location, float axisLength, DrawingVisibility visibility, float lifetimeSeconds );

	private:
		//Rendering
		Material m_shapeRenderingMaterial;
		void CleanupDeadShapes();
		void RemoveDeadShapesFromDataArray( std::vector< ShapeVertexData >& vertexDataArray );

		void RenderShapesDrawnSkinnyWhenOccluded( const Renderer* renderer ) const;
		void RenderShapesDrawnOnlyWhenVisible( const Renderer* renderer ) const;
		void RenderShapesDrawnAlways( const Renderer* renderer ) const;

		void UpdateLifetimesInShapeDataArray( float deltaSeconds, std::vector< ShapeVertexData >& vertexDataArray );
		std::vector< ShapeVertexData > m_linesAlwaysDrawn;
		std::vector< ShapeVertexData > m_linesSkinnyOccluded;
		std::vector< ShapeVertexData > m_linesDrawnWhenVisible;
		std::vector< ShapeVertexData > m_triStripsAlwaysDrawn;
		std::vector< ShapeVertexData > m_triStripsSkinnyOccluded;
		std::vector< ShapeVertexData > m_triStripsDrawnWhenVisible;
	};

	extern ShapeManager g_shapeManager;

	inline void InitializeDrawingSystem() { g_shapeManager.Initialize(); }
	inline void RenderDrawings() { g_shapeManager.Render(); }
	inline void UpdateDrawings( float deltaSeconds ) { g_shapeManager.Update( deltaSeconds ); }


	//-----------------------------------------------------------------------------------------------
	class Drawing
	{
	public:
		enum Visibility
		{
			DRAW_ONLY_IF_VISIBLE = 0,
			DRAW_SKINNY_IF_OCCLUDED = 1,
			DRAW_ALWAYS = 2
		};

	private:
		bool				m_dead;
		float				m_lifetimeSeconds;
		unsigned int		m_numberOfVertices;
		//DrawingVertexData*	m_drawingVertices;
		Visibility			m_visibility;
		Renderer::Shape		m_shape;

	public:
		Drawing()
			: m_dead( false )
			, m_lifetimeSeconds( 0.f )
			, m_numberOfVertices( 0 )
			//, m_drawingVertices( nullptr )
			, m_visibility( DRAW_ONLY_IF_VISIBLE )
			, m_shape( Renderer::LINES )
		{ }

// 		Drawing( float lifetimeSeconds, unsigned int numberOfVertices, DrawingVertexData* pointerToVertices, Renderer::Shape shape, Visibility visibility )
// 			: m_dead( false )
// 			, m_lifetimeSeconds( lifetimeSeconds )
// 			, m_numberOfVertices( numberOfVertices )
// 			, m_drawingVertices( pointerToVertices )
// 			, m_visibility( visibility )
// 			, m_shape( shape )
// 		{ }

		Drawing( const Drawing& drawing )
			: m_dead( drawing.m_dead )
			, m_lifetimeSeconds( drawing.m_lifetimeSeconds )
			, m_numberOfVertices( drawing.m_numberOfVertices )
			//, m_drawingVertices( new DrawingVertexData[ m_numberOfVertices ] )
			, m_visibility( drawing.m_visibility )
			, m_shape( drawing.m_shape )
		{
			for( unsigned int i = 0; i < m_numberOfVertices; ++i )
			{
				//m_drawingVertices[ i ] = drawing.m_drawingVertices[ i ];
			}
		}

		~Drawing()
		{
			//FIX: Memory leak!!!
			//if( m_drawingVertices != nullptr )
				//delete[] m_drawingVertices;
		}

		Drawing& operator=( const Drawing& rhs )
		{
			if( this == &rhs ) return *this; //Check for self-assignment

			m_shape = rhs.m_shape;
			m_visibility = rhs.m_visibility;
			m_dead = rhs.m_dead;
			m_lifetimeSeconds = rhs.m_lifetimeSeconds;
			m_numberOfVertices = rhs.m_numberOfVertices;

			//delete[] m_drawingVertices;
// 			m_drawingVertices = new DrawingVertexData[ m_numberOfVertices ];
// 			for( unsigned int i = 0; i < m_numberOfVertices; ++i )
// 			{
// 				m_drawingVertices[ i ] = rhs.m_drawingVertices[ i ];
// 			}

			return *this;
		}

		float		GetLifetimeRemaining() const { return m_lifetimeSeconds; }
		Visibility	GetVisibility() const		 { return m_visibility; }
		bool		IsDead()					 { return m_dead; }
		void		SetDead()					 { m_dead = true; }

		void Render( float opacity ) const;
		void Update( float deltaSeconds ) { m_lifetimeSeconds -= deltaSeconds; }
	};



	//-----------------------------------------------------------------------------------------------
	extern std::vector< Debug::Drawing > g_activeDebugDrawings;
	extern Material g_debugDrawingMaterial;


	//-----------------------------------------------------------------------------------------------
	inline void DrawPointForTime( const FloatVector3& location, float size, const Color& color, DrawingVisibility visibility, float lifetimeSeconds )
	{
		std::vector< ShapeVertexData >* arrayToAddPoint = nullptr;

		switch( visibility )
		{
		case DRAW_ALWAYS:
			arrayToAddPoint = &g_shapeManager.m_linesAlwaysDrawn;
			break;
		case DRAW_ONLY_IF_VISIBLE:
			arrayToAddPoint = &g_shapeManager.m_linesDrawnWhenVisible;
			break;
		case DRAW_SKINNY_IF_OCCLUDED:
			arrayToAddPoint = &g_shapeManager.m_linesSkinnyOccluded;
			break;
		default:
			assert( false );
		}
		
 		static const unsigned int NUMBER_OF_VERTICES_IN_POINT = 6;
		arrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, location - FloatVector3( size, 0.f, 0.f ), color ) );
		arrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, location + FloatVector3( size, 0.f, 0.f ), color ) );
		arrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, location - FloatVector3( 0.f, size, 0.f ), color ) );
		arrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, location + FloatVector3( 0.f, size, 0.f ), color ) );
		arrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, location - FloatVector3( 0.f, 0.f, size ), color ) );
		arrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, location + FloatVector3( 0.f, 0.f, size ), color ) );
	}

	//-----------------------------------------------------------------------------------------------
	inline void DrawPoint( const FloatVector3& location, float size, const Color& color, DrawingVisibility visibility ) 
	{ 
		DrawPointForTime( location, size, color, visibility, 0.f ); 
	}

	//-----------------------------------------------------------------------------------------------
	inline void DrawLineForTime( const FloatVector3& startLocation, const Color& startColor, const FloatVector3& endLocation, const Color& endColor, DrawingVisibility visibility, float lifetimeSeconds )
	{
		std::vector< ShapeVertexData >* arrayToAddPoint = nullptr;

		switch( visibility )
		{
		case DRAW_ALWAYS:
			arrayToAddPoint = &g_shapeManager.m_linesAlwaysDrawn;
			break;
		case DRAW_ONLY_IF_VISIBLE:
			arrayToAddPoint = &g_shapeManager.m_linesDrawnWhenVisible;
			break;
		case DRAW_SKINNY_IF_OCCLUDED:
			arrayToAddPoint = &g_shapeManager.m_linesSkinnyOccluded;
			break;
		default:
			assert( false );
		}
		
 		static const unsigned int NUMBER_OF_VERTICES_IN_LINE = 2;
		arrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, startLocation, startColor ) );
		arrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, endLocation,   endColor   ) );
	}

	//-----------------------------------------------------------------------------------------------
	inline void DrawLine( const FloatVector3& startLocation, const Color& startColor, const FloatVector3& endLocation, const Color& endColor, DrawingVisibility visibility )
	{
		DrawLineForTime( startLocation, startColor, endLocation, endColor, visibility, 0.f );
	}

	//-----------------------------------------------------------------------------------------------
	inline void DrawAxesForTime( const FloatVector3& location, float axisLength, DrawingVisibility visibility, float lifetimeSeconds )
	{
		std::vector< ShapeVertexData >* arrayToAddPoint = nullptr;

		switch( visibility )
		{
		case DRAW_ALWAYS:
			arrayToAddPoint = &g_shapeManager.m_linesAlwaysDrawn;
			break;
		case DRAW_ONLY_IF_VISIBLE:
			arrayToAddPoint = &g_shapeManager.m_linesDrawnWhenVisible;
			break;
		case DRAW_SKINNY_IF_OCCLUDED:
			arrayToAddPoint = &g_shapeManager.m_linesSkinnyOccluded;
			break;
		default:
			assert( false );
		}

 		static const unsigned int NUMBER_OF_VERTICES_IN_AXES = 6;
		arrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, location,											Color( 1.f, 0.f, 0.f, 1.f ) ) );
		arrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, location.x + axisLength, location.y, location.z,	Color( 1.f, 0.f, 0.f, 1.f ) ) );
		arrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, location,											Color( 0.f, 1.f, 0.f, 1.f ) ) );
		arrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, location.x, location.y + axisLength, location.z,	Color( 0.f, 1.f, 0.f, 1.f ) ) );
		arrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, location,											Color( 0.f, 0.f, 1.f, 1.f ) ) );
		arrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, location.x, location.y, location.z + axisLength,	Color( 0.f, 0.f, 1.f, 1.f ) ) );
	}

	//-----------------------------------------------------------------------------------------------
	inline void DrawAxes( const FloatVector3& location, float axisLength, DrawingVisibility visibility )
	{
		DrawAxesForTime( location, axisLength, visibility, 0.f );
	}

	inline void GramSchmidt3D( FloatVector3& vector1, FloatVector3& vector2, FloatVector3& vector3 )
	{
		vector1.Normalize();
		vector2 = vector2 - ( DotProduct( vector2, vector1 ) / DotProduct( vector1, vector1 ) ) * vector1;
		vector2.Normalize();
		vector3 = vector3 - ( DotProduct( vector3, vector2 ) / DotProduct( vector2, vector2 ) ) * vector2 - ( DotProduct( vector3, vector1 ) / DotProduct( vector1, vector1 ) ) * vector1;
		vector3.Normalize();
	}

	//-----------------------------------------------------------------------------------------------
	inline void DrawArrowForTime( const FloatVector3& startLocation, const Color& , const FloatVector3& endLocation, const Color& , Drawing::Visibility , float )
	{
		static const float ARROWHEAD_SIZE = 1.f;
		FloatVector3 lineDirection = endLocation - startLocation;
		lineDirection.Normalize();
		FloatVector3 arrowheadStart = endLocation - ARROWHEAD_SIZE * 1.5f * lineDirection;

		static const unsigned int NUMBER_OF_VERTICES_IN_LINE = 2;
		//DrawingVertexData* lineVertices = new DrawingVertexData[ NUMBER_OF_VERTICES_IN_LINE ];
		//lineVertices[ 0 ] = DrawingVertexData( startLocation, startColor );
		//lineVertices[ 1 ] = DrawingVertexData( arrowheadStart, endColor );

		FloatVector3 orthogonalVector1( 0.f, 1.f, 0.f ), orthogonalVector2( 0.f, 0.f, 1.f );
		GramSchmidt3D( lineDirection, orthogonalVector1, orthogonalVector2 );

// 		static const unsigned int NUMBER_OF_VERTICES_IN_ARROWHEAD = 8;
// 		DrawingVertexData* arrowheadVertices = new DrawingVertexData[ NUMBER_OF_VERTICES_IN_ARROWHEAD ];
// 		arrowheadVertices[ 0 ] = DrawingVertexData( arrowheadStart + orthogonalVector1 - orthogonalVector2, endColor );
// 		arrowheadVertices[ 1 ] = DrawingVertexData( arrowheadStart - orthogonalVector1 + orthogonalVector2, endColor );
// 		arrowheadVertices[ 2 ] = DrawingVertexData( arrowheadStart - orthogonalVector1 - orthogonalVector2, endColor );
// 		arrowheadVertices[ 3 ] = DrawingVertexData( endLocation, endColor );
// 		arrowheadVertices[ 4 ] = arrowheadVertices[ 0 ];
// 		arrowheadVertices[ 5 ] = DrawingVertexData( arrowheadStart + orthogonalVector1 + orthogonalVector2, endColor );
// 		arrowheadVertices[ 6 ] = arrowheadVertices[ 1 ];
// 		arrowheadVertices[ 7 ] = arrowheadVertices[ 3 ];
// 
// 		g_activeDebugDrawings.push_back( Debug::Drawing( timeSeconds, NUMBER_OF_VERTICES_IN_LINE, lineVertices, Renderer::LINES, visibility ) );
// 		g_activeDebugDrawings.push_back( Debug::Drawing( timeSeconds, NUMBER_OF_VERTICES_IN_ARROWHEAD, arrowheadVertices, Renderer::TRIANGLE_STRIP, visibility ) );
	}

	//-----------------------------------------------------------------------------------------------
	inline void DrawArrow( const FloatVector3& , const Color& , const FloatVector3& , const Color& , Drawing::Visibility  )
	{
		//DrawArrowForTime( startLocation, startColor, endLocation, endColor, visibility, 0.f );
	}

	//-----------------------------------------------------------------------------------------------
	inline void DrawAABBForTime( const FloatVector3& , const FloatVector3& , const Color& , const Color& , Drawing::Visibility , float  )
	{
// 		static const unsigned int NUMBER_OF_VERTICES_FOR_EDGES = 24;
// 		DrawingVertexData* edgeVertices = new DrawingVertexData[ NUMBER_OF_VERTICES_FOR_EDGES ];
// 
// 		//Near side quad
// 		edgeVertices[  0 ] = DrawingVertexData( minLocation, edgeColor );
// 		edgeVertices[  1 ] = DrawingVertexData( minLocation.x, minLocation.y, maxLocation.z, edgeColor );
// 		edgeVertices[  2 ] = edgeVertices[ 1 ];
// 		edgeVertices[  3 ] = DrawingVertexData( maxLocation.x, minLocation.y, maxLocation.z, edgeColor );
// 		edgeVertices[  4 ] = edgeVertices[ 3 ];
// 		edgeVertices[  5 ] = DrawingVertexData( maxLocation.x, minLocation.y, minLocation.z, edgeColor );
// 		edgeVertices[  6 ] = edgeVertices[ 5 ];
// 		edgeVertices[  7 ] = edgeVertices[ 0 ];
// 
// 		//Far side quad
// 		edgeVertices[  8 ] = DrawingVertexData( maxLocation, edgeColor );
// 		edgeVertices[  9 ] = DrawingVertexData( minLocation.x, maxLocation.y, maxLocation.z, edgeColor );
// 		edgeVertices[ 10 ] = edgeVertices[  9 ];
// 		edgeVertices[ 11 ] = DrawingVertexData( minLocation.x, maxLocation.y, minLocation.z, edgeColor );
// 		edgeVertices[ 12 ] = edgeVertices[ 11 ];
// 		edgeVertices[ 13 ] = DrawingVertexData( maxLocation.x, maxLocation.y, minLocation.z, edgeColor );
// 		edgeVertices[ 14 ] = edgeVertices[ 13 ];
// 		edgeVertices[ 15 ] = edgeVertices[  8 ];
// 
// 		//Connecting lines
// 		edgeVertices[ 16 ] = DrawingVertexData( minLocation, edgeColor );
// 		edgeVertices[ 17 ] = DrawingVertexData( minLocation.x, maxLocation.y, minLocation.z, edgeColor );
// 		edgeVertices[ 18 ] = DrawingVertexData( maxLocation.x, minLocation.y, minLocation.z, edgeColor );
// 		edgeVertices[ 19 ] = DrawingVertexData( maxLocation.x, maxLocation.y, minLocation.z, edgeColor );
// 		edgeVertices[ 20 ] = DrawingVertexData( minLocation.x, minLocation.y, maxLocation.z, edgeColor );
// 		edgeVertices[ 21 ] = DrawingVertexData( minLocation.x, maxLocation.y, maxLocation.z, edgeColor );
// 		edgeVertices[ 22 ] = DrawingVertexData( maxLocation.x, minLocation.y, maxLocation.z, edgeColor );
// 		edgeVertices[ 23 ] = DrawingVertexData( maxLocation, edgeColor );
// 
// 		static const unsigned int NUMBER_OF_VERTICES_FOR_FACES = 14;
// 		DrawingVertexData* faceVertices = new DrawingVertexData[ NUMBER_OF_VERTICES_FOR_FACES ];
// 		faceVertices[  0 ] = DrawingVertexData( minLocation, faceColor );
// 		faceVertices[  1 ] = DrawingVertexData( maxLocation.x, minLocation.y, minLocation.z, faceColor );
// 		faceVertices[  2 ] = DrawingVertexData( minLocation.x, maxLocation.y, minLocation.z, faceColor );
// 		faceVertices[  3 ] = DrawingVertexData( maxLocation.x, maxLocation.y, minLocation.z, faceColor );
// 		faceVertices[  4 ] = DrawingVertexData( maxLocation, faceColor );
// 		faceVertices[  5 ] = faceVertices[ 1 ];
// 		faceVertices[  6 ] = DrawingVertexData( maxLocation.x, minLocation.y, maxLocation.z, faceColor );
// 		faceVertices[  7 ] = faceVertices[ 0 ];
// 		faceVertices[  8 ] = DrawingVertexData( minLocation.x, minLocation.y, maxLocation.z, faceColor );
// 		faceVertices[  9 ] = faceVertices[ 2 ];
// 		faceVertices[ 10 ] = DrawingVertexData( minLocation.x, maxLocation.y, maxLocation.z, faceColor );
// 		faceVertices[ 11 ] = faceVertices[ 4 ];
// 		faceVertices[ 12 ] = faceVertices[ 8 ];
// 		faceVertices[ 13 ] = faceVertices[ 6 ];
// 
// 		g_activeDebugDrawings.push_back( Debug::Drawing( timeSeconds, NUMBER_OF_VERTICES_FOR_EDGES, edgeVertices, Renderer::LINES, visibility ) );
// 		g_activeDebugDrawings.push_back( Debug::Drawing( timeSeconds, NUMBER_OF_VERTICES_FOR_FACES, faceVertices, Renderer::TRIANGLE_STRIP, visibility ) );
	}

	//-----------------------------------------------------------------------------------------------
	inline void DrawAABB( const FloatVector3& , const FloatVector3& , const Color& , const Color& , Drawing::Visibility  )
	{
		//DrawAABBForTime( minLocation, maxLocation, edgeColor, faceColor, visibility, 0.f );
	}

	//-----------------------------------------------------------------------------------------------
	void DrawSphereForTime( const FloatVector3& , float , const Color& , Drawing::Visibility , float  );

	//-----------------------------------------------------------------------------------------------
	inline void DrawSphere( const FloatVector3& , float , const Color& , Drawing::Visibility  )
	{
		//DrawSphereForTime( center, radius, color, visibility, 0.f );
	}
}

#endif //INCLUDED_DEBUG_DRAWING_HPP
