#ifndef INCLUDED_OPEN_GL_RENDERER_HPP
#define INCLUDED_OPEN_GL_RENDERER_HPP
#pragma once

//-----------------------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>
#pragma comment( lib, "opengl32" ) // Link in the OpenGL32.lib static library
#include "glext.h"
#include "Renderer.hpp"

#pragma region OpenGL_Function_Pointer_Declarations
//-----------------------------------------------------------------------------------------------
//Vertex Buffers
extern PFNGLBINDBUFFERPROC				 glBindBuffer;
extern PFNGLBUFFERDATAPROC				 glBufferData;
extern PFNGLGENBUFFERSPROC				 glGenBuffers;
extern PFNGLPRIMITIVERESTARTINDEXPROC	 glPrimitiveRestartIndex;
extern PFNGLVERTEXATTRIBPOINTERPROC		 glVertexAttribPointer;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray;

//Mipmaps
extern PFNGLGENERATEMIPMAPPROC	glGenerateMipmap;

//Shaders
extern PFNGLCOMPILESHADERPROC		glCompileShader;
extern PFNGLCREATESHADERPROC		glCreateShader;
extern PFNGLGETSHADERINFOLOGPROC	glGetShaderInfoLog;
extern PFNGLGETSHADERIVPROC			glGetShaderiv;
extern PFNGLSHADERSOURCEPROC		glShaderSource;

//Shader Programs
extern PFNGLATTACHSHADERPROC		glAttachShader;
extern PFNGLCREATEPROGRAMPROC		glCreateProgram;
extern PFNGLDELETEPROGRAMPROC		glDeleteProgram;
extern PFNGLDELETESHADERPROC		glDeleteShader;
extern PFNGLDETACHSHADERPROC		glDetachShader;
extern PFNGLGETACTIVEATTRIBPROC		glGetActiveAttrib;
extern PFNGLGETACTIVEUNIFORMPROC	glGetActiveUniform;
extern PFNGLGETATTRIBLOCATIONPROC	glGetAttribLocation;
extern PFNGLGETUNIFORMLOCATIONPROC	glGetUniformLocation;
extern PFNGLGETPROGRAMIVPROC		glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC	glGetProgramInfoLog;
extern PFNGLLINKPROGRAMPROC			glLinkProgram;
extern PFNGLUSEPROGRAMPROC			glUseProgram;

extern PFNGLUNIFORM1FPROC			glUniform1f;
extern PFNGLUNIFORM2FPROC			glUniform2f;
extern PFNGLUNIFORM3FPROC			glUniform3f;
extern PFNGLUNIFORM4FPROC			glUniform4f;

extern PFNGLUNIFORM1FVPROC			glUniform1fv;
extern PFNGLUNIFORM2FVPROC			glUniform2fv;
extern PFNGLUNIFORM3FVPROC			glUniform3fv;
extern PFNGLUNIFORM4FVPROC			glUniform4fv;

extern PFNGLUNIFORM1IPROC			glUniform1i;
extern PFNGLUNIFORM2IPROC			glUniform2i;
extern PFNGLUNIFORM3IPROC			glUniform3i;
extern PFNGLUNIFORM4IPROC			glUniform4i;

extern PFNGLUNIFORM1IVPROC			glUniform1iv;
extern PFNGLUNIFORM2IVPROC			glUniform2iv;
extern PFNGLUNIFORM3IVPROC			glUniform3iv;
extern PFNGLUNIFORM4IVPROC			glUniform4iv;

extern PFNGLUNIFORMMATRIX4FVPROC	glUniformMatrix4fv;

//Textures
extern PFNGLACTIVETEXTUREPROC		glActiveTexture;
#pragma endregion

//-----------------------------------------------------------------------------------------------
STATIC class OpenGLRenderer : Renderer
{
	friend class Renderer;
public:

private:
	//Don't allow other Plebian programmers to call our singleton's constructor.
	OpenGLRenderer()
		: Renderer() 
	{ }

	//Copy and assign are not allowed
	OpenGLRenderer( const OpenGLRenderer& );
	void operator=( const OpenGLRenderer& );

	void Initialize();
	void InitializeOpenGLFunctionPointers();

public:
	//Feature Enabling
	void EnableArrayType( ArrayType type ) const;
	void DisableArrayType( ArrayType type ) const;
	void BindVertexArraysToAttributeLocation( unsigned int location ) const;
	void UnbindVertexArraysFromAttributeLocation( unsigned int location ) const;

	void EnableFeature( Feature feature ) const;
	void DisableFeature( Feature feature ) const;

	//Color and Depth Buffers
	void ClearColorBuffer() const;
	void ClearDepthBuffer() const;
	void SetColorBufferClearValue( float red, float green, float blue, float alpha );
	void SetDepthBufferClearValue( float depthBetweenZeroAndOne );
	void DisableDepthBufferWriting() const;
	void EnableDepthBufferWriting() const;

	//Draw Modification
	void SetAlphaBlendingFunction( ColorBlendingMode sourceBlendingFactor, ColorBlendingMode destinationBlendingFactor ) const;
	void SetColor( float red, float green, float blue, float alpha ) const;
	void SetLineWidth( float widthPixels ) const;
	void SetPointSize( float pointSize ) const;

	//Mipmaps
	void GenerateMipmaps( Feature textureType );
	void SetMaximumMipmapLevel( Feature textureType, unsigned int maxLevel );
	void SetMipmapQuality( QualityLevel qualityLevel );

	//Shaders
	int CompileShader( Shader shaderType, const char* sourceString, std::string& out_errors );
	int CreateProgramFromShaders( int vertexShaderID, int geometryShaderID, int pixelFragmentShaderID, std::string& out_errors );
	void DeleteProgramDataOnCard( ShaderProgram* program );
	void DeleteShaderDataOnCard( int shaderID );
	void DetachShaderFromProgram( int shaderID, int programID );
	int GetAttributeLocation( const ShaderProgram* program, const std::string& attributeName );
	int GetNumberOfAttributesInProgram( const ShaderProgram* program ) const;
	int GetNumberOfUniformsInProgram( const ShaderProgram* program ) const;
	void GetShaderAttributeName( const ShaderProgram* program, unsigned int attributeIndex, std::string& out_attributeName );
	void GetShaderUniformName( const ShaderProgram* program, unsigned int uniformIndex, std::string& out_uniformName );
	int GetUniformVariableLocation( const ShaderProgram* program, const std::string& constantName );
	void SetUniformVariable( int programConstantLocation, int setting );
	void SetUniformVariable( int programConstantLocation, float setting );
	void SetUniformVariable( int programConstantLocation, const IntVector2& vector2 );
	void SetUniformVariable( int programConstantLocation, const FloatVector2& vector2 );
	void SetUniformVariable( int programConstantLocation, const FloatVector3& vector3 );
	void SetUniformVariable( int programConstantLocation, const FloatVector4& vector4 );
	void SetUniformVariable( int programConstantLocation, const Float4x4Matrix& matrix );
	void UseShaderProgram( const ShaderProgram* program );

	//Textures
	void BindTexture( Feature textureType, const Texture* texture ) const;
	void CreateTextureFrom2DImage( Feature textureType, unsigned int mipmapLevel, ColorComponents cardColorComponentFormat, 
								   unsigned int imageWidth, unsigned int imageHeight, ColorComponents inputColorComponentFormat, 
								   CoordinateType pixelDataType, const void* imageData );
	void DeleteTextureDataOnCard( Texture* texture );
	void GenerateTextureIDs( unsigned int numberOfTextureIDs, unsigned int *arrayOfTextureIDs );
	void SetActiveTextureUnit( unsigned int textureUnitNumber );
	void SetTextureInputImageAlignment( unsigned int bytePackingOneTwoFourOrEight );
	void SetTextureMagnificationMode( Feature textureType, TextureFilteringMethod magnificationMethod );
	void SetTextureMinificationMode( Feature textureType, TextureFilteringMethod minificationMethod );
	void SetTextureWrappingMode( Feature textureType, TextureWrapMode wrapMode );

	//Vertex Arrays
	void RenderPartOfArray( Shape drawingShape, unsigned int numberPointsToDraw, CoordinateType indexType, const void* firstIndexToRender ) const;
	void RenderVertexArray( Shape drawingShape, unsigned int startingArrayIndex, unsigned int numberPointsInArray ) const;
	void SetPointerToColorArray( unsigned int coordinatesPerVertex, CoordinateType coordinateType, unsigned int gapBetweenVertices, const void* firstVertexInArray ) const;
	void SetPointerToGenericArray( unsigned int variableLocation, int numberOfVertexCoordinates, CoordinateType coordinateType, bool normalizeData, unsigned int gapBetweenVertices, const void* firstVertexInArray );
	void SetPointerToTextureCoordinateArray( unsigned int coordinatesPerVertex, CoordinateType coordinateType, unsigned int gapBetweenVertices, const void* firstVertexInArray ) const;
	void SetPointerToVertexArray( unsigned int coordinatesPerVertex, CoordinateType coordinateType, unsigned int gapBetweenVertices, const void* firstVertexInArray ) const;
	void SetShapeRestartIndex( unsigned int index );

	//Vertex Buffer Objects
	void BindBufferObject( BufferType bufferType, unsigned int bufferID );
	void GenerateBuffer( unsigned int numberOfBuffersToGenerate, unsigned int* arrayOfBufferIDs );
	void SendDataToBuffer( BufferType bufferType, unsigned int sizeOfBufferBytes, const void* dataToSendToBuffer );
};

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::Initialize()
{
	InitializeOpenGLFunctionPointers();
	SetShapeRestartIndex( 0xFFFF );
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++ Feature Enabling +++++++++++++++++++++++++++++++++++++++++++++++++++
//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::EnableArrayType( ArrayType type ) const { glEnableClientState( type ); }

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::DisableArrayType( ArrayType type ) const { glDisableClientState( type ); }

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::BindVertexArraysToAttributeLocation( unsigned int location ) const { glEnableVertexAttribArray( location ); }

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::UnbindVertexArraysFromAttributeLocation( unsigned int location ) const { glDisableVertexAttribArray( location ); }

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::EnableFeature( Feature feature ) const { glEnable( feature ); }

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::DisableFeature( Feature feature ) const { glDisable( feature ); }






//+++++++++++++++++++++++++++++++++++++++++++++++++++ Color and Depth Buffers +++++++++++++++++++++++++++++++++++++++++++++++++++
//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::ClearColorBuffer() const { glClear( GL_COLOR_BUFFER_BIT ); }

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::ClearDepthBuffer() const { glClear( GL_DEPTH_BUFFER_BIT ); }

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::SetColorBufferClearValue( float red, float green, float blue, float alpha )
{
	glClearColor( red, green, blue, alpha );
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::SetDepthBufferClearValue( float depthBetweenZeroAndOne )
{
	glClearDepth( depthBetweenZeroAndOne );
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::DisableDepthBufferWriting() const { glDepthMask( GL_FALSE ); }

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::EnableDepthBufferWriting() const { glDepthMask( GL_TRUE ); }







//+++++++++++++++++++++++++++++++++++++++++++++++++++ Draw Modification +++++++++++++++++++++++++++++++++++++++++++++++++++
//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::SetAlphaBlendingFunction( ColorBlendingMode sourceBlendingFactor, ColorBlendingMode destinationBlendingFactor ) const
{
	glBlendFunc( sourceBlendingFactor, destinationBlendingFactor );
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::SetColor( float red, float green, float blue, float alpha ) const { glColor4f( red, green, blue, alpha ); }

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::SetLineWidth( float widthPixels ) const { glLineWidth( widthPixels ); }

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::SetPointSize( float pointSize ) const { glPointSize( pointSize ); }






//+++++++++++++++++++++++++++++++++++++++++++++++++++ Mipmaps +++++++++++++++++++++++++++++++++++++++++++++++++++
//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::GenerateMipmaps( Feature textureType ) { glGenerateMipmap( textureType ); }

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::SetMaximumMipmapLevel( Feature textureType, unsigned int maxLevel ) { glTexParameteri( textureType, GL_TEXTURE_MAX_LEVEL, maxLevel ); }

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::SetMipmapQuality( QualityLevel qualityLevel ) { glHint( GL_GENERATE_MIPMAP_HINT, qualityLevel ); }








//+++++++++++++++++++++++++++++++++++++++++++++++++++ Shaders +++++++++++++++++++++++++++++++++++++++++++++++++++
//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::DeleteProgramDataOnCard( ShaderProgram* program )
{
	glDeleteProgram( program->GetID() );
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::DeleteShaderDataOnCard( int shaderID )
{
	glDeleteShader( shaderID );
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::DetachShaderFromProgram( int shaderID, int programID )
{
	glDetachShader( programID, shaderID );
}

//-----------------------------------------------------------------------------------------------
inline int OpenGLRenderer::GetAttributeLocation( const ShaderProgram* program, const std::string& attributeName )
{
	return glGetAttribLocation( program->GetID(), attributeName.c_str() );
}

//-----------------------------------------------------------------------------------------------
inline int OpenGLRenderer::GetNumberOfAttributesInProgram( const ShaderProgram* program ) const
{
	int numberOfAttributes = 0;
	glGetProgramiv( program->GetID(), GL_ACTIVE_ATTRIBUTES, &numberOfAttributes );
	return numberOfAttributes;
}

//-----------------------------------------------------------------------------------------------
inline int OpenGLRenderer::GetNumberOfUniformsInProgram( const ShaderProgram* program ) const
{
	int numberOfUniforms = 0;
	glGetProgramiv( program->GetID(), GL_ACTIVE_UNIFORMS, &numberOfUniforms );
	return numberOfUniforms;
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::GetShaderAttributeName( const ShaderProgram* program, unsigned int attributeIndex, std::string& out_attributeName )
{
	static const unsigned int maxBufferLength = 256;
	out_attributeName.resize( maxBufferLength );
	GLsizei actualNameLength = 0;
	GLint attributeSize = 0;
	GLenum attributeType;

	glGetActiveAttrib( program->GetID(), attributeIndex, maxBufferLength, &actualNameLength, &attributeSize, &attributeType, &out_attributeName[ 0 ] );
	out_attributeName.resize( actualNameLength );
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::GetShaderUniformName( const ShaderProgram* program, unsigned int uniformIndex, std::string& out_uniformName )
{
	static const unsigned int maxBufferLength = 256;
	out_uniformName.resize( maxBufferLength );
	GLsizei actualNameLength = 0;
	GLint uniformSize = 0;
	GLenum uniformType;
	glGetActiveUniform( program->GetID(), uniformIndex, maxBufferLength, &actualNameLength, &uniformSize, &uniformType, &out_uniformName[ 0 ] );
	out_uniformName.resize( actualNameLength );
}

//-----------------------------------------------------------------------------------------------
inline int OpenGLRenderer::GetUniformVariableLocation( const ShaderProgram* program, const std::string& constantName )
{
	return glGetUniformLocation( program->GetID(), constantName.c_str() );
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::SetUniformVariable( int programConstantLocation, int setting ) 
{ 
	glUniform1i( programConstantLocation, setting ); 
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::SetUniformVariable( int programConstantLocation, float setting ) 
{ 
	glUniform1f( programConstantLocation, setting ); 
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::SetUniformVariable( int programConstantLocation, const IntVector2& vector2 )
{
	glUniform2i( programConstantLocation, vector2.x, vector2.y ); 
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::SetUniformVariable( int programConstantLocation, const FloatVector2& vector2 )
{
	glUniform2f( programConstantLocation, vector2.x, vector2.y );
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::SetUniformVariable( int programConstantLocation, const FloatVector3& vector3 )
{
	glUniform3f( programConstantLocation, vector3.x, vector3.y, vector3.z );
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::SetUniformVariable( int programConstantLocation, const FloatVector4& vector4 )
{
	glUniform4f( programConstantLocation, vector4.x, vector4.y, vector4.z, vector4.w );
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::SetUniformVariable( int programConstantLocation, const Float4x4Matrix& matrix )
{
	glUniformMatrix4fv( programConstantLocation, 1, false, matrix.GetRawBuffer() );
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::UseShaderProgram( const ShaderProgram* program ) 
{ 
	glUseProgram( program->GetID() );
	s_activeShaderProgram = program;
}








//+++++++++++++++++++++++++++++++++++++++++++++++++++ Textures +++++++++++++++++++++++++++++++++++++++++++++++++++
//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::BindTexture( Feature textureType, const Texture* texture ) const { glBindTexture( textureType, texture->GetID() ); }

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::CreateTextureFrom2DImage( Feature textureType, unsigned int mipmapLevel, ColorComponents cardColorComponentFormat, 
													  unsigned int imageWidth, unsigned int imageHeight, ColorComponents inputColorComponentFormat, 
													  CoordinateType pixelDataType, const void* imageData )
{
	static const unsigned int NO_IMAGE_BORDERS = 0;
	glTexImage2D( textureType, mipmapLevel, cardColorComponentFormat, imageWidth, imageHeight, NO_IMAGE_BORDERS, inputColorComponentFormat, pixelDataType, imageData );
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::DeleteTextureDataOnCard( Texture* texture ) 
{ 
	unsigned int textureID = texture->GetID();
	glDeleteTextures(1, (GLuint*) &textureID ); 
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::GenerateTextureIDs( unsigned int numberOfTextureIDs, unsigned int *arrayOfTextureIDs )
{
	glGenTextures( numberOfTextureIDs, (GLuint*) arrayOfTextureIDs );
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::SetActiveTextureUnit( unsigned int textureUnitNumber )
{
	glActiveTexture( GL_TEXTURE0 + textureUnitNumber );
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::SetTextureInputImageAlignment( unsigned int bytePackingOneTwoFourOrEight )
{
	glPixelStorei( GL_UNPACK_ALIGNMENT, bytePackingOneTwoFourOrEight );
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::SetTextureMagnificationMode( Feature textureType, TextureFilteringMethod magnificationMethod )
{
	glTexParameteri( textureType, GL_TEXTURE_MAG_FILTER, magnificationMethod );
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::SetTextureMinificationMode( Feature textureType, TextureFilteringMethod minificationMethod )
{
	glTexParameteri( textureType, GL_TEXTURE_MIN_FILTER, minificationMethod );
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::SetTextureWrappingMode( Feature textureType, TextureWrapMode wrapMode )
{
	glTexParameteri( textureType, GL_TEXTURE_WRAP_S, wrapMode );
	glTexParameteri( textureType, GL_TEXTURE_WRAP_T, wrapMode );
}






//+++++++++++++++++++++++++++++++++++++++++++++++++++ Vertex Arrays +++++++++++++++++++++++++++++++++++++++++++++++++++
//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::RenderPartOfArray( Shape drawingShape, unsigned int numberPointsToDraw, CoordinateType indexType, const void* firstIndexToRender ) const
{
	//glLoadMatrixf( m_matrixStack.top().GetRawBuffer() );
	glDrawElements( drawingShape, numberPointsToDraw, indexType, firstIndexToRender );
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::RenderVertexArray( Shape drawingShape, unsigned int startingArrayIndex, unsigned int numberPointsInArray ) const
{
	//glLoadMatrixf( m_matrixStack.top().GetRawBuffer() );
	glDrawArrays( drawingShape, startingArrayIndex, numberPointsInArray );
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::SetPointerToColorArray( unsigned int coordinatesPerVertex, CoordinateType coordinateType, unsigned int gapBetweenVertices, const void* firstVertexInArray ) const
{
	glColorPointer( coordinatesPerVertex, coordinateType, gapBetweenVertices, firstVertexInArray );
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::SetPointerToGenericArray( unsigned int variableLocation, int numberOfVertexCoordinates, CoordinateType coordinateType, bool normalizeData, unsigned int gapBetweenVertices, const void* firstVertexInArray )
{
	glVertexAttribPointer( variableLocation, numberOfVertexCoordinates, coordinateType, normalizeData, gapBetweenVertices, firstVertexInArray );
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::SetPointerToTextureCoordinateArray( unsigned int coordinatesPerVertex, CoordinateType coordinateType, unsigned int gapBetweenVertices, const void* firstVertexInArray ) const
{
	glTexCoordPointer( coordinatesPerVertex, coordinateType, gapBetweenVertices, firstVertexInArray );
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::SetPointerToVertexArray( unsigned int coordinatesPerVertex, CoordinateType coordinateType, unsigned int gapBetweenVertices, const void* firstVertexInArray ) const
{
	glVertexPointer( coordinatesPerVertex, coordinateType, gapBetweenVertices, firstVertexInArray );
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::SetShapeRestartIndex( unsigned int index )
{
	glPrimitiveRestartIndex( index );
}






//+++++++++++++++++++++++++++++++++++++++++++++++++++ Vertex Buffer Objects +++++++++++++++++++++++++++++++++++++++++++++++++++
//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::BindBufferObject( BufferType bufferType, unsigned int bufferID ) { glBindBuffer( bufferType, bufferID ); }

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::GenerateBuffer( unsigned int numberOfBuffersToGenerate, unsigned int* arrayOfBufferIDs )
{
	glGenBuffers( numberOfBuffersToGenerate, arrayOfBufferIDs );
}

//-----------------------------------------------------------------------------------------------
inline void OpenGLRenderer::SendDataToBuffer( BufferType bufferType, unsigned int sizeOfBufferBytes, const void* dataToSendToBuffer )
{
	glBufferData( bufferType, sizeOfBufferBytes, dataToSendToBuffer, GL_STATIC_DRAW );
}

#endif //INCLUDED_OPEN_GL_RENDERER_HPP
