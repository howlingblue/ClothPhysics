#include "../Engine/Graphics/Renderer.hpp"
#include "../Engine/DebugDrawing.hpp"
#include "Game.hpp"

//-----------------------------------------------------------------------------------------------
void Game::UpdateConsoleInput( float, Keyboard& keyInput, const Mouse& )
{
	if( keyInput.KeyIsPressed( Keyboard::BACKSPACE ) )
		m_console->RemoveLetterLeftOfCursorPosition();

	if( keyInput.KeyIsPressed( Keyboard::DELETE_KEY ) )
		m_console->RemoveLetterRightOfCursorPosition();

	if( keyInput.KeyIsPressed( Keyboard::ESCAPE ) )
	{
		if( m_console->PromptIsEmpty() )
			m_consoleVisible = false;
		else
			m_console->ClearPrompt();
	}

	if( keyInput.KeyIsPressed( Keyboard::ENTER ) )
	{
		if( m_console->PromptIsEmpty() )
			m_consoleVisible = false;
		else
			m_console->ExecuteCommandInPrompt();
	}


	//Cursor Movement
	if( keyInput.KeyIsPressed( Keyboard::ARROW_LEFT ) )
		m_console->MovePromptCursorLeft();

	if( keyInput.KeyIsPressed( Keyboard::ARROW_RIGHT ) )
		m_console->MovePromptCursorRight();

	if( keyInput.KeyIsPressed( Keyboard::HOME ) )
		m_console->MovePromptCursorToStart();

	if( keyInput.KeyIsPressed( Keyboard::END ) )
		m_console->MovePromptCursorToEnd();

	//Character Input
	while( !keyInput.InputQueueIsEmpty() )
	{
		m_console->AddCharacterToPrompt( keyInput.GetCharacterFromInputQueue() );
	}
}

//-----------------------------------------------------------------------------------------------
Game::Game( bool& quitVariable, unsigned int screenWidth, unsigned int screenHeight, float horizontalFOVDegrees )
	: m_quitVariable( quitVariable )
	, m_screenWidth( screenWidth )
	, m_screenHeight( screenHeight )
	, m_aspectRatio( static_cast< float >( m_screenWidth ) / static_cast< float >( m_screenHeight ) )
	, m_horizontalFOVDegrees( horizontalFOVDegrees )
	, m_console( nullptr )
	, m_consoleVisible( false )
{ }

//-----------------------------------------------------------------------------------------------
void ClearConsoleLog( const CommandConsole::CommandArguments& )
{
	CommandConsole* console = CommandConsole::GetConsole();

	console->ClearLog( );
}

//-----------------------------------------------------------------------------------------------
void Game::Initialize() 
{
	Debug::InitializeDrawingSystem();

	CommandConsole::CreateConsole
		( 
			FloatVector2( 0.f,									static_cast<float>( m_screenHeight ) * .25f ), 
			FloatVector2( static_cast<float>( m_screenWidth ), static_cast<float>( m_screenHeight ) ),
			FloatVector2( 0.f,									0.f ),
			FloatVector2( static_cast<float>( m_screenWidth ), static_cast<float>( m_screenHeight ) * .05f ) 
		);
	m_console = CommandConsole::GetConsole();

	CommandConsole::RegisterConsoleCommand( "clear", ClearConsoleLog );
}

//-----------------------------------------------------------------------------------------------
void Game::Render() const
{
	static const double NEAR_CLIPPING_PLANE_DISTANCE = 0.1;
	static const double FAR_CLIPPING_PLANE_DISTANCE = 1000.0;

	Renderer* renderer = Renderer::GetRenderer();

	renderer->PushMatrix();

	renderer->SetPerpectiveProjection( m_horizontalFOVDegrees, m_aspectRatio, NEAR_CLIPPING_PLANE_DISTANCE, FAR_CLIPPING_PLANE_DISTANCE );

	RenderGame();
	Debug::RenderDrawings();

	renderer->PopMatrix();

	renderer->PushMatrix();

	renderer->SetOrthographicProjection( 0.0, static_cast< double >( m_screenWidth ), 0.0, static_cast< double >( m_screenHeight ), 0.0, FAR_CLIPPING_PLANE_DISTANCE );
	renderer->DisableFeature( Renderer::DEPTH_TESTING );
	renderer->DisableDepthBufferWriting();

	RenderUI();

	if( m_consoleVisible )
		m_console->Render();

	renderer->EnableDepthBufferWriting();
	renderer->EnableFeature( Renderer::DEPTH_TESTING );

	renderer->PopMatrix();
}

//-----------------------------------------------------------------------------------------------
void Game::Update( float deltaSeconds, Keyboard& keyInput, const Mouse& mouseInput, const Xbox::Controller& xboxInput )
{
	Debug::UpdateDrawings( deltaSeconds );

	if( keyInput.KeyIsPressed( Keyboard::GRAVE ) )
		m_consoleVisible = !m_consoleVisible;

	if( m_consoleVisible )
	{
		UpdateConsoleInput( deltaSeconds, keyInput, mouseInput );
		m_console->Update( deltaSeconds );
	}
	else
	{
		InputUpdate( deltaSeconds, keyInput, mouseInput, xboxInput );
	}

	GameUpdate( deltaSeconds );
}