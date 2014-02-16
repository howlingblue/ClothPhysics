#ifndef INCLUDED_MATERIAL_HPP
#define INCLUDED_MATERIAL_HPP
#pragma once

//-----------------------------------------------------------------------------------------------
#include <string>
#include "Renderer.hpp"
#include "ShaderProgram.hpp"

//-----------------------------------------------------------------------------------------------
class Material
{
public:
	struct ArrayInfo
	{
	private:
		friend class Material;
		int attributeID;

	public:
		Renderer::CoordinateType coordinateType;
		unsigned int numberOfVertexCoordinates;
		size_t		 sizeOfVertexArrayStructure;
		const void*	 vertexArrayStartLocation;

		ArrayInfo()
			: attributeID( -1 )
			, coordinateType( Renderer::FLOAT_TYPE )
			, numberOfVertexCoordinates( 0 )
			, sizeOfVertexArrayStructure( 0 )
			, vertexArrayStartLocation( NULL )
		{ }

	};

	struct TextureInfo
	{
		int textureUnitID;
		int samplerUniformID;
		Texture* texture;
	};
private:
	const ShaderProgram* m_program;

	int m_modelMatrixUniformLocation;
	int m_viewMatrixUniformLocation;
	int m_projectionMatrixUniformLocation;
	unsigned int m_vertexBufferID;
	unsigned int m_indexBufferID;

	std::vector< TextureInfo > m_infoForTextures;
	std::vector< ArrayInfo > m_genericArrays;
	std::vector< Renderer::Feature > m_featuresToEnableBeforeRender;
	std::vector< Renderer::Feature > m_featuresToDisableBeforeRender;

public:

	Material()
		: m_program( nullptr )
		, m_modelMatrixUniformLocation( -1 )
		, m_viewMatrixUniformLocation( -1 )
		, m_projectionMatrixUniformLocation( -1 )
		, m_vertexBufferID( 0 )
		, m_indexBufferID( 0 )
	{ }

	void Apply( Renderer* renderer );
	void Remove( Renderer* renderer );

	const ShaderProgram* GetShaderProgram() const { return m_program; }
	bool SetArrayOntoAttribute( const std::string& attributeName, const ArrayInfo& arrayInfo );
	void SetFeatureToDisableBeforeRender( Renderer::Feature feature ) { m_featuresToDisableBeforeRender.push_back( feature ); }
	void SetFeatureToEnableBeforeRender( Renderer::Feature feature )  { m_featuresToEnableBeforeRender.push_back( feature ); }
	bool SetFloatUniform( const std::string& uniformName, float value );
	bool SetFloatUniform( const std::string& uniformName, const FloatVector3& vector );
	bool SetFloatUniform( const std::string& uniformName, const FloatVector4& vector );
	void SetIndexBufferID( unsigned int iboID ) { m_indexBufferID = iboID; }
	bool SetIntegerUniform( const std::string& uniformName, int value );
	bool SetModelMatrixUniform( const std::string& uniformName );
	bool SetViewMatrixUniform( const std::string& uniformName );
	bool SetProjectionMatrixUniform( const std::string& uniformName );
	void SetShaderProgram( const ShaderProgram* shaderProgram ) { m_program = shaderProgram; }
	bool SetTextureUniform( const std::string& uniformName, int textureUnitID, const std::string& textureFileLocation );
	void SetVertexBufferID( unsigned int vboID ) { m_vertexBufferID = vboID ; }
};

//-----------------------------------------------------------------------------------------------
inline void Material::Apply( Renderer* renderer )
{
	renderer->UseShaderProgram( m_program );

	renderer->SetUniformVariable( m_modelMatrixUniformLocation, renderer->GetModelMatrix() );
	renderer->SetUniformVariable( m_viewMatrixUniformLocation, renderer->GetViewMatrix() );
	renderer->SetUniformVariable( m_projectionMatrixUniformLocation, renderer->GetProjectionMatrix() );

	for( int i = 0; i < (int)m_infoForTextures.size(); ++i )
	{
		TextureInfo& texInfo = m_infoForTextures[ i ];
		renderer->SetActiveTextureUnit( texInfo.textureUnitID );
		renderer->BindTexture( Renderer::TEXTURES_2D, texInfo.texture );
		renderer->SetUniformVariable( texInfo.samplerUniformID, texInfo.textureUnitID );
	}

	renderer->BindBufferObject( Renderer::ARRAY_BUFFER, m_vertexBufferID );
	renderer->BindBufferObject( Renderer::INDEX_BUFFER, m_indexBufferID );

	for( unsigned int i = 0; i < m_featuresToEnableBeforeRender.size(); ++i )
	{
		renderer->EnableFeature( m_featuresToEnableBeforeRender[ i ] );
	}
	
	for( unsigned int i = 0; i < m_featuresToDisableBeforeRender.size(); ++i )
	{
		renderer->DisableFeature( m_featuresToDisableBeforeRender[ i ] );
	}

	for( unsigned int i = 0; i < m_genericArrays.size(); ++i )
	{
		const ArrayInfo& genericArray = m_genericArrays[ i ];

		renderer->BindVertexArraysToAttributeLocation( genericArray.attributeID );
		renderer->SetPointerToGenericArray( genericArray.attributeID, genericArray.numberOfVertexCoordinates, genericArray.coordinateType, 
											false, genericArray.sizeOfVertexArrayStructure, genericArray.vertexArrayStartLocation );
	}
}

//-----------------------------------------------------------------------------------------------
inline void Material::Remove( Renderer* renderer )
{
	for( unsigned int i = 0; i < m_genericArrays.size(); ++i )
	{
		const ArrayInfo& genericArray = m_genericArrays[ i ];

		renderer->UnbindVertexArraysFromAttributeLocation( genericArray.attributeID );
	}
}

//-----------------------------------------------------------------------------------------------
inline bool Material::SetArrayOntoAttribute( const std::string& attributeName, const ArrayInfo& arrayInfo )
{
	int attributeID = m_program->GetAttributeIDFromName( attributeName );
	if( attributeID == -1 )
		return false;

	m_genericArrays.push_back( arrayInfo );
	m_genericArrays.back().attributeID = attributeID;
	return true;
}

//-----------------------------------------------------------------------------------------------
inline bool Material::SetFloatUniform( const std::string& uniformName, float value )
{
	int uniformID = m_program->GetUniformVariableIDFromName( uniformName );
	if( uniformID == -1 )
		return false;

	Renderer* renderer = Renderer::GetRenderer();
	renderer->SetUniformVariable( uniformID, value );
	return true;
}

//-----------------------------------------------------------------------------------------------
inline bool Material::SetFloatUniform( const std::string& uniformName, const FloatVector3& vector )
{
	int uniformID = m_program->GetUniformVariableIDFromName( uniformName );
	if( uniformID == -1 )
		return false;

	Renderer* renderer = Renderer::GetRenderer();
	renderer->SetUniformVariable( uniformID, vector );
	return true;
}

//-----------------------------------------------------------------------------------------------
inline bool Material::SetFloatUniform( const std::string& uniformName, const FloatVector4& vector )
{
	int uniformID = m_program->GetUniformVariableIDFromName( uniformName );
	if( uniformID == -1 )
		return false;

	Renderer* renderer = Renderer::GetRenderer();
	renderer->SetUniformVariable( uniformID, vector );
	return true;
}

//-----------------------------------------------------------------------------------------------
inline bool Material::SetIntegerUniform( const std::string& uniformName, int value )
{
	int uniformID = m_program->GetUniformVariableIDFromName( uniformName );
	if( uniformID == -1 )
		return false;

	Renderer* renderer = Renderer::GetRenderer();
	renderer->SetUniformVariable( uniformID, value );
	return true;
}

//-----------------------------------------------------------------------------------------------
inline bool Material::SetModelMatrixUniform( const std::string& uniformName )
{
	int uniformID = m_program->GetUniformVariableIDFromName( uniformName );
	if( uniformID == -1 )
		return false;

	m_modelMatrixUniformLocation = uniformID;
	return true;
}

//-----------------------------------------------------------------------------------------------
inline bool Material::SetViewMatrixUniform( const std::string& uniformName )
{
	int uniformID = m_program->GetUniformVariableIDFromName( uniformName );
	if( uniformID == -1 )
		return false;

	m_viewMatrixUniformLocation = uniformID;
	return true;
}

//-----------------------------------------------------------------------------------------------
inline bool Material::SetProjectionMatrixUniform( const std::string& uniformName )
{
	int uniformID = m_program->GetUniformVariableIDFromName( uniformName );
	if( uniformID == -1 )
		return false;

	m_projectionMatrixUniformLocation = uniformID;
	return true;
}

//-----------------------------------------------------------------------------------------------
inline bool Material::SetTextureUniform( const std::string& uniformName, int textureUnitID, const std::string& textureFileLocation )
{
	int uniformID = m_program->GetUniformVariableIDFromName( uniformName );
	if( uniformID == -1 )
		return false;

	Renderer* renderer = Renderer::GetRenderer();
	TextureInfo texInfo;

	texInfo.textureUnitID = textureUnitID;
	renderer->SetActiveTextureUnit( textureUnitID );

	texInfo.texture = Texture::CreateOrGetTexture( textureFileLocation, Texture::linearInterpolation, Texture::clampToEdge );
	renderer->SetUniformVariable( uniformID, textureUnitID );

	texInfo.samplerUniformID = uniformID;
	m_infoForTextures.push_back( texInfo );

	return true;
}

#endif //INCLUDED_MATERIAL_HPP
