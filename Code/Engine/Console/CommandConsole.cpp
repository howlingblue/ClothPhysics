#include <algorithm>
#include <iterator>
#include <sstream>
#include "CommandConsole.hpp"
#include "../Graphics/Renderer.hpp"
#include "../Graphics/VertexDataContainers.hpp"

#pragma region Command Console Const Variable Definitions
STATIC CommandConsole* CommandConsole::s_console = nullptr;
STATIC std::map< std::string, CommandConsole::ConsoleCommand > CommandConsole::s_commandRegistry;

STATIC const Color CommandConsole::BACKGROUND_PANE_COLOR = Color( 0.133333f, 0.156862f, 0.164705f, 0.4f );
STATIC const Color CommandConsole::BORDER_COLOR = Color( 0.f, 0.f, 1.f, 1.f );
STATIC const float CommandConsole::BORDER_THICKNESS = 7.f;
STATIC const float CommandConsole::TEXT_LINE_HEIGHT = 30.f;

STATIC const float CommandConsole::Prompt::CURSOR_BLINK_RATE_SECONDS = 0.75f;
#pragma endregion

//-----------------------------------------------------------------------------------------------
struct CommandConsole::CommandArguments
{
	std::string argumentsAsSingleString;
	std::vector< std::string > argumentsAsStringArray;

	CommandArguments() { }

	CommandArguments( const std::string& singleStringOfArguments )
		: argumentsAsSingleString( singleStringOfArguments )
	{
		//Assume that arguments are white-space delimited, so we can use a stringstream to parse
		std::istringstream argumentStream( singleStringOfArguments );

		//This wonderful piece of code courtesy of Zunino on Stack Overflow:
		//http://stackoverflow.com/questions/236129/how-to-split-a-string-in-c
		std::copy( std::istream_iterator< std::string >( argumentStream ), 
					std::istream_iterator< std::string >(),
					std::back_inserter< std::vector< std::string > >( argumentsAsStringArray ) );
	}
};

//-----------------------------------------------------------------------------------------------
struct CommandConsole::PaneColorData
{
	FloatVector2 vertex;
	Color paneColor;
	Color borderColor;

	PaneColorData() { }

	PaneColorData( const FloatVector2& consoleVertex, const Color& backPaneColor, const Color& borderWrapColor )
		: vertex( consoleVertex )
		, paneColor( backPaneColor )
		, borderColor( borderWrapColor )
	{ }
};

//-----------------------------------------------------------------------------------------------
void CommandConsole::ExecuteCommandInPrompt()
{
	std::string command;
	CommandArguments arguments;
	SplitCommandIntoCommandAndArguments( m_prompt.currentInput, command, arguments );

	std::map< std::string, ConsoleCommand >::const_iterator commandPairing = s_commandRegistry.find( command );
	if( commandPairing == s_commandRegistry.end() )
	{
		WriteTextToLog( "ERROR: Command " + command + " not found.", Color( 1.f, 0.f, 0.f, 1.f ) );
	}
	else
	{
		commandPairing->second( arguments );
	}

	ClearPrompt();
}

//-----------------------------------------------------------------------------------------------
void CommandConsole::GenerateConsolePaneVBO()
{
	static const unsigned int NUMBER_OF_PANE_VERTICES = 8;
	PaneColorData consolePaneVertices[ NUMBER_OF_PANE_VERTICES ] = 
	{
		PaneColorData( m_log.lowerLeftCorner, BACKGROUND_PANE_COLOR, BORDER_COLOR ),
		PaneColorData( FloatVector2( m_log.upperRightCorner.x, m_log.lowerLeftCorner.y ), BACKGROUND_PANE_COLOR, BORDER_COLOR ),
		PaneColorData( FloatVector2( m_log.lowerLeftCorner.x, m_log.upperRightCorner.y ), BACKGROUND_PANE_COLOR, BORDER_COLOR ),
		PaneColorData( m_log.upperRightCorner, BACKGROUND_PANE_COLOR, BORDER_COLOR ),

		PaneColorData( m_prompt.lowerLeftCorner, BACKGROUND_PANE_COLOR, BORDER_COLOR ),
		PaneColorData( FloatVector2( m_prompt.upperRightCorner.x, m_prompt.lowerLeftCorner.y ), BACKGROUND_PANE_COLOR, BORDER_COLOR ),
		PaneColorData( FloatVector2( m_prompt.lowerLeftCorner.x, m_prompt.upperRightCorner.y ), BACKGROUND_PANE_COLOR, BORDER_COLOR ),
		PaneColorData( m_prompt.upperRightCorner, BACKGROUND_PANE_COLOR, BORDER_COLOR )
	};

	static const unsigned int NUMBER_OF_PANE_INDICES = 9;
	unsigned short consolePaneIndices[ NUMBER_OF_PANE_INDICES ] = { 0, 1, 2, 3, 0xFFFF, 4, 5, 6, 7 };

	static const unsigned int NUMBER_OF_BORDER_INDICES = 9;
	unsigned short consoleBorderIndices[ NUMBER_OF_BORDER_INDICES ] = { 0, 1, 3, 2, 0xFFFF, 4, 5, 7, 6 };

	Renderer* renderer = Renderer::GetRenderer();

	renderer->GenerateBuffer( 1, &m_vertexBufferID );
	renderer->BindBufferObject( Renderer::ARRAY_BUFFER, m_vertexBufferID );
	renderer->SendDataToBuffer( Renderer::ARRAY_BUFFER, NUMBER_OF_PANE_VERTICES * sizeof( PaneColorData ), &consolePaneVertices[ 0 ] );

	renderer->GenerateBuffer( 1, &m_paneIndexBufferID );
	renderer->BindBufferObject( Renderer::INDEX_BUFFER, m_paneIndexBufferID);
	renderer->SendDataToBuffer( Renderer::INDEX_BUFFER, NUMBER_OF_PANE_INDICES * sizeof( unsigned short ), &consolePaneIndices[ 0 ] );

	renderer->GenerateBuffer( 1, &m_borderIndexBufferID );
	renderer->BindBufferObject( Renderer::INDEX_BUFFER, m_borderIndexBufferID);
	renderer->SendDataToBuffer( Renderer::INDEX_BUFFER, NUMBER_OF_BORDER_INDICES * sizeof( unsigned short ), &consoleBorderIndices[ 0 ] );

	renderer->BindBufferObject( Renderer::ARRAY_BUFFER, 0 );
}

//-----------------------------------------------------------------------------------------------
void CommandConsole::GenerateTextCursorVBO()
{
	static const Color CURSOR_COLOR = Color( 1.f, 1.f, 1.f, 1.f );
	static const unsigned int NUMBER_OF_CURSOR_VERTICES = 8;

	VertexColorData2D cursorVertices[ NUMBER_OF_CURSOR_VERTICES ] = 
	{
		VertexColorData2D( FloatVector2( 0.f, 0.f ), CURSOR_COLOR ),
		VertexColorData2D( FloatVector2( 0.f, TEXT_LINE_HEIGHT ), CURSOR_COLOR )
	};

	Renderer* renderer = Renderer::GetRenderer();

	renderer->GenerateBuffer( 1, &m_prompt.cursorVertexBufferID );
	renderer->BindBufferObject( Renderer::ARRAY_BUFFER, m_prompt.cursorVertexBufferID );
	renderer->SendDataToBuffer( Renderer::ARRAY_BUFFER, NUMBER_OF_CURSOR_VERTICES * sizeof( VertexColorData2D ), &cursorVertices[ 0 ] );

	renderer->BindBufferObject( Renderer::ARRAY_BUFFER, 0 );
}

//-----------------------------------------------------------------------------------------------
void CommandConsole::RenderBackgroundPanes() const
{
	Renderer* renderer = Renderer::GetRenderer();
	renderer->BindBufferObject( Renderer::ARRAY_BUFFER, m_vertexBufferID );
	renderer->BindBufferObject( Renderer::INDEX_BUFFER, m_paneIndexBufferID );

	static const int BEGINNING_OF_BUFFER = 0;
	static const int SIZE_OF_QUAD_ARRAY_STRUCTURE = sizeof( PaneColorData );
	static const int NUMBER_OF_VERTEX_COORDINATES = 2;
	renderer->SetPointerToVertexArray( NUMBER_OF_VERTEX_COORDINATES, Renderer::FLOAT_TYPE, SIZE_OF_QUAD_ARRAY_STRUCTURE, BEGINNING_OF_BUFFER );

	static const int NUMBER_OF_COLOR_COORDINATES = 4;
	renderer->SetPointerToColorArray( NUMBER_OF_COLOR_COORDINATES, Renderer::FLOAT_TYPE, SIZE_OF_QUAD_ARRAY_STRUCTURE, ( float* )offsetof( PaneColorData, paneColor ) );
	
	renderer->RenderPartOfArray( Renderer::TRIANGLE_STRIP, 9, Renderer::UNSIGNED_SHORT_TYPE, BEGINNING_OF_BUFFER );

	renderer->BindBufferObject( Renderer::INDEX_BUFFER, 0 );
	renderer->BindBufferObject( Renderer::ARRAY_BUFFER, 0 );
}

//-----------------------------------------------------------------------------------------------
void CommandConsole::RenderBorders() const
{
	Renderer* renderer = Renderer::GetRenderer();
	renderer->BindBufferObject( Renderer::ARRAY_BUFFER, m_vertexBufferID );
	renderer->BindBufferObject( Renderer::INDEX_BUFFER, m_borderIndexBufferID );

	static const int BEGINNING_OF_BUFFER = 0;
	static const int SIZE_OF_QUAD_ARRAY_STRUCTURE = sizeof( PaneColorData );
	static const int NUMBER_OF_VERTEX_COORDINATES = 2;
	renderer->SetPointerToVertexArray( NUMBER_OF_VERTEX_COORDINATES, Renderer::FLOAT_TYPE, SIZE_OF_QUAD_ARRAY_STRUCTURE, BEGINNING_OF_BUFFER );

	static const int NUMBER_OF_COLOR_COORDINATES = 4;
	renderer->SetPointerToColorArray( NUMBER_OF_COLOR_COORDINATES, Renderer::FLOAT_TYPE, SIZE_OF_QUAD_ARRAY_STRUCTURE, ( float* )offsetof( PaneColorData, borderColor ) );

	renderer->SetLineWidth( BORDER_THICKNESS );
	renderer->RenderPartOfArray( Renderer::LINE_LOOP, 9, Renderer::UNSIGNED_SHORT_TYPE, BEGINNING_OF_BUFFER );

	renderer->BindBufferObject( Renderer::INDEX_BUFFER, 0 );
	renderer->BindBufferObject( Renderer::ARRAY_BUFFER, 0 );
}

//-----------------------------------------------------------------------------------------------
void CommandConsole::RenderConsoleText() const
{
	static const float LINE_VERTICAL_SPACING = 5.f;

	Renderer* renderer = Renderer::GetRenderer();
	float textStartX = m_log.lowerLeftCorner.x + 10.f; //Get off the screen edge
	float currentTextLowerLeftY = m_log.lowerLeftCorner.y + 10.f; //Get above the border

	std::vector< TextLine >::const_reverse_iterator line;
	for( line = m_log.previousOutput.rbegin(); line != m_log.previousOutput.rend(); ++line )
	{
		renderer->Render2DText( line->text, m_font, TEXT_LINE_HEIGHT, FloatVector2( textStartX, currentTextLowerLeftY ), 
								line->textColor, line->backgroundColor, line->shadowColor );
		currentTextLowerLeftY += TEXT_LINE_HEIGHT + LINE_VERTICAL_SPACING;
	}




	FloatVector2 consoleInputStartLocation( m_prompt.lowerLeftCorner.x + 5.f, m_prompt.lowerLeftCorner.y + 5.f );
	renderer->Render2DText( "> " + m_prompt.currentInput, m_font, TEXT_LINE_HEIGHT, consoleInputStartLocation, Color( 1.f, 1.f, 0.f, 1.f ) );
}

//-----------------------------------------------------------------------------------------------
void CommandConsole::RenderTextCursor() const
{
	Renderer* renderer = Renderer::GetRenderer();
	renderer->PushMatrix();

	static const unsigned int LINE_START = 0;
	float xPosition = renderer->CalculateTextWidthFrom( "> " + m_prompt.currentInput, m_font, TEXT_LINE_HEIGHT, LINE_START, m_prompt.cursorLocation + 2 ) + 5.f;
	float yPosition = m_prompt.lowerLeftCorner.y + 5.f;		//start slightly below the baseline
	renderer->TranslateWorld( FloatVector3( xPosition, yPosition, 0.f ) );

	renderer->SetAlphaBlendingFunction( Renderer::ONE_MINUS_DESTINATION_COLOR, Renderer::NO_COLOR );
	renderer->BindBufferObject( Renderer::ARRAY_BUFFER, m_prompt.cursorVertexBufferID );

	static const int BEGINNING_OF_BUFFER = 0;
	static const int SIZE_OF_LINE_ARRAY_STRUCTURE = sizeof( VertexColorData2D );
	static const int NUMBER_OF_VERTEX_COORDINATES = 2;
	renderer->SetPointerToVertexArray( NUMBER_OF_VERTEX_COORDINATES, Renderer::FLOAT_TYPE, SIZE_OF_LINE_ARRAY_STRUCTURE, BEGINNING_OF_BUFFER );

	static const int NUMBER_OF_COLOR_COORDINATES = 4;
	renderer->SetPointerToColorArray( NUMBER_OF_COLOR_COORDINATES, Renderer::FLOAT_TYPE, SIZE_OF_LINE_ARRAY_STRUCTURE, ( float* )offsetof( VertexColorData2D, red ) );

	static const unsigned int NUMBER_OF_CURSOR_VERTICES = 2;
	renderer->SetLineWidth( 1.f );

	renderer->RenderVertexArray( Renderer::LINES, BEGINNING_OF_BUFFER, NUMBER_OF_CURSOR_VERTICES );

	renderer->BindBufferObject( Renderer::ARRAY_BUFFER, 0 );
	renderer->SetAlphaBlendingFunction( Renderer::SOURCE_ALPHA, Renderer::ONE_MINUS_SOURCE_ALPHA );
	renderer->PopMatrix();
}

//-----------------------------------------------------------------------------------------------
void CommandConsole::SplitCommandIntoCommandAndArguments( const std::string& commandLine, std::string& out_command, CommandArguments& out_arguments ) const
{
	std::size_t firstWhiteSpaceLocation = commandLine.find( " " );

	if( firstWhiteSpaceLocation == std::string::npos )
	{
		out_command.assign( commandLine );
	}
	else
	{
		out_command.assign( commandLine.begin(), commandLine.begin() + firstWhiteSpaceLocation );

		//skip the white space at the start of the arguments
		out_arguments = CommandArguments( std::string( commandLine.begin() + firstWhiteSpaceLocation + 1, commandLine.end() ) );
	}
}

//-----------------------------------------------------------------------------------------------
void CommandConsole::Render() const
{
	Renderer* renderer = Renderer::GetRenderer();

	renderer->EnableFeature( Renderer::SHAPE_RESTART_INDEXING );
	renderer->EnableArrayType( Renderer::VERTEX_ARRAYS );
	renderer->EnableArrayType( Renderer::COLOR_ARRAYS );

	const ShaderProgram* activeShader = renderer->GetActiveShaderProgram();
	unsigned int shaderTextureToggleID = renderer->GetUniformVariableLocation( activeShader, "u_texturesEnabled" );
	renderer->SetUniformVariable( shaderTextureToggleID, 0 );

	RenderBackgroundPanes();

	renderer->SetUniformVariable( shaderTextureToggleID, 1 );

	RenderConsoleText();

	renderer->SetUniformVariable( shaderTextureToggleID, 0 );

	renderer->EnableArrayType( Renderer::VERTEX_ARRAYS );
	renderer->EnableArrayType( Renderer::COLOR_ARRAYS );

	RenderBorders();

	if( m_prompt.cursorIsShowing )
		RenderTextCursor();

	renderer->SetUniformVariable( shaderTextureToggleID, 1 );

	renderer->DisableArrayType( Renderer::COLOR_ARRAYS );
	renderer->DisableArrayType( Renderer::VERTEX_ARRAYS );
	renderer->DisableFeature( Renderer::SHAPE_RESTART_INDEXING );
}

//-----------------------------------------------------------------------------------------------
void CommandConsole::Update( float deltaSeconds )
{
	m_prompt.secondsSinceCursorBlinkChange += deltaSeconds;
	if( m_prompt.secondsSinceCursorBlinkChange > Prompt::CURSOR_BLINK_RATE_SECONDS )
	{
		m_prompt.cursorIsShowing = !m_prompt.cursorIsShowing;
		m_prompt.secondsSinceCursorBlinkChange = 0.f;
	}
}