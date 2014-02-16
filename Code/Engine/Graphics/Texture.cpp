#include "../EngineDefines.hpp"
#include "Renderer.hpp"
#include "stb_image.h"
#include "Texture.hpp"

//-----------------------------------------------------------------------------------------------
STATIC std::map< std::string, Texture* > Texture::s_activeTextureRegistry;
STATIC const unsigned int Texture::DEFAULT_LEVEL_OF_DETAIL = 0;

//-----------------------------------------------------------------------------------------------
Texture::Texture( const std::string& textureFileLocation, FilteringMethod filterMethod, WrappingMode wrapMode )
	: m_openGLTextureID( 0 )
	, m_imageWidth( 0 )
	, m_imageHeight( 0 )
	, m_fileLocation( textureFileLocation )
{
	int numberOfColorComponents, requestedNumberOfColorComponents = 0;
	unsigned char* textureData = stbi_load( textureFileLocation.c_str(), &m_imageWidth, &m_imageHeight, 
											&numberOfColorComponents, requestedNumberOfColorComponents );
	// FIX: Handle error case where texture file does not load
	Renderer::ColorComponents bufferTextureComponentFormat;
	if( numberOfColorComponents == 4 )
		bufferTextureComponentFormat = Renderer::RGBA;
	else
		bufferTextureComponentFormat = Renderer::RGB;
	Renderer::ColorComponents cardTextureComponentFormat = bufferTextureComponentFormat;
	// FIX: This doesn't correctly handle exotic component formats

	Renderer* renderer = Renderer::GetRenderer();

	static const unsigned int BYTE_ALIGNED = 1;
	renderer->SetTextureInputImageAlignment( BYTE_ALIGNED );

	renderer->GenerateTextureIDs( 1, &m_openGLTextureID );
	renderer->BindTexture( Renderer::TEXTURES_2D, this );

	switch( wrapMode )
	{
	case clampToEdge:
		renderer->SetTextureWrappingMode( Renderer::TEXTURES_2D, Renderer::CLAMP_TO_EDGE );
		break;
	case repeatOverGeometry:
		renderer->SetTextureWrappingMode( Renderer::TEXTURES_2D, Renderer::REPEAT_OVER_GEOMETRY );
		break;
	default:
		return;
	}

	switch( filterMethod )
	{
	case nearestNeighbor:
		renderer->SetTextureMagnificationMode( Renderer::TEXTURES_2D, Renderer::NEAREST_NEIGHBOR );
		break;
	case linearInterpolation:
		renderer->SetTextureMagnificationMode( Renderer::TEXTURES_2D, Renderer::LINEAR_INTERPOLATION );
		break;
	default:
		return;
	}
	renderer->SetTextureMinificationMode( Renderer::TEXTURES_2D, Renderer::INTERPOLATE_MIPMAPS_NEAREST_TEXTURE );

	renderer->CreateTextureFrom2DImage( Renderer::TEXTURES_2D, 
										DEFAULT_LEVEL_OF_DETAIL,
										cardTextureComponentFormat,
										m_imageWidth,
										m_imageHeight,
										bufferTextureComponentFormat,
										Renderer::UNSIGNED_BYTE,
										textureData );

	renderer->SetMaximumMipmapLevel( Renderer::TEXTURES_2D, 4 );
	renderer->SetMipmapQuality( Renderer::FASTEST );
	renderer->GenerateMipmaps( Renderer::TEXTURES_2D );

	stbi_image_free( textureData );

	s_activeTextureRegistry[ textureFileLocation ] = this;
}

//-----------------------------------------------------------------------------------------------
Texture::~Texture() { }

//-----------------------------------------------------------------------------------------------
STATIC Texture* Texture::CreateOrGetTexture( const std::string& textureFileLocation, FilteringMethod filterMethod, WrappingMode wrapMode )
{
	std::map< std::string, Texture* >::iterator textureIterator = s_activeTextureRegistry.find( textureFileLocation );

	if( textureIterator == s_activeTextureRegistry.end() )
	{
		Texture* texture = new Texture( textureFileLocation, filterMethod, wrapMode );
		return texture;
	}

	return textureIterator->second;
}

//-----------------------------------------------------------------------------------------------
STATIC void Texture::CleanUpTextureRepository()
{
	Renderer* renderer = Renderer::GetRenderer();

	Texture* texture;
	for( std::map< std::string, Texture* >::iterator pair = s_activeTextureRegistry.begin();
													 pair != s_activeTextureRegistry.end();
													 ++pair )
	{
		texture = pair->second;
		renderer->DeleteTextureDataOnCard( texture );
		delete texture;
	}
	s_activeTextureRegistry.clear();
}
