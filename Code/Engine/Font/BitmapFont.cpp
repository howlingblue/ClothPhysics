#include "../XML/pugixml.hpp"
#include "BitmapFont.hpp"
#include <sstream>

unsigned int ConvertStringToUnsignedInt( const std::string& numberString )
{
	unsigned int output;
	std::istringstream stringConverter( numberString );
	stringConverter >> output;
	return output;
}

float ConvertStringToFloat( const std::string& numberString )
{
	float output;

	std::istringstream stringConverter( numberString );
	stringConverter >> output;

	return output;
}

FloatVector2 ConvertStringToFloatVector2( const std::string& numberString )
{
	float floatX = 0.f, floatY = 0.f;

	std::size_t commaLocation = numberString.find( "," );

	std::string floatXstring( numberString.begin(), numberString.begin() + commaLocation );
	std::istringstream stringConverter( floatXstring );
	stringConverter >> floatX;

	//Add 1 to commaLocation to avoid the string at the start
	std::string floatYstring( numberString.begin() + commaLocation + 1, numberString.end() );
	stringConverter.str( floatYstring );
	stringConverter.clear(); //Clear any eof flags, since we probably reached the end after the last convert
	stringConverter >> floatY;

	return FloatVector2( floatX, floatY );
}

//-----------------------------------------------------------------------------------------------
void BitmapFont::LoadFontMetadataFromXML( const std::string& fontMetadataXMLfileLocation )
{
	pugi::xml_document fontXMLmetadata;

	pugi::xml_parse_result xmlParseResult = fontXMLmetadata.load_file( fontMetadataXMLfileLocation.c_str() );
	if( xmlParseResult != true )
	{
		exit( -14 );
	}

	pugi::xml_node rootNode = fontXMLmetadata.child( "FontDefinition" );

	pugi::xml_node fontInfoNode = rootNode.child( "FontInfo" );
	m_fontName = fontInfoNode.attribute( "name" ).value();
	m_numberOfTextureSheets = ConvertStringToUnsignedInt( fontInfoNode.attribute( "numTextureSheets" ).value() );

	for (pugi::xml_node glyphData = rootNode.child( "Glyph" ); glyphData != false; glyphData = glyphData.next_sibling( "Glyph" ) )
	{
		Glyph newGlyph( ConvertStringToUnsignedInt( glyphData.attribute( "sheet" ).value() ),
						ConvertStringToFloatVector2( glyphData.attribute( "texCoordMins" ).value() ),
						ConvertStringToFloatVector2( glyphData.attribute( "texCoordMaxs" ).value() ),
						ConvertStringToFloat( glyphData.attribute( "ttfA" ).value() ),
						ConvertStringToFloat( glyphData.attribute( "ttfB" ).value() ),
						ConvertStringToFloat( glyphData.attribute( "ttfC" ).value() ) );

		unsigned int ucsIndex = ConvertStringToUnsignedInt( glyphData.attribute( "ucsIndex" ).value() );
		m_glyphData[ ucsIndex ] = newGlyph;
	}
}

//-----------------------------------------------------------------------------------------------
BitmapFont::BitmapFont( const std::string& fontDefinitionXMLlocation, 
	const std::string* fontAtlasLocations, unsigned int numberOfAtlases )
{
	LoadFontMetadataFromXML( fontDefinitionXMLlocation );

	for( unsigned int textureID = 0; textureID < numberOfAtlases; ++textureID )
	{
		Texture* newAtlas = Texture::CreateOrGetTexture( fontAtlasLocations[ textureID ], Texture::linearInterpolation, Texture::clampToEdge );
		m_glyphAtlases.push_back( newAtlas );
	}
}

//-----------------------------------------------------------------------------------------------
float BitmapFont::GetWidthOfCharacter( char character, float fontHeight ) const
{
	const Glyph& characterGlyph = ( *m_glyphData.find( character ) ).second;

	float preAdvanceWidth = characterGlyph.m_advanceBeforeDrawA * fontHeight;
	float characterWidth = characterGlyph.m_fontWidthB * fontHeight;
	float postAdvanceWidth = characterGlyph.m_advanceAfterDrawC * fontHeight;

	return preAdvanceWidth + characterWidth + postAdvanceWidth;
}
