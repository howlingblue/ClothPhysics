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
		friend void DrawArrowForTime( const FloatVector3& startLocation, const Color& startColor, const FloatVector3& endLocation, const Color& endColor, DrawingVisibility visibility, float timelineSeconds );
		friend void DrawAABBForTime( const FloatVector3& minLocation, const FloatVector3& maxLocation, const Color& edgeColor, const Color& faceColor, DrawingVisibility visibility, float lifetimeSeconds );	
		friend void DrawSphereForTime( const FloatVector3& center, float radius, const Color& color, DrawingVisibility visibility, float lifetimeSeconds );

	private:
		void CleanupDeadShapes();
		void RemoveDeadShapesFromDataArray( std::vector< ShapeVertexData >& vertexDataArray );

		void RenderShapesDrawnSkinnyWhenOccluded( const Renderer* renderer ) const;
		void RenderShapesDrawnOnlyWhenVisible( const Renderer* renderer ) const;
		void RenderShapesDrawnAlways( const Renderer* renderer ) const;

		void UpdateLifetimesInShapeDataArray( float deltaSeconds, std::vector< ShapeVertexData >& vertexDataArray );

		Material m_shapeRenderingMaterial;
		int m_vertexAttributeID;
		int m_colorAttributeID;

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
			//Add a second set of vertices that will be drawn larger when visible
			DrawPointForTime( location, size, color, DRAW_ONLY_IF_VISIBLE, lifetimeSeconds );
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
			//Add a second set of vertices that will be drawn larger when visible
			DrawLineForTime( startLocation, startColor, endLocation, endColor, DRAW_ONLY_IF_VISIBLE, lifetimeSeconds );
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
			//Add a second set of vertices that will be drawn larger when visible
			DrawAxesForTime( location, axisLength, DRAW_ONLY_IF_VISIBLE, lifetimeSeconds );
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
	inline void DrawArrowForTime( const FloatVector3& startLocation, const Color& startColor, const FloatVector3& endLocation, const Color& endColor, DrawingVisibility visibility, float timelineSeconds )
	{
		static const float ARROWHEAD_SIZE = 1.f;
		FloatVector3 lineDirection = endLocation - startLocation;
		lineDirection.Normalize();
		FloatVector3 arrowheadStart = endLocation - ARROWHEAD_SIZE * 1.5f * lineDirection;


		std::vector< ShapeVertexData >* arrayToAddPoint = nullptr;
		switch( visibility )
		{
		case DRAW_ALWAYS:
			arrayToAddPoint = &g_shapeManager.m_triStripsAlwaysDrawn;
			DrawLineForTime( startLocation, startColor, arrowheadStart, endColor, DRAW_ALWAYS, timelineSeconds );
			break;
		case DRAW_SKINNY_IF_OCCLUDED:
			arrayToAddPoint = &g_shapeManager.m_triStripsSkinnyOccluded;
			DrawLineForTime( startLocation, startColor, arrowheadStart, endColor, DRAW_SKINNY_IF_OCCLUDED, timelineSeconds );
			DrawArrowForTime( startLocation, startColor, endLocation, endColor, DRAW_ONLY_IF_VISIBLE, timelineSeconds );
			break;
		case DRAW_ONLY_IF_VISIBLE:
			arrayToAddPoint = &g_shapeManager.m_triStripsDrawnWhenVisible;
			DrawLineForTime( startLocation, startColor, arrowheadStart, endColor, DRAW_ONLY_IF_VISIBLE, timelineSeconds );
			break;
		default:
			assert( false );
		}

		FloatVector3 orthogonalVector1( 0.f, 1.f, 0.f ), orthogonalVector2( 0.f, 0.f, 1.f );
		GramSchmidt3D( lineDirection, orthogonalVector1, orthogonalVector2 );

 		static const unsigned int NUMBER_OF_VERTICES_IN_ARROWHEAD = 8;
		arrayToAddPoint->push_back( ShapeVertexData( timelineSeconds, arrowheadStart + orthogonalVector1 - orthogonalVector2, endColor ) );
		arrayToAddPoint->push_back( ShapeVertexData( timelineSeconds, arrowheadStart - orthogonalVector1 + orthogonalVector2, endColor ) );
		arrayToAddPoint->push_back( ShapeVertexData( timelineSeconds, arrowheadStart - orthogonalVector1 - orthogonalVector2, endColor ) );
		arrayToAddPoint->push_back( ShapeVertexData( timelineSeconds, endLocation, endColor ) );
		arrayToAddPoint->push_back( ShapeVertexData( timelineSeconds, arrowheadStart + orthogonalVector1 - orthogonalVector2, endColor ) );
		arrayToAddPoint->push_back( ShapeVertexData( timelineSeconds, arrowheadStart + orthogonalVector1 + orthogonalVector2, endColor ) );
		arrayToAddPoint->push_back( ShapeVertexData( timelineSeconds, arrowheadStart - orthogonalVector1 + orthogonalVector2, endColor ) );
		arrayToAddPoint->push_back( ShapeVertexData( timelineSeconds, endLocation, endColor ) );
	}

	//-----------------------------------------------------------------------------------------------
	inline void DrawArrow( const FloatVector3& startLocation, const Color& startColor, const FloatVector3& endLocation, const Color& endColor, DrawingVisibility visibility )
	{
		DrawArrowForTime( startLocation, startColor, endLocation, endColor, visibility, 0.f );
	}

	//-----------------------------------------------------------------------------------------------
	inline void DrawAABBForTime( const FloatVector3& minLocation, const FloatVector3& maxLocation, const Color& edgeColor, const Color& faceColor, DrawingVisibility visibility, float lifetimeSeconds )
	{
		std::vector< ShapeVertexData >* lineArrayToAddPoint = nullptr;
		std::vector< ShapeVertexData >* triArrayToAddPoint = nullptr;
		
		switch( visibility )
		{
		case DRAW_ALWAYS:
			lineArrayToAddPoint = &g_shapeManager.m_linesAlwaysDrawn;
			triArrayToAddPoint = &g_shapeManager.m_triStripsAlwaysDrawn;
			break;
		case DRAW_SKINNY_IF_OCCLUDED:
			lineArrayToAddPoint = &g_shapeManager.m_linesSkinnyOccluded;
			triArrayToAddPoint = &g_shapeManager.m_triStripsSkinnyOccluded;
			break;
		case DRAW_ONLY_IF_VISIBLE:
			lineArrayToAddPoint = &g_shapeManager.m_linesDrawnWhenVisible;
			triArrayToAddPoint = &g_shapeManager.m_triStripsDrawnWhenVisible;
			break;
		default:
			assert( false );
		}

		static const unsigned int NUMBER_OF_VERTICES_FOR_EDGES = 24;

		//Near side quad
		lineArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, minLocation, edgeColor ) );
		lineArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, minLocation.x, minLocation.y, maxLocation.z, edgeColor ) );
		lineArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, minLocation.x, minLocation.y, maxLocation.z, edgeColor ) );
		lineArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, maxLocation.x, minLocation.y, maxLocation.z, edgeColor ) );
		lineArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, maxLocation.x, minLocation.y, maxLocation.z, edgeColor ) );
		lineArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, maxLocation.x, minLocation.y, minLocation.z, edgeColor ) );
		lineArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, maxLocation.x, minLocation.y, minLocation.z, edgeColor ) );
		lineArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, minLocation, edgeColor ) );

		//Far side quad
		lineArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, maxLocation, edgeColor ) );
		lineArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, minLocation.x, maxLocation.y, maxLocation.z, edgeColor ) );
		lineArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, minLocation.x, maxLocation.y, maxLocation.z, edgeColor ) );
		lineArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, minLocation.x, maxLocation.y, minLocation.z, edgeColor ) );
		lineArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, minLocation.x, maxLocation.y, minLocation.z, edgeColor ) );
		lineArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, maxLocation.x, maxLocation.y, minLocation.z, edgeColor ) );
		lineArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, maxLocation.x, maxLocation.y, minLocation.z, edgeColor ) );
		lineArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, maxLocation, edgeColor ) );

		//Connecting lines
		lineArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, minLocation, edgeColor ) );
		lineArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, minLocation.x, maxLocation.y, minLocation.z, edgeColor ) );
		lineArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, maxLocation.x, minLocation.y, minLocation.z, edgeColor ) );
		lineArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, maxLocation.x, maxLocation.y, minLocation.z, edgeColor ) );
		lineArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, minLocation.x, minLocation.y, maxLocation.z, edgeColor ) );
		lineArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, minLocation.x, maxLocation.y, maxLocation.z, edgeColor ) );
		lineArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, maxLocation.x, minLocation.y, maxLocation.z, edgeColor ) );
		lineArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, maxLocation, edgeColor ) );
		
		static const unsigned int NUMBER_OF_VERTICES_FOR_FACES = 14;
		triArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, minLocation, faceColor ) );
		triArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, maxLocation.x, minLocation.y, minLocation.z, faceColor ) );
		triArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, minLocation.x, maxLocation.y, minLocation.z, faceColor ) );
		triArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, maxLocation.x, maxLocation.y, minLocation.z, faceColor ) );
		triArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, maxLocation, faceColor ) );
		triArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, maxLocation.x, minLocation.y, minLocation.z, faceColor ) );
		triArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, maxLocation.x, minLocation.y, maxLocation.z, faceColor ) );
		triArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, minLocation, faceColor ) );
		triArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, minLocation.x, minLocation.y, maxLocation.z, faceColor ) );
		triArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, minLocation.x, maxLocation.y, minLocation.z, faceColor ) );
		triArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, minLocation.x, maxLocation.y, maxLocation.z, faceColor ) );
		triArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, maxLocation, faceColor ) );
		triArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, minLocation.x, minLocation.y, maxLocation.z, faceColor ) );
		triArrayToAddPoint->push_back( ShapeVertexData( lifetimeSeconds, maxLocation.x, minLocation.y, maxLocation.z, faceColor ) );
	}

	//-----------------------------------------------------------------------------------------------
	inline void DrawAABB( const FloatVector3& minLocation, const FloatVector3& maxLocation, const Color& edgeColor, const Color& faceColor, DrawingVisibility visibility )
	{
		DrawAABBForTime( minLocation, maxLocation, edgeColor, faceColor, visibility, 0.f );
	}

	//-----------------------------------------------------------------------------------------------
	void DrawSphereForTime( const FloatVector3& center, float radius, const Color& color, DrawingVisibility visibility, float lifetimeSeconds );

	//-----------------------------------------------------------------------------------------------
	inline void DrawSphere( const FloatVector3& center, float radius, const Color& color, DrawingVisibility visibility )
	{
		DrawSphereForTime( center, radius, color, visibility, 0.f );
	}
}

#endif //INCLUDED_DEBUG_DRAWING_HPP
