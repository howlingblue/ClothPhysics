#ifndef INCLUDED_TEXTURE_HPP
#define INCLUDED_TEXTURE_HPP
#pragma once

//-----------------------------------------------------------------------------------------------
#include <map>
#include <string>
#include "../Math/FloatVector2.hpp"

//-----------------------------------------------------------------------------------------------
class Texture
{
public:
	static enum FilteringMethod
	{
		nearestNeighbor		= 0,
		linearInterpolation = 1
	};

	static enum WrappingMode
	{
		clampToEdge			= 0,
		repeatOverGeometry  = 1
	};

private:
	static std::map< std::string, Texture* > s_activeTextureRegistry;
	static const unsigned int DEFAULT_LEVEL_OF_DETAIL;

	std::string m_fileLocation;
	int m_imageWidth, m_imageHeight;
	unsigned int m_openGLTextureID;

public:
	Texture( const std::string& textureFileLocation, FilteringMethod filterMethod, WrappingMode wrapMode );

	~Texture();

	unsigned int GetID() const { return m_openGLTextureID; }

	static Texture* CreateOrGetTexture( const std::string& textureFileLocation, FilteringMethod filterMethod, WrappingMode wrapMode );
	static void CleanUpTextureRepository();
};

#endif //INCLUDED_TEXTURE_HPP
