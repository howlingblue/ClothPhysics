#ifndef INCLUDED_SANDBOX_HPP
#define INCLUDED_SANDBOX_HPP
#pragma once

//-----------------------------------------------------------------------------------------------
#include "../Engine/Input/Keyboard.hpp"
#include "../Engine/Input/Mouse.hpp"
#include "../Engine/Input/Xbox.hpp"
#include "../Engine/Camera.hpp"
#include "../Engine/Game.hpp"

//-----------------------------------------------------------------------------------------------
class Sandbox: public Game
{
	Camera m_camera;
	FloatVector3 m_lightPosition;

	bool m_drawOrigin;
	float m_totalRunTimeSeconds;

	void ConvertKeyboardToCameraInput( const Keyboard& keyboard, float& out_xyMovementAngleDegrees, float& out_xyMovementMagnitude, float& out_zMovementMagnitude );
	void ConvertMouseToCameraInput( const Mouse& mouse, float& out_xMovement, float& out_yMovement );
	float TransformKeyInputIntoAngleDegrees( bool upKeyIsPressed, bool rightKeyIsPressed, bool downKeyIsPressed, bool leftKeyIsPressed );

	void UpdatePlayerFromInput( float deltaSeconds, Keyboard& keyboard, const Mouse& mouse );

public:
	Sandbox( bool& quitVariable, unsigned int width, unsigned int height, float horizontalFOVDegrees );

	void Initialize();

	void RenderGame() const;
	void RenderUI() const;

	void GameUpdate( float deltaSeconds );
	void InputUpdate( float deltaSeconds, Keyboard& keyboard, const Mouse& mouse, const Xbox::Controller& controller );
};



//-----------------------------------------------------------------------------------------------
inline Sandbox::Sandbox( bool& quitVariable, unsigned int width, unsigned int height, float horizontalFOVDegrees )
	: Game( quitVariable, width, height, horizontalFOVDegrees )
	, m_camera( -2.f, 0.f, 0.f )
	, m_lightPosition( 1.f, 1.f, 1.f )
	, m_drawOrigin( false )
	, m_totalRunTimeSeconds( 0.f )
{ }

#endif //INCLUDED_SANDBOX_HPP
