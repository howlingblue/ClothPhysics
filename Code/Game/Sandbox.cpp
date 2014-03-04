#include "../Engine/DebugDrawing.hpp"
#include "../Engine/Font/BitmapFont.hpp"
#include "Sandbox.hpp"

//-----------------------------------------------------------------------------------------------
void Sandbox::ConvertKeyboardToCameraInput( const Keyboard& keyboard, float& out_xyMovementAngleDegrees, float& out_xyMovementMagnitude, float& out_zMovementMagnitude )
{
	//Movement
	if ( keyboard.KeyIsPressedOrHeld( Keyboard::W ) || keyboard.KeyIsPressedOrHeld( Keyboard::D ) || keyboard.KeyIsPressedOrHeld( Keyboard::S ) || keyboard.KeyIsPressedOrHeld( Keyboard::A ) ) 
	{
		out_xyMovementMagnitude = 1.f;
		out_xyMovementAngleDegrees = TransformKeyInputIntoAngleDegrees( keyboard.KeyIsPressedOrHeld( Keyboard::W ), keyboard.KeyIsPressedOrHeld( Keyboard::D ),
																		keyboard.KeyIsPressedOrHeld( Keyboard::S ), keyboard.KeyIsPressedOrHeld( Keyboard::A ) );
	}
	else
		out_xyMovementMagnitude = 0.f;

	if ( keyboard.KeyIsPressedOrHeld( Keyboard::Q ) || keyboard.KeyIsPressedOrHeld( Keyboard::E ) ) 
	{
		if( keyboard.KeyIsPressedOrHeld( Keyboard::Q ) )
			out_zMovementMagnitude = 1.f;
		else
			out_zMovementMagnitude = -1.f;
	}
	else
		out_zMovementMagnitude = 0.f;
}

//-----------------------------------------------------------------------------------------------
void Sandbox::ConvertMouseToCameraInput( const Mouse& mouse, float& out_xMovement, float& out_yMovement )
{
	static const float INVERSE_SCREEN_WIDTH  = 1.f / m_screenWidth;
	static const float INVERSE_SCREEN_HEIGHT = 1.f / m_screenHeight;
	static const float MOUSE_MULTIPLIER = 20.f;

	//Looking
	out_xMovement = -mouse.GetMouseMovement().x * MOUSE_MULTIPLIER * INVERSE_SCREEN_WIDTH;
	out_yMovement = mouse.GetMouseMovement().y * MOUSE_MULTIPLIER * INVERSE_SCREEN_HEIGHT;
}

//-----------------------------------------------------------------------------------------------
float Sandbox::TransformKeyInputIntoAngleDegrees( bool upKeyIsPressed, bool rightKeyIsPressed, bool downKeyIsPressed, bool leftKeyIsPressed )
{
	static int upDirectionValue = 2, rightDirectionValue = 2, downDirectionValue = 2, leftDirectionValue = 2;

	if( upKeyIsPressed ) 
		upDirectionValue = 1;
	else 
		upDirectionValue = 0;

	if( rightKeyIsPressed ) 
		rightDirectionValue = 1;
	else 
		rightDirectionValue = 0;

	if( downKeyIsPressed ) 
		downDirectionValue = -1;
	else 
		downDirectionValue = 0;

	if( leftKeyIsPressed ) 
		leftDirectionValue = -1;
	else 
		leftDirectionValue = 0;

	float desiredOrientationRadians = atan2f( static_cast<float>( upDirectionValue + downDirectionValue ), 
		static_cast<float>( rightDirectionValue + leftDirectionValue ) );
	float desiredOrientationDegrees = ConvertRadiansToDegrees( desiredOrientationRadians );
	return desiredOrientationDegrees;
}

//-----------------------------------------------------------------------------------------------
void Sandbox::UpdatePlayerFromInput( float deltaSeconds, Keyboard& keyboard, const Mouse& mouse )
{	
	float changeInCameraYawAboutZDegrees = 0.f;
	float changeInCameraPitchAboutYDegrees = 0.f;

	ConvertMouseToCameraInput( mouse, changeInCameraYawAboutZDegrees, changeInCameraPitchAboutYDegrees );
	m_camera.RotateYawBy( changeInCameraYawAboutZDegrees, deltaSeconds );
	m_camera.RotatePitchBy( changeInCameraPitchAboutYDegrees, deltaSeconds );

	float xyMovementHeadingDegrees = 0.f;
	float xyMovementMagnitude = 0.f;
	float zMovementMagnitude = 0.f;

	ConvertKeyboardToCameraInput( keyboard, xyMovementHeadingDegrees, xyMovementMagnitude, zMovementMagnitude );

	static const float CAMERA_MOVEMENT_SPEED = 5.f;
	float adjustedXYAngleDegrees = xyMovementHeadingDegrees + m_camera.GetHeading().yawDegreesAboutZ - 90.f;

	FloatVector3 directionVector( cos( ConvertDegreesToRadians( adjustedXYAngleDegrees ) ), 
								  sin( ConvertDegreesToRadians( adjustedXYAngleDegrees ) ), 
								  0.f );

	FloatVector3 movementVector( ( directionVector ) * xyMovementMagnitude * CAMERA_MOVEMENT_SPEED * deltaSeconds );
	movementVector.z = zMovementMagnitude * CAMERA_MOVEMENT_SPEED * deltaSeconds;

	m_camera.MoveByVector( movementVector );

	if( keyboard.KeyIsPressed( Keyboard::O ) )
		m_drawOrigin = !m_drawOrigin;
}

//-----------------------------------------------------------------------------------------------
void Sandbox::Initialize()
{
	Game::Initialize();
}

//-----------------------------------------------------------------------------------------------
void Sandbox::RenderGame() const
{
	m_camera.ViewWorldThrough();

	m_cloth.Render();

	Debug::DrawPoint( m_lightPosition, 1.f, Color( 1.f, 1.f, 1.f, 1.f ), Debug::Drawing::DRAW_ONLY_IF_VISIBLE );
}

//-----------------------------------------------------------------------------------------------
void Sandbox::RenderUI() const
{
}

//-----------------------------------------------------------------------------------------------
void Sandbox::GameUpdate( float deltaSeconds )
{
	if( m_drawOrigin )
		Debug::DrawAxes( FloatVector3( 0.f, 0.f, 0.f ), 1.f, Debug::Drawing::DRAW_ALWAYS );

	m_cloth.Update( deltaSeconds );

	m_totalRunTimeSeconds += deltaSeconds;
}

//-----------------------------------------------------------------------------------------------
void Sandbox::InputUpdate( float deltaSeconds, Keyboard& keyboard, const Mouse& mouse, const Xbox::Controller& )
{
	if( keyboard.KeyIsPressed( Keyboard::ESCAPE ) )
		m_quitVariable = true;

	if( keyboard.KeyIsPressed( Keyboard::NUMBER_1 ) )
		m_cloth.SetWindForce( FloatVector3( 0.f, 0.f, 0.f ) );
	if( keyboard.KeyIsPressed( Keyboard::NUMBER_2 ) )
		m_cloth.SetWindForce( FloatVector3( 0.3f, 0.1f, 0.1f ) );
	if( keyboard.KeyIsPressed( Keyboard::NUMBER_3 ) )
		m_cloth.SetWindForce( FloatVector3( 0.f, 0.f, 0.1f ) );

	UpdatePlayerFromInput( deltaSeconds, keyboard, mouse );
}

