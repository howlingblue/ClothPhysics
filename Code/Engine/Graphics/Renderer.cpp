#include "Renderer.hpp"
#include "OpenGLRenderer.hpp"
#include "../../Game/EngineSettings.hpp"

//-----------------------------------------------------------------------------------------------
STATIC Renderer* Renderer::s_renderer = nullptr;

//-----------------------------------------------------------------------------------------------
VertexColorTextureData2D* Renderer::BuildGlyphArrayFromString( unsigned int& out_totalGlyphVertices, const std::string& textString, const BitmapFont& font, 
																float cellHeight, const FloatVector2& textStartLowerLeftCorner, const Color& textColor )
{
	static const unsigned int VERTICES_PER_GLYPH = 6; // four vertices for the glyph, two for sewing strips together.
	out_totalGlyphVertices = VERTICES_PER_GLYPH * textString.length() - 2; //The last two strip sewing vertices are unneeded.
	VertexColorTextureData2D* glyphVertexArray = new VertexColorTextureData2D[ out_totalGlyphVertices ];

	float minY = textStartLowerLeftCorner.y;
	float maxY = minY + cellHeight;
	float startX = textStartLowerLeftCorner.x;
	float endX = startX;

	for( unsigned int i = 0; i < textString.length(); ++i )
	{
		const Glyph& glyph = font.GetGlyphForCharacter( textString[ i ] );

		startX = endX + glyph.m_advanceBeforeDrawA * cellHeight;
		endX = startX + glyph.m_fontWidthB * cellHeight;

		glyphVertexArray[ VERTICES_PER_GLYPH * i + 0 ] = VertexColorTextureData2D( FloatVector2( startX, minY ), textColor, FloatVector2( glyph.m_textureUVMins.x, glyph.m_textureUVMaxs.y ) );
		glyphVertexArray[ VERTICES_PER_GLYPH * i + 1 ] = VertexColorTextureData2D( FloatVector2( endX,   minY ), textColor, glyph.m_textureUVMaxs );
		glyphVertexArray[ VERTICES_PER_GLYPH * i + 2 ] = VertexColorTextureData2D( FloatVector2( startX, maxY ), textColor, glyph.m_textureUVMins );
		glyphVertexArray[ VERTICES_PER_GLYPH * i + 3 ] = VertexColorTextureData2D( FloatVector2( endX,   maxY ), textColor, FloatVector2( glyph.m_textureUVMaxs.x, glyph.m_textureUVMins.y ) );

		endX += glyph.m_advanceAfterDrawC * cellHeight;

		//Create four degenerate triangles to sew the triangle strips together.
		//On the last loop, we can't look ahead, so just don't do it at all (and save four vertices in the bargain.)
		if( i + 1 != textString.length() )
		{
			glyphVertexArray[ VERTICES_PER_GLYPH * i + 4 ] = glyphVertexArray[ VERTICES_PER_GLYPH * i + 3 ];

			const Glyph& nextGlyph = font.GetGlyphForCharacter( textString[ i + 1 ] );
			glyphVertexArray[ VERTICES_PER_GLYPH * i + 5 ] = VertexColorTextureData2D( FloatVector2( endX + nextGlyph.m_advanceBeforeDrawA * cellHeight,   minY ), textColor, FloatVector2( 0.f, 0.f ) );
		}
	}

	return glyphVertexArray;
}

//-----------------------------------------------------------------------------------------------
STATIC void Renderer::CreateRenderer()
{
	#ifdef GRAPHICS_USE_OPENGL
		s_renderer = new OpenGLRenderer();
		s_renderer->Initialize();
	#endif

	if( s_renderer == nullptr )
		exit( -1 );
}

//-----------------------------------------------------------------------------------------------
float Renderer::CalculateTextWidth( const std::string& textString, const BitmapFont& font, float textHeight )
{
	return CalculateTextWidthFrom( textString, font, textHeight, 0, textString.length() );
}

//-----------------------------------------------------------------------------------------------
float Renderer::CalculateTextWidthFrom( const std::string& textString, const BitmapFont& font, float textHeight, unsigned int startIndex, unsigned int endIndex )
{
	float textWidth = 0.f;

	if( startIndex > textString.length() || endIndex > textString.length() )
		return textWidth;

	for( unsigned int i = startIndex; i < endIndex; ++i )
	{
		textWidth += font.GetWidthOfCharacter( textString[ i ], textHeight );
	}
	return textWidth;
}

//-----------------------------------------------------------------------------------------------
void RenderTextGlyphs( const Renderer* renderer, const VertexColorTextureData2D* vertexData, unsigned int numberOfVertices )
{
	static const int NUMBER_OF_VERTEX_COORDINATES = 2;
	static const int SIZE_OF_ARRAY_STRUCTURE = sizeof( VertexColorTextureData2D );
	renderer->SetPointerToVertexArray( NUMBER_OF_VERTEX_COORDINATES, Renderer::FLOAT_TYPE, SIZE_OF_ARRAY_STRUCTURE, &vertexData[ 0 ] );

	static const int NUMBER_OF_COLOR_COORDINATES = 4;
	renderer->SetPointerToColorArray( NUMBER_OF_COLOR_COORDINATES, Renderer::FLOAT_TYPE, SIZE_OF_ARRAY_STRUCTURE, &vertexData[ 0 ].red );

	static const int NUMBER_OF_TEXTURE_COORDINATES = 2;
	renderer->SetPointerToTextureCoordinateArray( NUMBER_OF_TEXTURE_COORDINATES, Renderer::FLOAT_TYPE, SIZE_OF_ARRAY_STRUCTURE, &vertexData[ 0 ].u );

	static const int VERTEX_ARRAY_START = 0;
	renderer->RenderVertexArray( Renderer::TRIANGLE_STRIP, 0, numberOfVertices );
}

//-----------------------------------------------------------------------------------------------
void RenderTextBackground( const Renderer* renderer, const VertexColorData2D* vertexData, unsigned int numberOfVertices )
{
	static const int SIZE_OF_QUAD_ARRAY_STRUCTURE = sizeof( VertexColorData2D );
	static const int NUMBER_OF_VERTEX_COORDINATES = 2;
	renderer->SetPointerToVertexArray( NUMBER_OF_VERTEX_COORDINATES, Renderer::FLOAT_TYPE, SIZE_OF_QUAD_ARRAY_STRUCTURE, &vertexData[ 0 ] );

	static const int NUMBER_OF_COLOR_COORDINATES = 4;
	renderer->SetPointerToColorArray( NUMBER_OF_COLOR_COORDINATES, Renderer::FLOAT_TYPE, SIZE_OF_QUAD_ARRAY_STRUCTURE, &vertexData[ 0 ].red );
	renderer->RenderVertexArray( Renderer::TRIANGLE_STRIP, 0, numberOfVertices );
}

//-----------------------------------------------------------------------------------------------
void Renderer::Render2DText( const std::string& textString, const BitmapFont& font, float cellHeight, const FloatVector2& textStartLowerLeftCorner,
							const Color& textColor, const Color& backgroundColor, const Color& textShadowColor )
{
	unsigned int totalGlyphVertices = 0;
	VertexColorTextureData2D* textQuadVertices = BuildGlyphArrayFromString( totalGlyphVertices, textString, font, cellHeight, textStartLowerLeftCorner, textShadowColor );


	static const unsigned int NUMBER_OF_BACKGROUND_QUAD_VERTICES = 4;
	float widthOfGlyphedText = s_renderer->CalculateTextWidth( textString, font, cellHeight );
	VertexColorData2D backgroundQuad[ NUMBER_OF_BACKGROUND_QUAD_VERTICES ] = 
	{
		VertexColorData2D( FloatVector2( textStartLowerLeftCorner.x,						textStartLowerLeftCorner.y ),				backgroundColor ),
		VertexColorData2D( FloatVector2( textStartLowerLeftCorner.x + widthOfGlyphedText,   textStartLowerLeftCorner.y ),				backgroundColor ),
		VertexColorData2D( FloatVector2( textStartLowerLeftCorner.x,						textStartLowerLeftCorner.y + cellHeight ),	backgroundColor ),
		VertexColorData2D( FloatVector2( textStartLowerLeftCorner.x + widthOfGlyphedText,   textStartLowerLeftCorner.y + cellHeight ),	backgroundColor )
	};
	s_renderer->PushMatrix();

	s_renderer->EnableArrayType( Renderer::VERTEX_ARRAYS );
	s_renderer->EnableArrayType( Renderer::COLOR_ARRAYS );



	const ShaderProgram* activeShader = GetActiveShaderProgram();
	unsigned int shaderTextureToggleID = s_renderer->GetUniformVariableLocation( activeShader, "u_texturesEnabled" );
	s_renderer->SetUniformVariable( shaderTextureToggleID, 0 );

	RenderTextBackground( s_renderer, backgroundQuad, NUMBER_OF_BACKGROUND_QUAD_VERTICES );

	s_renderer->SetUniformVariable( shaderTextureToggleID, 1 );


	s_renderer->EnableArrayType( Renderer::TEXTURE_COORD_ARRAYS );

	s_renderer->BindTexture( Renderer::TEXTURES_2D, font.GetTextureSheet( 0 ) );

	float shadowOffset = .05f * cellHeight;
	s_renderer->TranslateWorld( FloatVector3( shadowOffset, -shadowOffset, 0.f ) );
	RenderTextGlyphs( s_renderer, textQuadVertices, totalGlyphVertices );
	for( unsigned int i = 0; i < totalGlyphVertices; ++i )
	{
		textQuadVertices[ i ].SetColor( textColor );
	}
	s_renderer->TranslateWorld( FloatVector3( -shadowOffset, shadowOffset, 0.f ) );
	RenderTextGlyphs( s_renderer, textQuadVertices, totalGlyphVertices );

	s_renderer->DisableArrayType( Renderer::TEXTURE_COORD_ARRAYS );
	s_renderer->DisableArrayType( Renderer::COLOR_ARRAYS );
	s_renderer->DisableArrayType( Renderer::VERTEX_ARRAYS );

	s_renderer->PopMatrix();

	delete[] textQuadVertices;
}
