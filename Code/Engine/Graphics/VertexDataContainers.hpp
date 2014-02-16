#ifndef INCLUDED_VERTEX_DATA_CONTAINERS_HPP
#define INCLUDED_VERTEX_DATA_CONTAINERS_HPP
#pragma once

//-----------------------------------------------------------------------------------------------
#include "../Color.hpp"
#include "../Math/FloatVector2.hpp"
#include "../Math/FloatVector3.hpp"

//-----------------------------------------------------------------------------------------------
struct VertexColorData2D
{
	float x, y;
	float red, green, blue, alpha;

	VertexColorData2D( )
		: x( 0.f )
		, y( 0.f )
		, red( 0.f )
		, green( 0.f )
		, blue( 0.f )
		, alpha( 0.f )
	{ }

	VertexColorData2D( float vertexX, float vertexY, float colorR, float colorG, float colorB, float colorA )
		: x( vertexX )
		, y( vertexY )
		, red( colorR )
		, green( colorG )
		, blue( colorB )
		, alpha( colorA )
	{ }

	VertexColorData2D( const FloatVector2& vertex, const Color& color )
		: x( vertex.x )
		, y( vertex.y )
		, red( color.r )
		, green( color.g )
		, blue( color.b )
		, alpha( color.a )
	{ }

	void SetColor( const Color& color ) 
	{
		red = color.r;
		green = color.g;
		blue = color.b;
		alpha = color.a;
	}
};

//-----------------------------------------------------------------------------------------------
struct VertexColorData
{
	float x, y, z;
	float red, green, blue, alpha;

	VertexColorData( )
		: x( 0.f )
		, y( 0.f )
		, z( 0.f )
		, red( 0.f )
		, green( 0.f )
		, blue( 0.f )
		, alpha( 0.f )
	{ }

	VertexColorData( float vertexX, float vertexY, float vertexZ, float colorR, float colorG, float colorB, float colorA )
		: x( vertexX )
		, y( vertexY )
		, z( vertexZ )
		, red( colorR )
		, green( colorG )
		, blue( colorB )
		, alpha( colorA )
	{ }

	VertexColorData( const FloatVector3& vertex, const Color& color )
		: x( vertex.x )
		, y( vertex.y )
		, z( vertex.z )
		, red( color.r )
		, green( color.g )
		, blue( color.b )
		, alpha( color.a )
	{ }

	void SetColor( const Color& color ) 
	{
		red = color.r;
		green = color.g;
		blue = color.b;
		alpha = color.a;
	}
};

//-----------------------------------------------------------------------------------------------
struct VertexColorTextureData2D
{
	float x, y;
	float red, green, blue, alpha;
	float u, v;

	VertexColorTextureData2D( )
		: x( 0.f )
		, y( 0.f )
		, red( 0.f )
		, green( 0.f )
		, blue( 0.f )
		, alpha( 0.f )
		, u( 0.f )
		, v( 0.f )
	{ }

	VertexColorTextureData2D( float vertexX, float vertexY, float colorR, float colorG, float colorB, float colorA, float textureU, float textureV )
		: x( vertexX )
		, y( vertexY )
		, red( colorR )
		, green( colorG )
		, blue( colorB )
		, alpha( colorA )
		, u( textureU )
		, v( textureV )
	{ }

	VertexColorTextureData2D( const FloatVector2& vertex, const Color& color, const FloatVector2& textureUV )
		: x( vertex.x )
		, y( vertex.y )
		, red( color.r )
		, green( color.g )
		, blue( color.b )
		, alpha( color.a )
		, u( textureUV.x )
		, v( textureUV.y )
	{ }


	void SetColor( const Color& color ) 
	{
		red = color.r;
		green = color.g;
		blue = color.b;
		alpha = color.a;
	}
};

//-----------------------------------------------------------------------------------------------
struct VertexColorTextureData
{
	float x, y, z;
	float red, green, blue, alpha;
	float u, v;

	VertexColorTextureData( )
		: x( 0.f )
		, y( 0.f )
		, z( 0.f )
		, red( 0.f )
		, green( 0.f )
		, blue( 0.f )
		, alpha( 0.f )
		, u( 0.f )
		, v( 0.f )
	{ }

	VertexColorTextureData( float vertexX, float vertexY, float vertexZ, float colorR, float colorG, float colorB, float colorA, float textureU, float textureV )
		: x( vertexX )
		, y( vertexY )
		, z( vertexZ )
		, red( colorR )
		, green( colorG )
		, blue( colorB )
		, alpha( colorA )
		, u( textureU )
		, v( textureV )
	{ }

	VertexColorTextureData( const FloatVector3& vertex, const Color& color, const FloatVector2& textureUV )
		: x( vertex.x )
		, y( vertex.y )
		, z( vertex.z )
		, red( color.r )
		, green( color.g )
		, blue( color.b )
		, alpha( color.a )
		, u( textureUV.x )
		, v( textureUV.y )
	{ }

	void SetColor( const Color& color ) 
	{
		red = color.r;
		green = color.g;
		blue = color.b;
		alpha = color.a;
	}
};

//-----------------------------------------------------------------------------------------------
struct VertexColorNormalTextureData
{
	float posX, posY, posZ;
	float red, green, blue, alpha;
	float normX, normY, normZ;
	float u, v;

	VertexColorNormalTextureData( )
		: posX( 0.f )
		, posY( 0.f )
		, posZ( 0.f )
		, red( 0.f )
		, green( 0.f )
		, blue( 0.f )
		, alpha( 0.f )
		, normX( 0.f )
		, normY( 0.f )
		, normZ( 0.f )
		, u( 0.f )
		, v( 0.f )
	{ }

	VertexColorNormalTextureData( float vertexX, float vertexY, float vertexZ, 
								  float colorR, float colorG, float colorB, float colorA, 
								  float normalX, float normalY, float normalZ, 
								  float textureU, float textureV )
		: posX( vertexX )
		, posY( vertexY )
		, posZ( vertexZ )
		, red( colorR )
		, green( colorG )
		, blue( colorB )
		, alpha( colorA )
		, normX( normalX )
		, normY( normalY )
		, normZ( normalZ )
		, u( textureU )
		, v( textureV )
	{ }

	VertexColorNormalTextureData( const FloatVector3& vertex, const Color& color, const FloatVector3& normal, const FloatVector2& textureUV )
		: posX( vertex.x )
		, posY( vertex.y )
		, posZ( vertex.z )
		, red( color.r )
		, green( color.g )
		, blue( color.b )
		, alpha( color.a )
		, normX( normal.x )
		, normY( normal.y )
		, normZ( normal.z )
		, u( textureUV.x )
		, v( textureUV.y )
	{ }

	void SetColor( const Color& color ) 
	{
		red = color.r;
		green = color.g;
		blue = color.b;
		alpha = color.a;
	}
};

#endif //INCLUDED_VERTEX_DATA_CONTAINERS_HPP
